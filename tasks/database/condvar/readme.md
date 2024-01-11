# Условная переменная (кондвар)

## Пререквизиты

- [mutex/mutex](/tasks/mutex/mutex)

---

В этой задаче мы напишем [`std::condition_variable`](https://en.cppreference.com/w/cpp/thread/condition_variable).

## Кондвары

Условные переменные (_condition variables_) или просто _кондвары_ (_condvars_) – примитив синхронизации, который предоставляет потокам функциональность 1) блокирующего ожидания событий и 2) уведомления об их реализации. 

Под событием в контексте кондваров понимается модификация разделяемого состояния, защищенного мьютексом (например, очереди задач в пуле потоков).

## Операции

У кондвара есть один метод для ожидания (`Wait`) и два метода для нотификации (`NotifyOne` и `NotifyAll`).

Метод `Wait` можно вызывать только внутри критической секции, в него передается захваченный мьютекс.

Семантика `cv.Wait(mutex)` –

1. *Aтомарно* a) отпустить `mutex` и b) встать в очередь ожидания нотификации от `cv.NotifyOne()` или `cv.NotifyAll()`.
2. После пробуждения захватить обратно отпущенный `mutex` и завершить вызов.

До начала и после завершения вызова `cv.Wait(mutex)` поток владеет мьютексом.

Семантика `cv.NotifyOne()` / `cv.NotifyAll()` – разбудить один из потоков / все потоки, стоящие в очереди ожидания в вызове `Wait`.

Кондвар не запоминает нотификации: если в момент вызова `NotifyOne` или `NotifyAll` ни один поток не ждал внутри вызова `Wait`, то нотификация не будет иметь эффекта.


Под атомарностью метода `Wait` следует понимать атомарность _относительно_ `NotifyOne` / `NotifyAll`: 

Не допускается сценарий, когда поток уже отпустил мьютекс, но еще не встал в очередь ожидания кондвара, а в этот момент другой поток вызвал `cv.NotifyOne()` и никого не разбудил.


## Spurious Wakeups

Метод `Wait` допускает *ложные пробуждения* (*spurious wakeups*):
- Вызов `cv.Wait(mutex)` может вернуть управление даже без нотификации от других потоков
- После вызова `NotifyOne` из вызова `Wait` могут выйти несколько потоков

Найдите подобные сценарии в своей реализации.

Несмотря на то, что такое поведение кажется некорректным, исправлять его и думать о нем нет большой необходимости: типичные корректные паттерны использования кондваров автоматически учитывают и ложные пробуждения. 

## References

* [`std::condition_variable`](https://en.cppreference.com/w/cpp/thread/condition_variable)
* [`pthread_cond_wait`](https://github.com/lattera/glibc/blob/895ef79e04a953cac1493863bcae29ad85657ee1/nptl/pthread_cond_wait.c#L193)
* [`tf::CondVar`](https://gitlab.com/Lipovsky/tinyfibers/-/blob/master/tf/sync/condvar.hpp)

## Кондвар и фьютекс

Можно сказать, что кондвар обобщает фьютекс: ждать можно на произвольном состоянии / условии, а не только на одной ячейке памяти / зафиксированном условии ` == old`.

## Задание

Перенесите реализацию `Mutex` из задачи [mutex/mutex](/tasks/mutex/mutex) в [`mutex.hpp`](mutex.hpp), мьютекс и кондвар будут тестироваться вместе.

Реализуйте `CondVar` из [`condvar.hpp`](condvar.hpp).
 
## Реализация

### Futex

Как и мьютекс, кондвар должен блокировать и будить ждущие потоки. А значит для реализации нам потребуется уже знакомый инструмент – системный вызов [`futex`](https://man7.org/linux/man-pages/man2/futex.2.html), с которым мы работаем через [`Wait` из _Twist_](https://gitlab.com/Lipovsky/twist/-/blob/master/examples/wait/main.cpp).

### A-B-A

Скорее всего ваша реализация будет подвержена [_A-B-A problem_](https://en.wikipedia.org/wiki/ABA_problem) из-за переполнения 32-битного счетчика.

Попробуйте разобраться как эту проблему пытаются решить в [`pthread_cond_wait`](https://github.com/lattera/glibc/blob/895ef79e04a953cac1493863bcae29ad85657ee1/nptl/pthread_cond_wait.c#L193)