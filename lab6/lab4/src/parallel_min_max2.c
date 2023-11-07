#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <getopt.h>
//include <signal.h>

#include "find_min_max.h"
#include "utils.h"


#define debug
#ifdef debug
#include "find_min_max.c"
#include "utils.c"
#endif

static int pnum_ipc=-1;

typedef struct 
{
    /* data */
    pid_t pid;
    int stateOK; // 1 - program OK, 0- error
    int result;
} pid_stat_t;
// указатель на массив процессов 
pid_stat_t* pid_stat;
/*
void interrupt(int sig)
{
    for (int j = 0; j < pnum_ipc; j++)
    {
        // code 
        if (pid_stat[j].stateOK==0)
        {
            kill(pid_stat[j].pid,SIGKILL);
            printf("Kill pid %d \n",pid_stat[j].pid);
        }
    }
    
}
*/
int main(int argc, char **argv) {
  
  int *array=NULL;
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout=0;
  bool with_files = false;
  bool loop=false;
  int seed_loop=0;
  int pas;
  int stat_val; // считывание данных 
  char buffer[BUFSIZ+1]; // буфер для форматирования текста
  int res;// результат создания ФИФО буфера
  // ввод данных место командной строки
  #ifdef  debug
  char st0[]="prog"; // название откомпилировнной программы 
  char st1[]="--seed"; // начальная точка для генерации чисел
  char st2[]="10";
  char st3[]="--array_size"; // Размер массива
  char st4[]="11";
  char st5[]="--pnum"; // кол-во процессов для сортировки
  char st6[]="3";
  char st7[]="--by_files"; // выбор способа межпроцессорного
                              // взаимодействия через FIFO буфер
  char st8[]="--timeout"; 
  char st9[]="10";
  char *argv2[]={st0,st1,st2,st3,st4,st5,st6,st7,st8,st9 };
  argc=10; // кол-во аргументов в argv2
  argv=argv2; // подмена аргументов командной строки своей
  #endif


  
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 't'},
                                      {"loop", required_argument, 0, 'l'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "ft:l:", options, &option_index);

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
          case 1:
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
          case 2:
            pnum = atoi(optarg);
            pnum_ipc=pnum;
            // your code here
            // error handling
            sprintf(buffer,"%d",pnum);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --pnum\n");
              return -1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            // timeout
            timeout = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",timeout);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --timeout\n");
              return -1;
            }
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;
      case 't':
        // timeout
            timeout = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",timeout);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --timeout\n");
              return -1;
            }
        break;
      case 'l':
        loop=true;
        seed_loop = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",seed_loop);
            if  (strcmp(buffer,optarg)!=0)
            {
              printf("Error input numeric --loop\n");
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

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  // Выделяем память под массив процессов и указатель на результат выполнения
  pid_stat=(pid_stat_t*) malloc(sizeof(pid_t)*pnum);

  // Выделяем память под массив и проверяем 
  array = malloc(sizeof(int) * array_size);

  if (array==NULL)
  {
    printf("Error malloc array\n");
    return -1;
  }
  GenerateArray(array, array_size, seed);
  
  // выводим исходный массив
  printf("Source array\n");
  for (int i = 0; i < array_size; i++)
  {
    printf("%d ",array[i]);
    /* code */
  }
  printf("\n"); // только после перевода каретки данные выводятся в терминал

  // создаем указтель на массив межпроцессорных каналов
  int *pipes_ar;
  // Создаем указатель на массив ФИФО Буферов
  int *fifo_ar;
  if (!with_files) 
  {
    // IPC через pipe
    // Создаем массив для указателей для каналов
    // в котором четные для чтения, нечетные для записи
    pipes_ar=malloc(sizeof(int)*pnum*2);
    if (pipes_ar!=NULL)
    {
      printf("Created array pipes\n");
      for (int i = 0; i < pnum; i++)
      {
          // Создаем каналы и проверяем за одной что каналы созданы
          if (pipe(&pipes_ar[i*2])==0)
          {
            printf("pipe created %d \n",i);
          }
          else
          { 
            // если ошибка то выходим из программы
            printf("Failure pipe created %d \n",i);
            return -1;
          }
      }
    }
    else
    {
      // если ошибка то выходим из программы
      printf("Failure malloc array pipes\n");
      return -1;
    }
  }
  else
  {
    // Взаимодействие через ФИФО буфер
     // проверяем наличие папки для буферов
    if (access("./buf",F_OK)==-1)
    {
        // создаем папку  
        res=mkdir("./buf",0777);
        if (res!=0)
        {
            printf("Error created dir\n");
        }
    }
    // создаем массив дескрипторов
    fifo_ar=malloc(sizeof(int)*pnum);
    // проверяем наличие файла буфера
    for (int i = 0; i < pnum; i++)
    {
        /* code */
        memset(buffer,'\0',BUFSIZ );
        sprintf(buffer,"./buf/fifo%d",i);
        if (access(buffer,F_OK)==-1)
        {
            // создаем файл буфер если файл ФИФО не существует
            res = mkfifo(buffer,0777);
            if (res!=0)
            {
                printf("Error created fifo%d\n",i);
            }
        }
    }
  }

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Кол-во запущенных процессов
  int active_child_processes = 0;
  // Вычисляем границы
  int sizemin=array_size/pnum;
  int ost=array_size%pnum;


  for (int i = 0; i < pnum; i++) {
    // Подготавливаем название файла
    if (with_files) {
          // use files here
          memset(buffer,'\0',BUFSIZ );
          sprintf(buffer,"./buf/fifo%d",i);
        } 
        
    pid_t child_pid = fork();

    switch (child_pid)
    {
    case -1:
      // code 
      // error
      printf("Fork failed!\n");
      return -1;
      break;
    case 0:

      // child process
        // Открываем файл для записи если мы используем ФИФО
         /*
        if (with_files) {
          // use files here
          // сохраняем дескриптор файла
          fifo_ar[i]=open(buffer,O_WRONLY);
        } 
       
        printf("Process %d\n", getpid() );
        if (i== seed_loop && loop)
        {
          printf("looping\n");

          while(1);
        }
        struct MinMax min_max;
        if (i+1<=ost)
        {
          min_max=GetMinMax(array, i*(sizemin+1), (i+1)*(sizemin+1)-1);
          
          for (int j = i*(sizemin+1); j <= (i+1)*(sizemin+1)-1; j++)
          {
            printf("%d ",array[j]);
            // code 
          }
          printf("\nMin=%d Max=%d\n",min_max.min,min_max.max);
          
        }
        else
        {
          min_max=GetMinMax(array, ost*(sizemin+1)+(i-ost)*sizemin, ost*(sizemin+1)+(i-ost+1)*sizemin-1 );
         
          for (int j = ost*(sizemin+1)+(i-ost)*sizemin; j <= ost*(sizemin+1)+(i-ost+1)*sizemin-1; j++)
          {
            printf("%d ",array[j]);
            // code 
          }
          printf("\nMin=%d Max=%d\n",min_max.min,min_max.max);
        }
        // parallel somehow
          memset(buffer,'\0',sizeof(buffer));
          sprintf(buffer,"%d %d",min_max.min,min_max.max);
        if (with_files) {
          // use files here
          // закрываем файл
          write(fifo_ar[i],buffer,BUFSIZ);
          close(fifo_ar[i]);
        } else {
          // use pipe here
          
          write(pipes_ar[i*2+1], buffer, strlen(buffer));
        }
          */
        return 0;
      break;
    default:
      // parent
      active_child_processes += 1;
      // parent process
        // save pid child
        
        pid_stat[i].pid=child_pid;
        pid_stat[i].stateOK=0;
        pid_stat[i].result=0;
        /*
        if (with_files)
        {
          fifo_ar[i]=open(buffer,O_RDONLY);
        }
        */
      break;
    }
    
/*
    if (child_pid >= 0) 
    {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) 
      {
        // child process
        // Открываем файл для записи если мы используем ФИФО
        if (with_files) {
          // use files here
          // сохраняем дескриптор файла
          fifo_ar[i]=open(buffer,O_WRONLY);
        } 

        struct MinMax min_max;
        if (i+1<=ost)
        {
          min_max=GetMinMax(array, i*(sizemin+1), (i+1)*(sizemin+1)-1);
          printf("Process %d\n",i);
          for (int j = i*(sizemin+1); j <= (i+1)*(sizemin+1)-1; j++)
          {
            printf("%d ",array[j]);
            // code 
          }
          printf("\nMin=%d Max=%d\n",min_max.min,min_max.max);
          
        }
        else
        {
          min_max=GetMinMax(array, ost*(sizemin+1)+(i-ost)*sizemin, ost*(sizemin+1)+(i-ost+1)*sizemin-1 );
          printf("Process %d\n",i);
          for (int j = ost*(sizemin+1)+(i-ost)*sizemin; j <= ost*(sizemin+1)+(i-ost+1)*sizemin-1; j++)
          {
            printf("%d ",array[j]);
            // code 
          }
          printf("\nMin=%d Max=%d\n",min_max.min,min_max.max);
        }
        // parallel somehow
          memset(buffer,'\0',sizeof(buffer));
          sprintf(buffer,"%d %d",min_max.min,min_max.max);
        if (with_files) {
          // use files here
          // закрываем файл
          write(fifo_ar[i],buffer,BUFSIZ);
          close(fifo_ar[i]);
        } else {
          // use pipe here
          
          write(pipes_ar[i*2+1], buffer, strlen(buffer));
        }
        return 0;
      }
      else
      {
        // parent process
        // save pid child
        pid_stat[i].pid=child_pid;
        pid_stat[i].stat_loc=0;
        if (with_files)
        {
          fifo_ar[i]=open(buffer,O_RDONLY);
        }
      }

    } 
    else 
    {
      printf("Fork failed!\n");
      return -1;
    }
    */
  }
  free(array);
  // setting interrupt function and flag
  signal(SIGALRM,interrupt);
  // setting timeout
  if (loop)
  {
    alarm(timeout);
  }
  // Ждем окончания процессов
  while (active_child_processes > 0) {
    // your code here
    pid_t p= wait(&stat_val);
    // Ищем номер процесса для сохранения статуса процесса
    for (int j = 0; j < pnum; j++)
        {
            if (pid_stat[j].pid==p)
                {
                    pid_stat[j].stateOK = WIFEXITED(stat_val);
                    pid_stat[j].result = WEXITSTATUS(stat_val);

                    printf("Pid %d, state %d, return %d\n",
                           pid_stat[j].pid, pid_stat[j].stateOK, pid_stat[j].result);                       
                    if (pid_stat[j].stateOK==0)
                    {
                      printf("Error overtime process %d\n",pid_stat[j].pid);
                      return -1;
                    }
                    break;
                }
        }
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    memset(buffer,'\0',sizeof(buffer));
    if (with_files) {
      // read from files
      read(fifo_ar[i],buffer,BUFSIZ);
      close(fifo_ar[i]);
    } else {
      // read from pipes
      read(pipes_ar[i*2], buffer, BUFSIZ);
    }
    printf("Process %d = %s \n",i,buffer);
    sscanf(buffer,"%d %d", &min,&max);

    if (min < min_max.min) 
    {
      min_max.min = min;
    }
    if (max > min_max.max) 
    {
      min_max.max = max;
    }
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  if (with_files)
  {
    free(fifo_ar);
  }
  else
  {
    free(pipes_ar);
  }

  free(pid_stat);
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
