# Безопасный мьютекс

## Счетчик

Рассмотрим игрушечную реализацию атомарного счетчика:

```cpp
class AtomicCounter {
 public:
  void Increment() {
    std::lock_guard guard(mutex_);
    // Мы в критической секции
    ++value_;
  }
  
  size_t GetValue() const {
    // Забыли захватить мьютекс );
    return value_;
  }
  
 private:
  std::mutex mutex_;  // Защищает доступ к полю `value_`
  size_t value_{0};
};

```

(Этот класс можно заменить на `std::atomic` c методами `fetch_add` + `load`, но можно сделать и еще лучше...)

## Data race

Легко заметить ошибку: разработчик забыл захватить мьютекс в методе `GetValue`. 

В результате при работе со счетчиком из разных потоков чтения и записи переменной `value_` не будут упорядочены синхронизацией. Такая ситуация имеет специальное название – [_data race_](https://eel.is/c++draft/intro.races#def:data_race). 

Программа на С++, содержащая data race, является некорректной: поведение такой программы не определено семантикой языка, это UB.

## Причина

По замыслу мьютекс `mutex_` в классе `AtomicCounter` должен защищать доступ к полю `value_`. 
Но этот замысел существовал только в уме автора этого класса, а в коде остался не выражен, так что компилятор ничего не знает про связь полей `mutex_` и `value_`, а значит не может обнаружить ошибку.

## Решение – `Mutexed<T>`

Придумаем более безопасное API для взаимного исключения, которое будет гарантировать, что к разделяемому объекту _невозможно_ обратиться не владея при этом мьютексом.

Для этого свяжем объект, разделяемый между потоками, и мьютекс, который будет его защищать, с помощью класса – `Mutexed<T>`.

У класса `Mutexed` есть единственный метод `Acquire`, который возвращает пользователю `OwnerRef` – эксклюзивную ссылку на защищаемый объект:

```cpp
Mutexed<std::vector<int>> ints; // vector<int> + mutex

{
  // Захватываем владение вектором:
  auto onwer_ref = ints.Acquire();
  // Две вставки выполняются атомарно!
  onwer_ref->push_back(42);
  owner_ref->push_back(43);
}  // owner_ref разрушается -> мьютекс освобождается
```

Класс `Mutexed` гарантирует, что два потока не могут одновременно иметь в своем распоряжении ссылку на защищаемый объект: существование одной ссылки исключает существование других.

### `OwnerRef`

#### Вызов методов

```cpp
Mutexed<std::vector<int>> shared_vector;

{
  auto owner_ref = shared_vector.Acquire();
  // Вызываем метод вектора через ->
  owner_ref->push_back(7);
}
```

#### Примитивные типы

```cpp
Mutexed<int> shared_int{0};

{
  auto owner_ref = shared_int.Acquire();
  *owner_ref = *owner_ref + 1;
}
```

## Задание

Реализуйте [`Mutexed<T>`](mutexed.hpp).

### Замечания по реализации

Изучите правила перегрузки оператора `->`: https://en.cppreference.com/w/cpp/language/operators

Не вызывайте методы `lock` и `unlock` у мьютекса вручную. Вместо этого используйте [`std::lock_guard`](https://en.cppreference.com/w/cpp/thread/lock_guard).

## `Mutexed` in the Wild

- [`Synchronized`](https://github.com/facebook/folly/blob/main/folly/docs/Synchronized.md) из библиотеки `facebook/folly`
- [`Mutex`](https://doc.rust-lang.org/std/sync/struct.Mutex.html) из стандартной библиотеки языка [Rust](https://www.rust-lang.org/)

## Альтернативное решение

См. [Thread-safety analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) в компиляторе Clang.
