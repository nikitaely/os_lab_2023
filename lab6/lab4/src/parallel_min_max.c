#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include "find_min_max.h"
#include "utils.h"

//#define debug
#ifdef debug
#include "find_min_max.c"
#include "utils.c"
#endif

 int pnum_ipc=-1;
 int pnum = -1;
typedef struct 
{
    /* data */
    pid_t pid;
    int stateOK; // 1 - program OK, 0- error
    int result;
} pid_stat_t;
// указатель на массив процессов 
pid_stat_t* pid_stat=NULL;

void interrupt(int sig)
{
    for (int j = 0; j < pnum; j++)
    {
        // code 
        if (pid_stat[j].stateOK==0)
        {
            kill(pid_stat[j].pid,SIGKILL);
            printf("Kill process %d, pid %d \n",j,pid_stat[j].pid);
        }
    }
}

int main(int argc, char **argv) {
  

  int seed = -1;
  int array_size = -1;
  //int pnum = -1;
  bool with_files = false;
  int pas;
  int stat_val; // считывание данных 
  char buffer[BUFSIZ+1]; // буфер для форматирования текста
  int res;// результат создания ФИФО буфера
  int timeout=0; // Время для ожидания
  bool loop=false; // включение зацикливания
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
  char st8[]="--timeout";
  char st9[]="10";
  char st10[]="--loop";
                              // взаимодействия через FIFO буфер 
  char *argv2[]={st0,st1,st2,st3,st4,st5,st6,st7,st8,st9,st10 };
  argc=7; // кол-во аргументов в argv2
  argv=argv2; // подмена аргументов командной строки своей
  #endif


  //---------------------
  //--- Разбор параметоров комндной строки
  //-------------------------
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 't'}, // ввод данных времени ожидания
                                      {"loop", no_argument, 0, 'l'}, // включение зацикливания
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

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;
      case 'l':
        loop = true;
        break;
      case 't':
        timeout = atoi(optarg);
            // your code here
            // error handling
            sprintf(buffer,"%d",timeout);
            if  (strcmp(buffer,optarg)!=0 || timeout<0)
            {
              printf("Error input numeric --timeout\n");
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
  //---------------------
  //--- Конец Разбора параметоров комндной строки
  //-------------------------

  //-----------------------------------------------
  // Выделяем память под массив и проверяем 
  //--------------------------------
  int *array = malloc(sizeof(int) * array_size);
  if (array==NULL)
  {
    printf("Error malloc array\n");
    return -1;
  }
  // Выделяем память под массив состояний процессов
  pid_stat = (pid_stat_t *) malloc(sizeof(pid_stat_t) * pnum);
  if (pid_stat==NULL)
  {
    printf("Error malloc pid_stat\n");
    return -1;
  }

  //----------------------------------------------
  //----- конец выделения памяти
  //---------------------------------

  //-------------------------------------
  //----Генерируем исходный массив и выводим массив 
  //-----------------------------

  GenerateArray(array, array_size, seed);
 
  // выводим исходный массив
  printf("Source array\n");
  for (int i = 0; i < array_size; i++)
  {
    printf("%d ",array[i]);
    /* code */
  }
  printf("\n"); // только после перевода каретки данные выводятся в терминал
  //-------------------------------------
  //----Конец генерирования исходный массива 
  //-----------------------------
  
//----------------------------------------------------
// --- Создание каналов для IPC
//-----------------------------------------------
  // создаем указатель на массив межпроцессорных каналов
  int *pipes_ar=NULL;
  int *fifo_ar=NULL;
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
        else{
          printf("Created dir ./buf\n");
        }
    }
    else{
      printf("Exists dir ./buf\n");
    }
    // создаем массив дескрипторов
    fifo_ar=(int* ) malloc(sizeof(int)*pnum);
    // проверяем наличие файла буфера
    for (int i = 0; i < pnum; i++)
    {
        // code
        memset(buffer,'\0',BUFSIZ );
        sprintf(buffer,"./buf/fifo%d",i);
        if (access(buffer,F_OK)!=-1)
        {
          printf("Delete buffer %s\n",buffer);
          unlink(buffer);  
        }
        // создаем файл буфер если файл ФИФО не существует
        res = mkfifo(buffer,0777);
        if (res!=0)
        {
            printf("Error created %s\n",buffer);
        }
        else {
          printf("Created buffet %s\n",buffer);
        }

    }
  }
