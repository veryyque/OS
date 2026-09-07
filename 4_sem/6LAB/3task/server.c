#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#define SOCKET_PATH "/tmp/echo_socket"
#define BUFFER_SIZE 4096

static int server_running = 1;

void sigint_handler(int sig) {
    server_running = 0;
}

void *HandleClient(void *Arg) {
    int ClientFd = *(int*)Arg;
    free(Arg);
    char Buffer[BUFFER_SIZE];
    int BytesRead;
    
    printf("[+] Новый клиент подключен (дескриптор: %d)\n", ClientFd);
    
    while ((BytesRead = read(ClientFd, Buffer, sizeof(Buffer))) > 0) {
        //печать того, что пришло на сервер
        Buffer[BytesRead] = '\0';
        printf("Получено %d байт: \"%s\"\n", BytesRead, Buffer);
        //отправляю клиенту
        write(ClientFd, Buffer, BytesRead);
    }
    
    printf("[-] Клиент отключился\n");
    close(ClientFd);
    return NULL;
}

int main() {
    signal(SIGINT, sigint_handler);
    unlink(SOCKET_PATH);
    
    int ServerFd = socket(AF_UNIX, SOCK_STREAM, 0); //AF_UNIX - локальный сокет(тип адрр), потоковая передача, протокол 0 по умрлчанию(TCP)
    struct sockaddr_un addr; //структура адреса сокута
    addr.sun_family = AF_UNIX; //семейство адресов
    strcpy(addr.sun_path, SOCKET_PATH); 
    
    bind(ServerFd, (struct sockaddr*)&addr, sizeof(addr)); //привязка сокета к АДРЕСУ
    listen(ServerFd, 5);
    
    printf("Сервер запущен на %s\n", SOCKET_PATH);
    printf("Ожидание подключений...\n\n");
    
    while (1) {
        //при подключении клиента создается НОВЫЙ сокет
        int ClientFd = accept(ServerFd, NULL, NULL); //null - адрес клиента и размер адреса
        
        if (!server_running) {
            close(ClientFd);
            break;
        }

        int *fd_ptr = malloc(sizeof(int));
        *fd_ptr = ClientFd;
        
        pthread_t thread;
        pthread_create(&thread, NULL, HandleClient, fd_ptr); //создаю поток для обработки клиента
        pthread_detach(thread);
    }
    unlink(SOCKET_PATH);
    close(ServerFd);
    return 0;
}