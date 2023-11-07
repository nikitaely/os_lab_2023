#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  int server_sockfd,client_sockfd;
  int server_len, client_len;
  struct sockaddr_un server_adress, client_adress;
  unlink ("server_socket");
  server_sockfd=socket(AF_UNIX,SOCK_STREAM,0);
  server_adress.sun_family=AF_UNIX;
  strcpy(server_adress.sun_path,"server_socket");
  server_len=sizeof(server_adress);
  bind(server_sockfd,(struct sockaddr*)&server_adress,server_len );
  listen(server_sockfd,5);
  int k=0;
  while (1)
  {
    char ch;
    printf("server waiting\n");
    client_len=sizeof(client_adress);
    client_sockfd=accept(server_sockfd, 
                  (struct sockaddr*)&server_adress,
                  &client_len);
    read(client_sockfd,&ch,1);
    k++;
    if (k>50)
      k=1;
    ch+=k;
    sleep(3);
    write(client_sockfd,&ch,1);
    close(client_sockfd);
  }
  


  return 0;
}