//------------------------------
//-------- Конец создания каналов IPC
//--------------------------------------
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Кол-во запущенных процессов
  int active_child_processes = 0;
  // Вычисляем границы для распределения массива по процессам
  int sizemin=array_size/pnum;
  int ost=array_size%pnum;


  for (int i = 0; i < pnum; i++) {
    sprintf(buffer,"./buf/fifo%d",i);
    pid_t child_pid = fork();
    switch (child_pid)
    {
    case -1:
      printf("Fork failed!\n");
      return -1;
      break;
    case 0:
    //---------------
    //--- child process
    //--------------------
      if (with_files)
      {
        // закрываем файлы ФИФО буффера для чтения
        for (int j = 0; j < i; j++)
        {
          close(fifo_ar[j]);
        }
        // сохраняем дескриптор файла для записи
          fifo_ar[i]=open(buffer,O_WRONLY);
      }
      
      printf("Process %d pid=%d \n",i,getpid());
      if (loop && i%2)
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
        }
        else
        {
          min_max=GetMinMax(array, ost*(sizemin+1)+(i-ost)*sizemin, ost*(sizemin+1)+(i-ost+1)*sizemin-1 );
          
          for (int j = ost*(sizemin+1)+(i-ost)*sizemin; j <= ost*(sizemin+1)+(i-ost+1)*sizemin-1; j++)
          {
            printf("%d ",array[j]);
            // code 
          }
        }
        printf("\nMin=%d Max=%d\n",min_max.min,min_max.max);
        // parallel somehow
          memset(buffer,'\0',sizeof(buffer));
          sprintf(buffer,"%d %d ",min_max.min,min_max.max);
      if (with_files) {
          // use files here
          write(fifo_ar[i],buffer,BUFSIZ);
           // закрываем файл
          close(fifo_ar[i]);
        } else {
          // use pipe here
          write(pipes_ar[i*2+1], buffer, strlen(buffer));
        }
      return i;
      break;
      ///------------------------
      //--------End child
      //-----------------------------
    default:
      pid_stat[i].pid=child_pid;
      pid_stat[i].stateOK=0;
      // parent
       if (with_files)
        {
          fifo_ar[i]=open(buffer,O_RDONLY);
        }
      break;
    }    

   
  }


  //---------------------------
  //--- Ожидание окончания результата расчета
  //-----------------------
  // Структура для сохранения макс и мин числа
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  // номер завершенного процесса
  int ret; 
  // setting interrupt function and flag
  signal(SIGALRM,interrupt);
  // setting timeout
  if (timeout)
  {
    alarm(timeout);
  }

  for (int i = 0; i < pnum; i++)
  {
    pid_t p=wait(&stat_val);
    if (WIFEXITED(stat_val))
    {
      // процесс завершен штатно
      // получаем номер завершенного процесса
      ret=WEXITSTATUS(stat_val);
      pid_stat[ret].stateOK=1;
      pid_stat[ret].result=ret;
      memset(buffer,'\0',sizeof(buffer));
      if (with_files)
      {
          read(fifo_ar[ret],buffer,BUFSIZ);
          //close(fifo_ar[ret]);
      }
      else
      {
        read(pipes_ar[ret*2], buffer, BUFSIZ);
      }

      int min, max;
      printf("Process %d pid=%d str=%s\n",ret,p,buffer);
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
    else
    {
      // ошибка при выполнении процесса
      printf("Error process %d, pid=%d\n",ret,p);
      goto free_memory; // переход на освобождение памяти
    }
  }
  //---------------------------
  //--- Конец Ожидания окончания результата расчета
  //-----------------------
 

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  //-----------------
  //--------Освобождаем памаять
  //--------------------------
  free_memory:
  free(pid_stat);
  free(array);
  if (with_files)
  {
    // Закрываем все ФИФО буферы
    for (int i = 0; i < pnum; i++)
    {
      close(fifo_ar[i]);
      sprintf(buffer,"./buf/fifo%d",i);
        if (access(buffer,F_OK)!=-1)
        {
          printf("Delete buffer %s\n",buffer);
          unlink(buffer);  
        }
    }
    // освобождаем память
    free(fifo_ar);
  }
  else
  {
    free(pipes_ar);
  }
  //---------------------------
  //---Конец свобождения памяти
  //----------------------------
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  return 0;
}
