#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//#define debug // включение режима дебага

#define BUFSIZE 100
#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {

  #ifdef  debug
  char st0[]="tcpclient"; // название откомпилировнной программы 
  char st1[]="127.0.0.1"; // ip address
  char st2[]="10050"; // port
  char *argv2[]={st0,st1,st2};
  argc=3; // кол-во аргументов в argv2
  argv=argv2; // подмена аргументов командной строки своей  
  #endif

  int fd;
  int nread;
  char buf[BUFSIZE];
  struct sockaddr_in servaddr;
  if (argc < 2) {
    printf("Too few arguments \n");
    exit(1);
  }

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    perror("bad address");
    exit(1);
  }

  servaddr.sin_port = htons(atoi(argv[2]));

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }

  write(1, "Input message to send\n", 22);
  memset(buf,'\0',sizeof(buf));
  //scanf("%s",buf);
  while ((nread = read(0, buf, BUFSIZE)) > 0) 
  {
    if (write(fd, buf, strlen(buf)) < 0) {
      perror("write");
      exit(1);
    }
    // если длина сообщения более 10 символов соединение обрывается.
    if (strlen(buf)>10)
          break;
      memset(buf,'\0',sizeof(buf));
  }

  close(fd);
  printf("Client close connect\n");
  exit(0);
}
