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
#include <sys/stat.h>
#include <fcntl.h>

#include <getopt.h>

int main(int argc, char *argv[])
{
    char buffer[BUFSIZ+1];
    int res,k=0;
    int pnum=3;
    int *pipe_fd;
    int stat_val;
    // memset(buffer,'\0',BUFSIZ );
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

    pipe_fd=malloc(sizeof(int)*pnum);
    // проверяем наличие файла буфера
    for (int i = 0; i < pnum; i++)
    {
        /* code */
        memset(buffer,'\0',BUFSIZ );
        sprintf(buffer,"./buf/fifo%d",i);
        if (access(buffer,F_OK)==-1)
        {
            // создаем файл буфер
            res = mkfifo(buffer,0777);
            if (res!=0)
            {
                printf("Error created fifo%d\n",i);
            }
        }
    }
    
    
    // 
    for (int i = 0; i < pnum; i++)
    {
        /* code */
        memset(buffer,'\0',BUFSIZ );
        sprintf(buffer,"./buf/fifo%d",i);
        pid_t pid=fork();
        switch (pid)
        {
        case -1:
            printf("Error created pid\n");
            break;
        case 0:
            // childen
            pipe_fd[i]=open(buffer,O_WRONLY);
            k=k+i+1;
            printf("pid %d = %d\n",i,k);
            // готовим данные для отправки в буфер
            sprintf(buffer,"%d",k);
            write(pipe_fd[i],buffer,BUFSIZ);
            
            memset(buffer,'\0',BUFSIZ );
            sprintf(buffer,"./buf/fifo%d",i);
            close(buffer);
            return 0;
            break;
        default:
            // parent
            pipe_fd[i]=open(buffer,O_RDONLY);
            break;
        }

    }

    // ожидаем пока все процессы отработают
    for (int i = 0; i < pnum; i++)
    {
    wait(&stat_val);
    }

    for (int i = 0; i < pnum; i++)
    {

        read(pipe_fd[i],buffer,BUFSIZ);
        printf("fifo%d %s\n",i,buffer);
        close(pipe_fd[i]);
    }
    

    free(pipe_fd);
    k++;
    return 0;
}