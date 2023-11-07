#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 100

int main(int argc, char *argv[]) 
{
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char st[100]="test text\n";
    // создаем объект
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    // заполняем структуру для записи в объект
    address.sin_family = AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(9734);
    len=sizeof(address);
    // Подключение сокета к серверу
    result=connect(sockfd, (struct sockaddr *) &address,sizeof(address));
    if (result == -1)
    {
        perror("error connect");
        return -1;
    }
    // Обмен сообщениями
    write(sockfd,st, strlen(st));
    read(sockfd,st,sizeof(st));
    close(sockfd);
    return 0;
}
