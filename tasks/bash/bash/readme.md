# Скрипты на Bash и Python

В этой задаче познакомимся с языком Bash, а также сравним его с Python.

## Bash

Bash - язык скриптов для операционной системы. На нём пишут набор команд терминала, которые выполняются построчно. Классические bash скрипты: [`установка зависимостей`](../../install_deps.sh), [`установка программ`](../../install_client.sh), работа с docker-контейнерами и etc. 

## Изучение

Bash имеет довольно неприятный синтаксис. Изучить его предлагается двумя способами:
1) Интерактивный [tutorial](https://www.learnshell.org/)
2) Цикл статьей на [habr](https://habr.com/ru/companies/ruvds/articles/325522/)


## Python

Python - язык общего назначения. Однако, используя библиотку [`os`](https://docs.python.org/3/library/os.html) можно выполнять те же действия, что и в Bash.

В проектах применяют оба подхода, но чаще встречается Bash скрипты на случай, если на машине не установлен интерпретатор Python.

## References

* [`Bash vs. Python: For Modern Shell Scripting`](https://levelup.gitconnected.com/bash-vs-python-for-modern-shell-scripting-c1d3d79c3622)
* [`Bash manual GNU`](https://www.gnu.org/software/bash/manual/bash.html)

## Задание

Решите задачу своего [варианта](variant.md), написав реализацию в файлах `solution.sh` и `solution.py`. 

Скрипт нужно написать как на Bash, так и на Python.

Интерфейс скриптов `должен быть одинаковым!`.   
Входные данные передаются в виде аргументов командной строки.

Результат действия скриптов должен быть одинаковым.

Добавьте флаг -h или --help, который выведёт краткую инфорацию о том, как работать с скриптом.

## Реализация

### Аргументы командной строки

В Python стоит использовать библиотку [`Argparse`](https://docs.python.org/3/library/argparse.html)

В Bash делается [так](https://www.squash.io/passing-parameters-to-scripts-in-bash/)

Опции, как правило, бывают полные (`--help`), а бывают аналогичные, но краткие (`-h`). Приветствуется поддержка этого в Вашем скрипте.

### Обработка ошибок

Если ваш скрипт предполагает обязательные входные данные, а пользователь их не ввёл - стоит напечатать USAGE, i.e. -h и закончить программу. 

Никаких лишних сообщений об ошибках писать не стоит.

### Формат справки

`--help` имеет негласный шаблон. Используйте как у `ls`:
```shell
Usage: ls [OPTION]... [FILE]... # USAGE
List information about the FILEs (the current directory by default). # What about
Sort entries alphabetically if none of -cftuvSUX nor --sort is specified. # Specific 

Mandatory arguments to long options are mandatory for short options too. # Flags
  -a, --all                  do not ignore entries starting with .
  -A, --almost-all           do not list implied . and ..
      --author               with -l, print the author of each file
  -b, --escape               print C-style escapes for nongraphic char
  ...
``` 