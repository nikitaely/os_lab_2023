#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
//#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdbool.h>

//#define debug // включение режима дебага

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
struct par_fact_t
{
  int kol; // размер массива
  int i; // номер обработки
  long long *f; // массив факториалов
} factor;

void calcFact(void *factor2) {
  struct par_fact_t *ft=factor2;
  bool t=true;
  int i;
  while (t)
  {
    pthread_mutex_lock(&mut);
    i=ft->i;
    ft->i+=2;  
    pthread_mutex_unlock(&mut);
    if (i+1 >= ft->kol)
      return;
    ft->f[i]=ft->f[i]*ft->f[i+1];
    ft->f[i+1]=0;
  }
  
}


int main(int argc, char **argv)
{
  // переменные для работы с потоками
  pthread_t *threads; // массив потоков
  long long *mf=NULL; // указатель на массив факториалов
  int pnum=3; // кол-во потоков
  int kol; // размер для расчета произведения
  int portn; // Порт для подключения
  char buf[100]; // Буфер для приема данных
  int lenbuf;
  int a=6, // Начало факториала
      b=10; // Конец факториала 
  long long f=1; // факториал числа
  int server_sockfd, // указатель на сокет сервера
      client_sockfd; // укзатель на сокет клиента
  int server_len, client_len;
  struct sockaddr_in server_adress, // объект сокет сервера 
                    client_adress; // объект сокет клиента

  #ifdef  debug
  char st0[]="prog"; // название откомпилировнной программы 
  char st1[]="-p"; // номер порта
  char st2[]="9735";
  char st3[]="-n"; // кол-во потоков
  char st4[]="3";
  char *argv2[]={st0,st1,st2,st3,st4};
  argc=5; // кол-во аргументов в argv2
  argv=argv2; // подмена аргументов командной строки своей
  #endif
    
   //---------------------
  //--- Разбор параметоров комндной строки
  //-------------------------
  while (true) {
    int current_optind = optind ? optind : 1;
    memset(buf,'\0',sizeof(buf) ); // обнуляем буфер
    static struct option options[] = {{"port", required_argument, 0, 'p'},
                                      {"pnum", required_argument, 0, 'n'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "p:n:", options, &option_index);

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
      case 'p': // ввод адреса серверов
        portn = atoi(optarg);
            // your code here
            // error handling
            sprintf(buf,"%d",portn);
            if  (strcmp(buf,optarg)!=0)
            {
              printf("Error input numeric --mod\n");
              return -1;
            }
        break;
      case 'n': // ввод делителя
        pnum = atoi(optarg);
            // your code here
            // error handling
            sprintf(buf,"%d",pnum);
            if  (strcmp(buf,optarg)!=0)
            {
              printf("Error input numeric --mod\n");
              return -1;
            }

        break;
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }
    
  // Создание Сокета для сервера
  server_sockfd=socket(AF_INET,SOCK_STREAM,0);
  // Заполняем структуры адреса
  server_adress.sin_family=AF_INET;
  server_adress.sin_addr.s_addr=htonl(INADDR_ANY);
  server_adress.sin_port=htons(portn);
  server_len=sizeof(server_adress);
  // просл
  bind(server_sockfd,(struct sockaddr*)&server_adress,server_len );
  listen(server_sockfd,5);
  
  //strcpy(buf,"sfsefgesfsfsef");
  while (1)
  {
    printf("server waiting\n");
    memset(buf,'\0',sizeof(buf));
    client_len=sizeof(client_adress);
    client_sockfd=accept(server_sockfd, 
                  (struct sockaddr*)&server_adress,
                  &client_len);
    lenbuf=sizeof(buf) ;              
    lenbuf=read(client_sockfd,buf,sizeof(buf) );
    // расчет данных 
    sscanf(buf,"%d %d",&a,&b);
    kol=b-a+1; // рассчитываем размер массива
    
    //-----------------------
    //--выделяем память
    //-------------------
    // Выделяем память под процессы
    threads=(pthread_t *) malloc(sizeof(pthread_t) * pnum);
    // выделяем память под массив факториалов
    mf=(long long *) malloc(sizeof(long long)*kol );
    //-----------------------
    //--конец выделения памятя
    //-------------------

    //-------------------------
    // Создаем массив для вычисления
    //---------------------
    printf("Массив для вычисления факториала \n");
    for (int i = 0; i < kol; i++)
    {
      mf[i]=a;
      a++;
      printf("%llu ",mf[i]);
    }
    printf("\n");
   


    //---------------------------
    //---- заполнение структуры 
    //---------------------
    factor.f=mf; // копируем адрес массива
    factor.i=0; // индекс для обработки
    factor.kol=kol; // размер массива
    //---------------------------
    //----конец заполнения структуры 
    //---------------------


    //------------------
    // Вычисляем факториал
    //-------------------------
    while (factor.kol>1)
    {
      // запускаем процессы вычисления факториала
      for (int i = 0; i < pnum; i++)
      {
        if (pthread_create(&threads[i], NULL, (void *)calcFact,
              (void *)&factor) != 0) 
          {
            perror("pthread_create");
            exit(1);
          }
      }

     // ожидание завершения процессов

      for (int i = 0; i < pnum; i++)
      {
        if (pthread_join(threads[i], NULL) != 0) 
        {
        perror("pthread_join");
        exit(1);
        }
      }
      // конец ожидания процессов

      // вывод результата
      printf("Массив после прохода \n");
      for (int i = 0; i < kol; i++)
      {
        printf("%llu ",mf[i]);
      }
      printf("\n");
      // конец вывода результата

      // Сжатие массива
      for (int i = 1; i < kol/2+kol%2; i++)
      {
        mf[i]=mf[i*2];
        mf[i*2]=0;
      }
      // конец Сжатие массива

      // вывод результата после сжатия
      printf("Массив после сжатия \n");
      for (int i = 0; i < kol; i++)
      {
        printf("%llu ",mf[i]);
      }
        printf("\n");
      // конец вывода результата после сжатия
      
      // корректируем массив для вычисления факториала
      factor.kol=factor.kol/2+factor.kol%2;
      factor.i=0; // индекс для обработки
      //конец корректируем массив для вычисления факториала
    }
    //------------------
    // Конец Вычисляем факториал
    //-------------------------
  

  
    memset(buf,'\0',sizeof(buf));
    printf("%d %d result %llu\n",b-kol,b,*factor.f);
    sprintf(buf,"%llu",*factor.f);
    write(client_sockfd,buf,strlen(buf));
    close(client_sockfd);
  }
  
  free(threads);
  free(mf);
  return 0;
}