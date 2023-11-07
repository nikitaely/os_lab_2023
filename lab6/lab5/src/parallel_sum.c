#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#define debug


struct SumArgs {
  int *array;
  int begin;
  int end;
  int sum;
};

int Sum(const struct SumArgs *args) {
  int sum = 0;
  // TODO: your code here
  for (int i = args->begin; i <=args->end ; i++)
  {
    /* code */
    sum+=(args->array)[i];
  }
   
  return sum;
}
//int *sum;
void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  sum_args->sum=Sum(sum_args);
  //return (void *)(size_t)Sum(sum_args);
  //sum_args->sum=4;
  return 0;
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */
  char buffer[BUFSIZ+1]; // буфер для форматирования текста
  int threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  pthread_t *threads; // массив потоков
  //--------------------
  #ifdef  debug
  char st0[]="prog"; // название откомпилировнной программы 
  char st1[]="--seed"; // начальная точка для генерации чисел
  char st2[]="10";
  char st3[]="--array_size"; // Размер массива
  char st4[]="11";
  char st5[]="--threads_num"; // кол-во процессов для сортировки
  char st6[]="3";
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
    static struct option options[] = {{"seed", required_argument, 0, 's'},
                                      {"array_size", required_argument, 0, 'a'},
                                      {"threads_num", required_argument, 0, 't'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",seed);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --seed\n");
              return -1;
            }
            break;
         
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      //---------обработка коротких параметров
      case 's':
        seed = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",seed);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --seed\n");
              return -1;
            }
        break;
      case 'a':
        array_size = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",array_size);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --array_size\n");
              return -1;
            }

        break;
      case 't':
        threads_num = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",threads_num);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --threads_num\n");
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

  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }
  //---------------------
  //--- Конец Разбора параметоров комндной строки
  //-------------------------

  /*
   * TODO:
   * your code here
   * Generate array here
   */
  //-----------------------
  //--выделяем память
  //-------------------
  
  int *array = malloc(sizeof(int) * array_size);
  threads=(pthread_t *) malloc(sizeof(pthread_t) * threads_num);
  
  struct SumArgs *args=(struct SumArgs *) malloc(sizeof(struct SumArgs) * threads_num);
  //sum=malloc(sizeof( *sum ) * threads_num);
  //-----------------------
  //--конец выделения памятя
  //-------------------

 // Вычисляем границы для распределения массива по процессам
  
  int sizemin=array_size/threads_num;
  int ost=array_size%threads_num;
  
  for (uint32_t i = 0; i < threads_num; i++) 
  {
   //struct MinMax min_max;
        if (i+1<=ost)
        {
          args[i].array=array;
          args[i].begin=i*(sizemin+1);
          args[i].end=(i+1)*(sizemin+1)-1;
          printf("Thread %d\n",i);
          for (int j = args[i].begin; j <= args[i].end; j++)
          {
            printf("%d ",array[j]);
            // code 
          }
          printf("\n");
        }
        else
        {
          args[i].array=array;
          args[i].begin=ost*(sizemin+1)+(i-ost)*sizemin;
          args[i].end=ost*(sizemin+1)+(i-ost+1)*sizemin-1;
           printf("Thread %d\n",i);
          for (int j = args[i].begin; j <= args[i].end; j++)
          {
            printf("%d ",array[j]);
            // code 
          }
          printf("\n");
        }
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  

  int total_sum = 0;
  int the=threads_num;
  int sum = 0;
   int sum2 = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    
    //pthread_join(threads[i], (void **)&sum);
    pthread_join(threads[i], NULL);
    total_sum += args[i].sum;
  }

  free(array);
  free(threads);
  free(args);
  printf("Total: %d\n", total_sum);
  return 0;
}