#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    char buf[200];
    int server_sockfd, // указатель на поток сервера
        client_sockfd; // указатель на поток клиента
    struct sockaddr_in server_adr,
                       client_adr;
    int lenSockServer=sizeof(server_adr);
    int lenSockClient;
    // создаем неименованный сокет для сервера
    server_sockfd=socket(AF_INET, SOCK_STREAM,0);
    // Настраиваем структуру: протокол, адрес и порт
    server_adr.sin_family=AF_INET;
    server_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_adr.sin_port=htons(9734);
    // настраиваем адрес сервера
    bind(server_sockfd,(struct sockaddr*) &server_adr,lenSockServer);
    // настраиваем очередь запросов
    listen(server_sockfd,5);
    while (1)
    {
        printf("Server wait, ip=%llu  port=%d\n",server_adr.sin_addr.s_addr,ntohs(server_adr.sin_port));
        memset(buf,'\0',sizeof(buf));
        client_sockfd=accept(server_sockfd,
                            (struct sockaddr*) &client_adr, 
                            &lenSockClient);
        printf("Client addres, ip=%o  port=%d\n",ntohl(client_adr.sin_addr.s_addr),ntohs(client_adr.sin_port));                    
        read(client_sockfd,buf,sizeof(buf));
        write(client_sockfd,buf,strlen(buf));
        close(client_sockfd);

    }
    
    return 0;


}