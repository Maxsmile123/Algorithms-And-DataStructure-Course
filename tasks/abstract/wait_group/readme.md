# WaitGroup

[Go by Example: WaitGroups](https://gobyexample.com/waitgroups)

`WaitGroup` – счетчик абстрактной "работы", например, запущенных задач или файберов.

Изначально в счетчике 0.

Операции:
- `void Add(size_t count)` – добавить положительный `count`
- `void Done()` – вычесть единицу
- `void Wait()` – заблокировать поток до тех пор, пока значение счетчика не опустится до нуля

## Правила использования

Методы можно вызывать из разных потоков.

Вызовы `Add` должны быть упорядочены (синхронизацией / причинностью) с последним вызовом `Done` **пользователем** `WaitGroup` .

Можно считать, что `WaitGroup` – "одноразовый", т.е. счетчик опускается до нуля только один раз.

## Задание

Реализуйте [`WaitGroup`](wait_group.hpp)