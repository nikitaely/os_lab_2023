#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
char buf[10000];
//int port;
int countIP=1;
int countchar;
char *ip[500];

//char port[7];
int len;
FILE *ipfile;
char split[]=":";
char *port;
char st[30];
int main()
{
  ipfile=fopen("./iplist.txt","r");
  if (ipfile!=NULL)
  {    
    printf("File open\n");
    char *s;
    do{
      s=fgets(st,30,ipfile);
      if (s!=NULL)
      {
        int l=strlen(st);
        if (st[l-1]=='\n')
        {
          st[l-1]='\0'; 
        }
        printf("%s\n",s);
        port=strstr(st,split);
        if (port!=NULL)
          {
            *port='\0';
            port++;
          }
          else
          {
            printf("No port\n");
          }
      }
      else
      {
        break;
      }
    }while( 1);
    fclose(ipfile);
  }
  /*
  int file=open("./iplist.txt",O_RDONLY);
  countchar=read(file,buf,sizeof(buf));
  close(file);
  ip[0]=buf;
  for (int i = 1; i < countchar; i++)
  {
    if (buf[i]==':'||buf[i]=='\n')
    {
      ip[countIP]=&buf[i+1];
      countIP++;
      buf[i]='\0';
    }
  }

  if (countIP%2==1)
  {
    countIP--;
  } 

  */
  
  return 0;
}