# `Task` (ленивая `Future`)

## Пререквизиты

- [futures/futures](/tasks/futures/futures)
- [stackless/gorroutines](/tasks/stackless/gorroutines)

---

Рассмотрим пример:

```cpp
futures::Future<int> Compute(executors::ThreadPool& pool) {
  // Перепланируемся в пул потоков
  co_await tasks::TeleportTo(pool);
  // Асинхронно вычисляем ответ
  co_return 42;
}

executors::ThreadPool pool{4};

// Похоже на `futures::Execute`?
futures::Future<int> f = Compute(pool);
std::cout << "Value = " << futures::GetValue(std::move(f)) << std::endl;
```

В точке вызова (точнее, старта) корутина `Compute` не может сразу же вернуть caller-у значение типа `int` (оно будет вычислено асинхронно в пуле потоков), поэтому вызов `Compute(pool)` возвращает представление этого будущего значения – `Future<int>`:

```cpp
// Примерно такой код сгенерирует компилятор:
futures::Future<int> Compute(executors::ThreadPool& pool) {
  // Аллоцируем coroutine state
  // Он скрыт от пользователя корутин
  auto* coro = new CoroutineState{pool};
  // С помощью promise type 
  // (который является полем coroutine state-а) 
  // строим мгновенный результат - std::future<int>
  auto future = coro->GetReturnObject();
  // Делаем первый шаг корутины
  coro->Start();
  return future;
}
```

