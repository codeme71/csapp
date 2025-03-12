напиши на C для Linux клиент серверное приложение распределенньх вычислений 

напиши на C для Linux клиент серверное приложение распределенньх вычислений числа (2.23664562253)*10^45154499 которое вычисляет все знаки упомянутого числа и сохраняет результат на стороне сервера 

вычисления производятся на стороне клиентов функция сервера раздача заданий клиентам и сбор результатов клиентских вычислений в числовую последовательность в данном случае (2.23664562253)*10^45154499 вычисление 

Для создания клиент-серверного приложения, где сервер раздает задания клиентам, а сами вычисления выполняются на стороне клиентов, мы можем изменить структуру приложения. Сервер будет отправлять задачи по вычислению (например, сложение знаков числа \( (2.23664562253) \times 10^{45154499} \)) клиентам, и клиенты будут возвращать результаты. 

*--* Общее описание программы

1. *Сервер*:
   - Принимает подключения от клиентов.
   - Рассылает клиентам задачи на вычисления, включая отправку числа и индекса знака, который клиент должен вычислить.
   - Сбор результатов от клиентов и хранение их.

2. *Клиент*:
   - Подключается к серверу.
   - Получает задания от сервера.
   - Выполняет вычисления.
   - Отправляет результаты обратно на сервер.

*--* Сервер (server.c)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mpfr.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Привязка сокета к порту
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Прослушивание
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен, ожидает подключения клиентов...\n");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Каждому клиенту будет отправлено задание
        mpfr_t num;
        mpfr_init2(num, 1000); // Устанавливаем точность
        mpfr_set_d(num, 2.23664562253, MPFR_RNDN);
        
        int exponent = 45154499;
        mpfr_mul_2si(num, num, exponent, MPFR_RNDN);
        
        // Предоставление задания клиенту
        char result[BUFFER_SIZE];

        for (int i = 0; i < 100; ++i) { // Предположим, что мы хотим 100 знаков
            // Отправляем клиенту индекс знака
            send(new_socket, &i, sizeof(i), 0);
        }

        // Получаем результаты от клиента
        for (int i = 0; i < 100; ++i) {
            int digit;
            recv(new_socket, &digit, sizeof(digit), 0);
            printf("Получен знак %d: %d\n", i, digit);
            result[i] = digit + '0'; // Преобразуем в символ
        }

        // Записываем результат в файл
        FILE *file = fopen("result.txt", "w");
        if (file) {
            result[100] = '\0'; // Завершаем строку
            fprintf(file, "%s\n", result);
            fclose(file);
            printf("Результат сохранен в файл: %s\n", result);
        } else {
            perror("Ошибка открытия файла");
        }

        // Освобождение ресурсов
        mpfr_clear(num);
        close(new_socket);
    }

    return 0;
}
```

*--* Клиент (client.c)

Клиент будет выполнять вычисления и отправлять результаты серверу.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mpfr.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Ошибка при создании сокета \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Преобразование IP-адреса из текстового формата в бинарный
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {printf("Неверный адрес/Адрес не поддерживается \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Ошибка подключения \n");
        return -1;
    }

    // Получение задания от сервера
    int index;
    while (recv(sock, &index, sizeof(index), 0) > 0) {
        // Выполнение вычислений
        mpfr_t num;
        mpfr_init2(num, 1000);
        mpfr_set_d(num, 2.23664562253, MPFR_RNDN);
        int exponent = 45154499;
        mpfr_mul_2si(num, num, exponent, MPFR_RNDN);

        // Получаем нужный знак
        char digit_char;
        mpfr_get_str(&digit_char, NULL, 10, index + 1, num, MPFR_RNDN); // Получаем (index + 1)-й знак
        int digit = digit_char - '0'; // Преобразуем в целое число
        
        // Отправка результата обратно серверу
        send(sock, &digit, sizeof(digit), 0);

        mpfr_clear(num);
    }

    close(sock);
    return 0;
}
```

*--* Объяснение кода

1. *Сервер*:
   - Инициализирует сокет и ждет подключения клиентов.
   - Устанавливает число \( (2.23664562253) \times 10^{45154499} \) и отправляет клиентам запросы для вычисления знаков.
   - Получает от клиентов результаты и сохраняет их в файл `result.txt`.

2. *Клиент*:
   - Подключается к серверу и получает задания на вычисление.
   - Для каждого знака выполняет вычисления и отправляет результаты назад серверу.

*--* Компиляция и запуск

1. Скомпилируйте сервер и клиент с использованием библиотек MPFR и GMP:

```bash
gcc server.c -o server -lmpfr -lgmp
gcc client.c -o client -lmpfr -lgmp
```

2. Запустите сервер в одном терминале:

```bash
./server
```

3. Запустите несколько клиентов (по количеству знаков, которые вы хотите получить) в других терминалах:

```bash
./client
```

*--* Примечания

- Этот пример использует 100 клиентов для вычисления первых 100 знаков. Каждый клиент получает задание из сервера и работает параллельно.
- Обработка ошибок и дополнительные функции могут быть добавлены для улучшения программы.
- Сервер и клиенты необходимо запускать на одной машине или в сети, где они могут видеть друг друга. 


Установка необходимых библиотек

В Ubuntu или Debian вы можете установить их с помощью:

sudo apt-get install libgmp-dev libmpfr-dev


 Компиляция и запуск

1. Скомпилируйте сервер и клиент с использованием библиотек MPFR и GMP: 


gcc server.c -o server 
gcc client.c -o client -lmpfr -lgmp 











