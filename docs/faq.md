# FAQ

#### Рабочее окружение сломалось, как быть?

В любой непонятной ситуации проще пересоздать рабочее окружение по инструкции, а не разбираться как починить разломанное.

---

#### Не работает докер

В любой непонятной ситуации с докером лучше пересоздать образ. Для этого удалите все текущие образы и сбросьте кэши:
```shell
docker rm -vf $(docker ps -a -q)
docker rmi -f $(docker images -a -q)
```

А после создайте заново образ по [инструкции](docker.md)

---


#### Не найден clang++ в докер контейнере:

1. Выйдите из контейнера - exit
2. Пропишите в терминале Windows/OS X/Linux: 
```shell
docker exec -u 0 -it containerName bash
```
Где вместо containerName подставьте имя только что созданного контейнера (имя репозитория).</br>
3. Пропишите
```shell
sudo ln -s /usr/bin/clang-17 /usr/bin/clang
sudo ln -s /usr/bin/clang++-17 /usr/bin/clang++
sudo ln -s /usr/bin/clang-tidy-17 /usr/bin/clang-tidy
sudo ln -s /usr/bin/clangd-17 /usr/bin/clangd
sudo ln -s /usr/bin/clang-format-17 /usr/bin/clang-format

```

#### Как отключить запуск тестов в подпроцессе (например, для запуска тестов с отладчиком)?

Запустите исполняемый файл с тестами с флагами командной строки `--disable_forks` и `--disable_time_limits`.

---

#### При компиляции ворнинги считаются ошибками, как это отключить?

В промышленной разработке `-Werror` – стандарт.  Но при отладке решения задачи этот флаг может мешать. 

Выключить его можно закомментировав соответствующий `add_compile_options` в [`cmake/CompileOptions.cmake`](/cmake/CompileOptions.cmake).

---

#### Не работает github.com?

Такое случается. Проверить его статус можно здесь: https://www.githubstatus.com/.

---

#### Проект курса не собирается!

Обратите внимание: сборка через Clippy и сборка через CLion работают независимо друг от друга!

##### Clippy

Попробуйте собраться начисто:

`clippy cmake --clean`, затем `clippy warmup`.

Вместо `--clean` можно стереть директорию `build` в корне репозитория курса и выполнить обычный `clippy cmake`.


#### Как установить опцию CMake при сборке?

Для Clippy используйте файл `.clippy-build-profiles.json` в корне репозитория.

---

#### Утилита `clang-tidy` / команда `clippy tidy` генерирует тысячи ворнингов, так и должно быть?

К сожалению, да.

---

#### Как отключить проверку `clang-tidy` для конкретной строки кода?

См. [Suppressing Undesired Diagnostics](https://clang.llvm.org/extra/clang-tidy/#suppressing-undesired-diagnostics).

---

#### У меня MacBook с процессором ARM M1/M2, код не собирается, что делать?

Не поддерживаем, извините.

