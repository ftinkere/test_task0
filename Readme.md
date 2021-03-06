# Описание алгоритма

Сервер принимает порт для прослушки, а так же адрес сервера назначения, а так же возможно название файла для логирования, или сообщения будут идти в stdout.
Создаётся объект сервера с переданными аргументами или выбрасывается исключение (что хоть и не очень хорошо, но на данном проекте применяется для простоты). В это врем там открывается сокет, который будет слушать заданный порт. Данные о сервере назначения просто сохраняются в памяти объекта.
После этого вызывается главный цикл, в котором последовательно проходится четыре фазы.
1. Принятие входящих подключений
2. Обработка входящих сообщений
3. Обработка ответов с сервера назначения
4. Очистка ненужных больше соединений

# Тестирование
В качестве примера для тестирования я применял irc сервер, архив с исходниками которого имеется в репозитории, а в качестве клиента просто утилита `nc`.
Распаковать irc сервер, собрать, скопировать файл конфигурации из examples и запустить на стандартной конфигурации. (` ./run/bin/inspircd --nofork --debug` чтобы можно было увидеть о приходящих сообщениях)
Запустить прокси сервер `proxy 6777 127.0.0.1 6667`
Подключиться первым клиентом `nc 127.0.0.1 6777`
Ввести команды `nick a` и `user u u u u`
Подключиться вторым клиентом
Ввести команды на нём `nick b` и `user u u u u`

После введённых команд с сервера должны прийти сообщения об успешном подключении или ошибки, если те были допущены при вводе команд.
Теперь можно вводя команду `privmsg <кому> :<сообщение>` чатиться двумя этими клиентами, не замечая разницы в подключении напрямую или через прокси сервер, что так же можно проверить подключившись третьим клиентом напрямую.

#### Комментарий про #include "Server.hpp"
Изначально я хотел сделать два разных по логике сервера с общей кодовой базой, вынесенной в класс Server. Однако это оказалось не так удобно и требовало больше времени, отчего быстро отказался. Но иметь два класса с разделённым по ним кодом без какой-либо необходимости тоже неудобно. Поэтому класс Server был объединён с ProxyServer и убран из кодовой базы