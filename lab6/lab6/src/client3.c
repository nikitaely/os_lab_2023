#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
//#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

//#define debug // включение режима дебага
#define num_server_max 10 // максимальное кол-во серверов

typedef struct
{
  /* data */
  char ip[20];   // адрес
  int port;      // порт
  int socketpnt; // сокет сервера
  int conn;     // указатель на соединение
  int a;        // начало интервала
  int b;        // конец интервала
  long long f;  // результата
} server_t;



int main(int argc, char **argv)
{
  int mod; // делитель факториала
  char url_adr[100];
  char buf[500];
  int factInput=11; // число для вычисления
  long long fact=1; // результат вычисления факториала
  // переменные для работы с файлом
  server_t server[num_server_max]; 
  /*
  server[0].a=1;
  server[0].b=5;
  server[1].a=6;
  server[1].b=10;
  */
  FILE *ipfile;
  char split[]=":";
  char *port;
  int serverCount=0; // кол-во серверов
  //-----------------
  #ifdef  debug
  char st0[]="prog"; // название откомпилировнной программы 
  char st1[]="-u"; // факториал числа
  char st2[]="./iplist.txt";
  char st3[]="-f"; // делитель
  char st4[]="8";
  char st5[]="--mod"; // делитель
  char st6[]="17";
  char *argv2[]={st0,st1,st2,st3,st4,st5,st6};
  argc=7; // кол-во аргументов в argv2
  argv=argv2; // подмена аргументов командной строки своей
  #endif

  //---------------------
  //--- Разбор параметоров комндной строки
  //-------------------------
  while (true) {
    int current_optind = optind ? optind : 1;
    memset(buf,'\0',sizeof(buf) ); // обнуляем буфер
    static struct option options[] = {{"url", required_argument, 0, 'u'},
                                      {"mod", required_argument, 0, 'm'},
                                      {"fact", required_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "u:m:f:", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
          /*
            seed = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",seed);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --seed\n");
              return -1;
            }
            */
            break;
         
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      //---------обработка коротких параметров
      case 'u': // ввод адреса серверов
        strcpy(url_adr,optarg);
            // your code here
            // error handling
        break;
      case 'm': // ввод делителя
        mod = atoi(optarg);
            // your code here
            // error handling
            sprintf(buf,"%d",mod);
            if  (strcmp(buf,optarg)!=0)
            {
              printf("Error input numeric --mod\n");
              return -1;
            }

        break;
      case 'f': // ввод делителя
        factInput = atoi(optarg);
            // your code here
            // error handling
            sprintf(buf,"%d",factInput);
            if  (strcmp(buf,optarg)!=0)
            {
              printf("Error input numeric --fact\n");
              return -1;
            }

        break;
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  //---------------------
  //--- Конец Разбора параметоров комндной строки
  //-------------------------


  ipfile=fopen(url_adr,"r");
  if (ipfile!=NULL)
  {    
    printf("File open\n");
    char *s;
    while(1)
    {
      memset(buf,'\0',sizeof(buf)); // обнуляем буфер
      s=fgets(buf,30,ipfile); // считываем  строку длиной не более 30 символов
      if (s!=NULL)
      {
        if (buf[0]=='\n') // если пустая строка просто пробел 
          {
            continue;
          }
        int l=strlen(buf);
        if (buf[l-1]=='\n') // если в конце строки возврат каретки, то удаляем его
        {
          buf[l-1]='\0'; 
        }
        printf("%s\n",s); // печатаем найденную строку
        port=strstr(buf,split); // ищем символ раздела строки
        if (port!=NULL) // если нашли, то заменяем его на символ конец строки
          {
            *port='\0';
            port++;
            strcpy(server[serverCount].ip,buf);
            sscanf(port,"%d", &server[serverCount].port); 
            server[serverCount].f=1;
            serverCount++;
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
    }
    fclose(ipfile);
    printf("file closed\n");
  }
  else
  {
    printf("file not found\n End program \n");
    return -1;
  }
  //-----------------
  // Вычисляем границы для распределения массива по процессам
  // factInput=10;
  int sizemin=(factInput)/serverCount;
  int ost=(factInput)%serverCount;


  for (uint32_t i = 0; i < serverCount; i++) 
  {
   //struct MinMax min_max;
        if (i==0)
        {
          server[i].a=1;
          server[i].b=sizemin;
          if (ost)
          {
            server[i].b++;
            ost--;
          }
          //printf("Thread %d\n",i);
        }
        else
        {
          
          server[i].a=server[i-1].b+1;
          if (ost)
          {
            server[i].b=server[i].a+sizemin;
            ost--;
          }
          else
          {
            server[i].b=server[i].a+sizemin-1;
          }
        }
  }

  // создаем сокет клиент
  for (size_t i = 0; i < serverCount; i++)
  {
    /* code */
    server[i].socketpnt=socket(AF_INET,SOCK_STREAM,0);
    // настраиваем структуру
    struct sockaddr_in adress;
    int len; 
    adress.sin_family=AF_INET;
    adress.sin_addr.s_addr=inet_addr(server[i].ip);
    adress.sin_port=htons(server[i].port);
    len=sizeof(adress);
    server[i].conn=connect(server[i].socketpnt, (struct sockaddr *) &adress,len  );
    if (!server[i].conn)
    {
      sprintf(buf,"%d %d",server[i].a, server[i].b);
      write(server[i].socketpnt,buf,strlen(buf));
      printf("send %s:%d: %s\n",server[i].ip,server[i].port,buf);
    }
    else 
    {
      printf("Error connect with server %s:%d\n",server[i].ip,server[i].port);
    }
  }

  for (size_t i = 0; i < serverCount; i++)
  {
    if (!server[i].conn)
    {
      /* code */
      memset(buf,'\0',sizeof(buf));
      read(server[i].socketpnt,buf,sizeof(buf) );
      sscanf(buf,"%llu",&server[i].f);
      printf("Resv %s:%d: %llu\n",server[i].ip,server[i].port,server[i].f);
      close(server[i].socketpnt);
    }
  }

  for (size_t i = 0; i < serverCount; i++)
  {
    /* code */
    if (!server[i].conn)
    {
      fact =fact*server[i].f;
    }
    else
    {
      fact=0;
      printf("Error server \n End program\n");
      return -1;
    }
  }
  printf("Factorial %llu mod %d= %llu \n",fact,mod,fact%mod);
  //printf("Factorial %llu\n",fact%mod);
  printf("End program\n");

  
  
  return 0;
}