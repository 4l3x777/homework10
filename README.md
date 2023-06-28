# Bulk server
## Задание 10

Цель – переработать **Задание 9** так, чтобы ввод данных осуществлялся по сети. Необходимо разработать асинхронный сервер, работающий с протоколом, аналогичным консоли (tcp). 

Сервер принимает команды построчно.

Дальнейшая обработка остаётся без изменений – вывод в консоль и файлы, статический блоки и блоки динамического размера.

В качестве библиотеки сетевого взаимодействия следует использовать библиотеку **Boost.ASIO** (используем асинхронные возможности). 

### 1. Комманды

Запуск bulk сервера

**# bulk_server \<port\>**

Инициирование сессии клиента

**# echo connect \<bulk_size\> | nc localhost \<port\>**

Отправка комманд от клиента к серверу

**# echo receive \<context\> [commands] | nc localhost \<port\>**

Отключение сессии клиента

**# echo disconnect \<context\> | nc localhost \<port\>**

, где
* **port** – номер tcp порта для входящих соединений. Соединения должны обслуживаться с 
произвольных интерфейсов
* **bulk_size** - размер блока комманд клиента
* **context** - токен, возвращаемый от сервера - клиенту после запроса ***connect***

### 2. Пример запуска команд

### 2.1 Примеры запуска статических блоков

Запуск bulk сервера

**# bulk_server 9000**

Инициирование сессии клиента

**# echo connect 3 | nc localhost 9000**

**# You context is: 105731079**

Отправка комманд от клиента к серверу

**# echo receive 105731079 0 1 2 3 4 5 6 7 8 9 | nc localhost 9000**

Отключение сессии клиента

**# echo disconnect 105731079 | nc localhost 9000**

PS: для Windows необходимо поставить **cygwin** с пакетом ***netcat***

Запуск команды 

**# echo receive 105731079 0 1 2 3 4 5 6 7 8 9 | nc localhost 9000** 

должен привести к появлению в консоли сервера:

bulk_105731079: 0, 1, 2

bulk_105731079: 3, 4, 5

bulk_105731079: 6, 7, 8

и формированию трех файлов с содержимым, аналогичным каждой строке. 

Команда 9 остается в незаконченном (ожидающем ещё 2 комманды) статическом блоке комманд размера 3.

Одновременный запуск двух команд (например, из двух разных консолей) из предыдущей сессии и с инициированной новой сессией клиента:

**# echo connect 4 | nc localhost 9000**

**# You context is: 106302030**

Запуск команды из сессии 105731079

**# echo receive 105731079 0 1 2 3 4 5 6 7 8 9 | nc localhost 9000** 

и

Запуск команды из сессии 106302030

**# echo receive 106302030 10 11 12 13 14 15 16 17 18 19 | nc localhost 9000** 

Приведет к появлению в консоли:

bulk_105731079: 9, 0, 1

bulk_106302030: 10, 11, 12, 13

bulk_105731079: 2, 3, 4

bulk_106302030: 14, 15, 16, 17

bulk_105731079: 5, 6, 7

и формированию пяти файлов с содержимым, аналогичным каждой строке. 

Комманды 8,9 остаются в незаконченном статическом блоке сессии 105731079 (размер блока 3). 

Комманды 18,19 остаются в незаконченном статическом блоке сессии 106302030 (размер блока 4).

### 2.2 Примеры запуска динамических блоков

**# echo receive 106302030 { 10 11 12 13 14 15 16 17 18 19 } | nc localhost 9000**

bulk_106302030: 10, 11, 12, 13, 14, 15, 16 17, 18, 19

**# echo receive 106302030 { 10 11 | nc localhost 9000**

**# echo receive 106302030 12 13 14 | nc localhost 9000**

**# echo receive 106302030 } | nc localhost 9000**

bulk_106302030: 10, 11, 12, 13, 14

### 3. Требования к реализации 

Результатом работы должен стать исполняемый файл **bulk_server**, который находится в пакете **bulk_server**. 

Специальных усилий по демонизации сервера выполнять не стоит – сразу после  запуска сервер может не возвращать управление вплоть до принудительного завершения.

### 4. Проверка

Задание считается выполненным успешно, если после установки пакета и запуска с тестовыми данными вывод соответствует описанию **Задания 9**.
