#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  int sockfd;
  int len;
  struct sockaddr_un adress;
  int result;
  char ch='A';
  sockfd= socket(AF_UNIX,SOCK_STREAM,0);
  adress.sun_family=AF_UNIX;
  strcpy( adress.sun_path,"server_socket");
  len=sizeof(adress);
  result=connect(sockfd, (struct sockaddr *) &adress,len  );
  
    write(sockfd,&ch,1);
    printf("%c\n",ch);
    read(sockfd,&ch,1);
    printf("%c\n",ch);
  
  close(sockfd);
  return 0;
}