См. [Hand-crafted stackless coroutine](https://gitlab.com/Lipovsky/stackless_examples/-/blob/master/state_machine_4/main.cpp)

Но фьюча приносит с собой дополнительные накладные расходы:
- Динамическая аллокация shared state
- Атомарный подсчет ссылок на shared state
- Синхронизация на shared state из-за гонки между продьюсером и консьюмером

Наша задача – избежать этих накладных расходов, заменив для корутин `Future<T>` на `Task<T>`: [Add coroutine task type](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1056r0.html)

## `Task<T>`

Вместо фьючи корутина будет возвращать `tasks::Task<T>`:

```cpp
tasks::Task<int> Compute(executors::ThreadPool& pool) {
  co_await tasks::TeleportTo(pool);  // Перепланируемся в пул потоков
  // <-- Теперь мы исполняемся в потоке пула
  
  // Асинхронно (относительно caller-а корутины)
  // вычисляем ответ
  co_return 42;
}
```

Класс `Task` через свой promise type определяет, что при вызове корутина должна остановиться _перед_ исполнением пользовательского кода, в служебной точке `co_await promise.initial_suspend()`:

```cpp
tasks::Task<int> task = Compute(pool);
// <- В этой точке вычисление еще не запланировано
// в пул потоков
```

После вызова корутины уже аллоцирован coroutine state, caller получил представление будущего результата в виде объекта `Task`, но сама асинхронная операция еще не стартовала.

### Планирование

Созданную задачу можно запланировать в пул потоков:

```cpp
tasks::Task<int> Compute() {
  co_return 42;
}

auto task = tasks::Schedule(pool, Compute());
```

### Распаковка

Асинхронная операция запускается caller-ом корутины явно в точке "распаковки" `Task`. Сделать это можно двумя способами:

#### 1. `co_await`

Первый способ "распаковать" `Task` – самому стать корутиной:
```cpp
tasks::Task<> RedCaller(executors::ThreadPool& pool) {
  Task<int> task = Compute(pool);
  // 1) Подписываемся на завершение задачи,
  // 2) стартуем корутину `task`-а
  // 3) останавливаемся.
  int value = co_await task;
  
  std::cout << "Value = " << value << std::endl;
}
```

#### 2. `Await`

Альтернативный способ – провести границу между [красными и синими вызовами](https://journal.stuffwithstuff.com/2015/02/01/what-color-is-your-function/) с помощью блокирующего вызова `Await` и тем самым остановить синтаксическое заражение:
```cpp
void BlueCaller(executors::ThreadPool& pool) {
  Task<int> task = Compute(pool);
  // Стартуем корутину и блокируем поток до ее завершения.
  int value = tasks::Run(std::move(task));
  std::cout << "Value = " << value << std::endl;
}

int main() {
  // Функция main не может быть корутиной,
  // использовать co_await в ней нельзя.
  BlueCaller();
}
```

### Profit

#### Аллокация

Нам не нужна отдельная динамическая аллокация для shared state, у нас уже есть аллоцированный на куче coroutine state (причем компилятор может стереть эту аллокацию, если будет уверен, что она не нужна).

#### Подсчет ссылок

Нам не нужен подсчет ссылок для контроля времени жизни shared state / coroutine state: `co_await task` означает, что время жизни корутины-caller-а (которая играет роль консьюмера) покрывает время жизни корутины-callee (которая является продьюсером).

#### Синхронизация

Старт асинхронной операции отложенный, им управляет caller, а значит он может подписать продолжение (continuation) на этот результат без гонки, т.е. без синхронизации.

## Fork-join

Пусть теперь мы хотим написать на корутинах рекурсивный алгоритм, использующий технику разделяй-и-властвуй.

Пока `Task` не позволяет нам параллельно запустить две подзадачи:
```cpp
tasks::Task<int> Fin(int n) {
  if (n > 2) {
    Task<int> t1 = Fib(n - 1);
    // <- Пока задачи не запущены!

    int f1 = co_await t1;  // =(
    int f2 = Fib(n - 2);
    return f1 + f2;
    
  } else {
    return 1;
  }
}  

```

Мы хотим разделить ожидание завершения и старт асинхронной операции:
```cpp
Task<int> t1 = Fib(n - 1);
Task<int> t2 = Fib(n - 2);

Start(t1);
Start(t2);

int f1 = co_await t1;
int f2 = co_await t2;

return f1 + f2;
```

Заметим, что одну из подзадач можно исполнить синхронно:

```cpp
Task t1 = Fib(n - 1);
Start(t1);  // Стартуем асинхронное исполнение первой подзадачи
int f2 = Fib(n - 2);  // Синхронно выполняем вторую подзадачу
int f1 = co_await t1;  // Дожидаемся первой подзадачи
return f1 + f2;
```


## Задача

1) Реализуйте [`JoinHandle`](task/join_handle.hpp) – синоним для `Task<void>`
2) Реализуйте [`Task<T>`](task/task.hpp)
3) Поддержите `Task<void>` и выразите `JoinHandle` через него.

### Требования

- Реализация не должна явно аллоцировать память на куче (помимо скрытых аллокаций coroutine state-а, которые выполняет сам компилятор)
- [Опционально] Реализация не должна требовать синхронизации caller-а и callee

## Замечания по реализации

Вам потребуется реализовать:

- `Task<T>` – представление будущего результата
- promise type для корутины, которая возвращает `Task<T>`
- awaiter для `co_await task`

Используйте готовый awaiter [`suspend_always`](https://en.cppreference.com/w/cpp/coroutine/suspend_always) для `initial_suspend`.

Получить [`coroutine_handle`](https://en.cppreference.com/w/cpp/coroutine/coroutine_handle) для текущей корутины из `promise_type` можно с помощью метода [`from_promise`](https://en.cppreference.com/w/cpp/coroutine/coroutine_handle/from_promise)

Используйте `wheels::Result<T>` для передачи значения / исключения между caller / callee.

`Task` / `JoinHandle` не должны зависеть от пула потоков. Использование `Task` не требует исполнения корутины в пуле.

## References

- [Asymmetric Transfer](https://lewissbaker.github.io/)
- [cppreference / Coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- [dcl.fct.def.coroutine](https://eel.is/c++draft/dcl.fct.def.coroutine), [expr.await](https://eel.is/c++draft/expr.await#:co_await)
