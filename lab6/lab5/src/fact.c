/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *  Modified by A.Kostin
 ********************************************************
 * mutex.c
 *
 * Simple multi-threaded example with a mutex lock.
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

//#define debug

void do_one_thing(int *);
void do_another_thing(int *);
void do_wrap_up(int);
int common = 0; /* A shared variable for two threads */
int r1 = 0, r2 = 0, r3 = 0;
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

int main(int argc, char **argv) {
  pthread_t *threads; // массив потоков
  long long *mf=NULL;
  long long ans;
  int kol=-1, // число факториала
  pnum=-1, // кол-во потоков
  mod=-1; // остаток от деления
  char buffer[BUFSIZ+1]; // буфер для форматирования текста

  #ifdef  debug
  char st0[]="prog"; // название откомпилировнной программы 
  char st1[]="-k"; // факториал числа
  char st2[]="9";
  char st3[]="--pnum"; // кол-во потоков
  char st4[]="4";
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
    memset(buffer,'\0',BUFSIZ ); // обнуляем буфер
    static struct option options[] = {{"kol", required_argument, 0, 'k'},
                                      {"pnum", required_argument, 0, 'p'},
                                      {"mod", required_argument, 0, 'm'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "k:p:m:", options, &option_index);

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
      case 'k': // ввод факториала
        kol = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",kol);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --kol or -k\n");
              return -1;
            }
        break;
      case 'm': // ввод делителя
        mod = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",mod);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --mod\n");
              return -1;
            }

        break;
      case 'p': // ввод кол-во потоков
        pnum = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",pnum);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --pnum\n");
              return -1;
            }

        break;  
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (kol == -1 || pnum == -1 || mod == -1) {
    printf("Usage: %s --kol \"num\" --pnum \"num\" --mod \"num\" \n",
           argv[0]);
    return 1;
  }
  // factor.kol=kol;
  // printf("%llu \n",factor.f);
  //---------------------
  //--- Конец Разбора параметоров комндной строки
  //-------------------------


  //-----------------------
  //--выделяем память
  //-------------------
  
  // Выделяем память под процессы
  threads=(pthread_t *) malloc(sizeof(pthread_t) * pnum);
  // выделяем память под массив факториалов
  mf=(long long *) malloc(sizeof(long long)*kol );
  printf("Массив для вычисления факториала \n");
  for (int i = 0; i < kol; i++)
  {
    mf[i]=i+1;
    printf("%llu ",mf[i]);
  }
    printf("\n");
  //-----------------------
  //--конец выделения памятя
  //-------------------

  //---------------------------
  //---- заполнение структуры 
  //---------------------
  factor.f=mf; // копируем адрес массива
  factor.i=0; // индекс для обработки
  factor.kol=kol; // размер массива
  //---------------------------
  //----конец заполнения структуры 
  //---------------------

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
    ans=factor.f[0]%mod;
    printf("%d!=%llu\n",kol,factor.f[0]);
    printf("%d! MOD %d= %llu\n",kol,mod, ans);

  free(mf);
  return 0;
}