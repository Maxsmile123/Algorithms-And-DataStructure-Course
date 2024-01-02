# Автоматическое тестирование (CI)

## Просьба

Пожалуйста 🙏, не отправляйте в CI решения, которые не проходят тесты (`clippy test`) локально: в CI запускаются те же самые тесты с помощью той же самой команды.

## Шаги, которые нужно выполнить 1 раз

### Регистрация

Заведите аккаунт на https://github.com/, если у вас его нет.

Не используйте в логине дефис.

### Fork репозитория с задачи

Выполните Fork данного репозиториря. Сделайте его `приватным!`

В этот репозиторий вы будете коммитить и пушить решения.

### Настраиваем репозиторий

#### Шаг 1. Выдать мне доступ

Перейдите во вкладку `Settings` своего репозитория. Далее переходим в `Collaborators`. Нажимаем `Add people` и пишем там мой ник: ```Maxsmile123```.

Этим действием вы выдадите мне доступ на просмотр вашего репозитория.

#### Шаг 2. Настроить обязательность Code Review

Переходим в `Branches` -> `Add branch protection rule`.

В графе `Branch name pattern` пишем `master`.

Ставим галочки в следующих полях:
- [x] Require a pull request before merging
- [x] Require review from Code Owners
- [x] Require status checks to pass before merging
- [x] Require branches to be up to date before merging

<details>
<summary>Подробнее про правила</summary>

`Первое правило` обязывает мержить коммиты в main только через Pull Request.

`Второе правило` ставит в Reviewers участников из файла CODEOWNERS, т.е. меня. Это позволит мне получать уведомление о том, что ваш PR нужно посмотреть.

`Третье правило` требует обязательного прохождения проверок в CI при мерже.

`Четвертое правило` требует, чтобы ваш код был синхронизирован с моим репозиторием.
</details>

### Настраиваем локально

#### Шаг 1. Установите `git`

#### Шаг 2. Настройте конфиг

Чтобы связать свой аккаунт GitHub с Git укажите своё имя
```shell
git config --global user.name "Your Name"
```
и почту:
```shell
git config --global user.email "your.email@address.com"
```

#### Шаг 3. Выдайте себе PAT

На этом шаге вам нужно получить токен для работы с API https://github.com/

Settings --> Developer Settings--> Persaonl access tokens --> Generate new token --> Note --> set permissions (repo,repo_hook maybe) --> generate token

Для этого на https://github.com/ зайдите в `Settings` своего профиля, затем в `Developer Settings`, после в `Personal access tokens`, там нажмите `Generate new token (Classic)` после чего выберите произвольное имя для вашего токена, поставьте галки напротив (repo, admin:org), установите expiration date и нажмите `Generate token`.

См. [Personal Access Tokens](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens)

Токен – это полученная случайная строчка из цифр и букв. Сохраните его, иначе он пропадет! Имя токена нам не понадобится.

#### Шаг 4. Запишем PAT в конфиг git

При каждом пуше вас будут просить ввести логин и PAT. Чтобы этого избежать, нужно ввести в консоли в папке репозитория следующую команду:
```shell
git config --global credential.helper store
```
Логин и пароль у вас запросят только при первом пуше.


#### Шаг 5. Установим связь с репозиторием с задачами

```shell
git remote add upstream https://github.com/Maxsmile123/Algorithms-And-DataStructure-Course.git
```

## Работа с задачей / репозиторием решений

#### Шаг 1

Переходим в директорию с задачей:
```shell
cd tasks/tutorial/aplusb
```

#### Шаг 2

Решаем задачу.

#### Шаг 3

Коммитим файлы текущей задачи в отдельную ветку локального репозитория решений:
```shell
clippy commit
```

Можно указать комментарий:
```shell
clippy commit -m "yep"
```

#### Шаг 4

Пушим коммиты из локальной ветки задачи в ветку вашего приватного remote-репозитория:
```shell
clippy push
```

#### Шаг 5

Создаем [_pull request_](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request) ветки с решением в _master_:
на созданном PR будет запускаться CI.

#### Дальнейшие шаги

Пусть дальше вы нашли и исправили баги в своем коде:

```shell
# ...
clippy commit -m 'fix'
# ...
clippy commit -m 'another fix'
clippy push
```

После `push` новые коммиты попадут в уже существующий PR и перезапустят CI.


## Под капотом

Для каждой задачи в этом репозитории будет создаваться отдельная ветка (например, `tutorial/aplusb`).

Команда `clippy commit` коммитит файлы текущей задачи (см. `submit_files` в `task.json`) _локально_ в ваш репозиторий решений.

Команда `clippy push` пушит коммиты из ветки текущей задачи локального репозитория решений в remote-репозиторий.


[Документация по командам](https://gitlab.com/Lipovsky/clippy/-/blob/master/docs/commands.md)


Запуск джоба CI на тестирующем сервере состоит из следующих шагов:

1) Скачивается коммит с файлами решения из вашего репозитория решений.
2) Эти файлы накатываются на директорию задачи в репозитории курса с помощью команды `clippy apply`.
3) На задаче в репозитории курса запускаются команды `clippy cmake`, `clippy validate` и `clippy test`, т.е. ровно те же команды, что вы запускаете локально.
