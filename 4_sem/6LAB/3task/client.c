#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/echo_socket"
#define BUFFER_SIZE 4096

int main() {
    int SockFd;
    struct sockaddr_un ServerAddr;
    char Buffer[BUFFER_SIZE];
    
    SockFd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    ServerAddr.sun_family = AF_UNIX;
    strcpy(ServerAddr.sun_path, SOCKET_PATH);
    
    connect(SockFd, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)); //соединение с сокетом сервера
    
    printf("Подключено к серверу. Введите сообщения (quit - выход):\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(Buffer, sizeof(Buffer), stdin) == NULL) break;
        
        Buffer[strcspn(Buffer, "\n")] = '\0';
        
        if (strcmp(Buffer, "quit") == 0) break;
        
        if (strlen(Buffer) == 0) continue;
        
        write(SockFd, Buffer, strlen(Buffer)); //отправляю сообщение серверу
        
        int n = read(SockFd, Buffer, sizeof(Buffer) - 1); //чтение ответа от сервера
        if (n <= 0) break;
        
        Buffer[n] = '\0';
        printf("Эхо: %s\n\n", Buffer);
    }
    
    close(SockFd);
    return 0;
}