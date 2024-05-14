/*
#include <stdio.h>
#include <stdlib.h>

#include <winsock2.h>

int main(){



  return 0;
}
*/

#include <stdio.h>
#include <winsock2.h>

int main() {
    // Создаем сокет
    SOCKET socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
      printf("d\n");
        perror("Не удалосьasda создать сокет");
        return 1;
    }

    // Получаем IP-адрес сервера youtube.com
    struct hostent* server = gethostbyname("youtube.com");
    if (server == NULL) {
        perror("Не удалось получить   dasdfIP-адрес сервера");
        return 1;
    }

    // Устанавливаем адрес сервера и порт
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr = *((struct in_addr*)server->h_addr);

    // Устанавливаем соединение с сервером
    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Не удалось установитьsdf as соединение");
        return 1;
    }

    // Отправляем HTTP-запрос на сервер
    char* request = "GET / HTTP/1.1\r\nHost: youtube.com\r\n\r\n";
    if (send(socket_desc, request, strlen(request), 0) < 0) {
        perror("Не удалось отправитsdfasdь запрос");
        return 1;
    }

    // Получаем и выводим ответ от сервера
    char response[4096];
    if (recv(socket_desc, response, sizeof(response), 0) < 0) {
        perror("Не удалось получитьsdf ответ");
        return 1;
    }
    printf("%s\n", response);

    // Закрываем сокет
    //close(socket_desc);

    return 0;
}
