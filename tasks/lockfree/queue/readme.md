# Michael-Scott Queue

Реализуйте MPMC лок-фри очередь [Michael-Scott](https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf).

## Управление памятью

Для управления памятью используйте [Hazard Pointers](https://www.cs.otago.ac.nz/cosc440/readings/hazard-pointers.pdf).

Пример использования: [`LockFreeStack<T>`](lock_free_stack.hpp).

### API

<details>
<summary>Вы можете менять его</summary>

#### `Manager`

– сборщик мусора.

Методы:
- `Get()` – получить доступ к глобальному экземпляру `Manager`
- `MakeMutator` – построить _мутатор_, с помощью которого поток в операции над лок-фри контейнером сможет
  - защищать объекты в памяти от удаления и 
  - планировать удаление объектов
- `Collect` – собрать мусор во всех retire list-ах всех зарегистрированных потоков

#### `Mutator`

_Мутатором_ (_mutator_) в алгоритмах [garbage collection](https://gchandbook.org/) называют поток программы, который меняет ссылки в графе объектов.

`Mutator` создается для каждого _вызова операции_ над лок-фри контейнером. 

Число мутаторов в программе, таким образом, не превосходит число потоков; для каждого потока может существовать не более одного мутатора.

Методы:
- `Protect(size_t index, AtomicPtr<T>& ptr)` – прочитать указатель на объект из атомика `ptr` и защитить этот объект от удаления для последующих обращений к нему, используя локальный слот `index`.
- `Announce(size_t index, T* ptr)` – анонсировать обращение к объекту для других потоков через локальный слот `index`
- `Retire(T* ptr)` – добавить объект в очередь на удаление (retire list)
- `Clear()` – сбросить все защищенные мутатором указатели

</details>

### Требования к реализации

- Ограниченный расход памяти
- Гарантия прогресса – lock-free
- На быстром пути – wait-free и ничего сложнее операций `load` / `store`

## Задание

1) Реализуйте [лок-фри очередь Michael-Scott](lock_free_queue.hpp) без управления памятью (тесты [queue/lf.json](pipelines/queue/lf.json))
2) Реализуйте [hazard pointers](hazard.hpp) (тесты [stack/lf_mm.json](pipelines/stack/lf_mm.json))
3) Добавьте hazard pointers в очередь (тесты [queue/lf_mm.json](pipelines/queue/lf_mm.json))

## References

- [Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects](https://www.cs.otago.ac.nz/cosc440/readings/hazard-pointers.pdf)
- [Схемы управления памятью для лок-фри структур данных](https://habr.com/ru/articles/202190/)

### Промышленные реализации

- [Folly](https://github.com/facebook/folly/blob/main/folly/synchronization/Hazptr.h)
- [YT](https://github.com/ytsaurus/ytsaurus/blob/main/yt/yt/core/misc/hazard_ptr.h)
- [YDB](https://github.com/ydb-platform/ydb/blob/main/ydb/core/util/hazard.h)
