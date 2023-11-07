#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int w=1;

int pnum=4;
typedef struct 
{
    /* data */
    pid_t pid;
    int stat_loc;
} pid_stat_t;

pid_stat_t* pid_stat;

void interrupt(int sig)
{
    for (int j = 0; j < pnum; j++)
    {
        /* code */
        if (pid_stat[j].stat_loc==0)
        {
            kill(pid_stat[j].pid,SIGKILL);
        }
    }
    
}
int main()
{
    int d=0;
    int i=0;
    int stat_loc;
    pid_t p;
    // count pid processes
    
    printf("Start program\n");
    // выделяем память 
    pid_stat=(pid_stat_t*) malloc(sizeof(pid_stat_t)*pnum);

    for ( i = 0; i < pnum; i++)
    {
        /* code */
        p=fork();
        switch (p)
        {
        case -1:
            /* code */
            printf("error fork\n");
            return -1;
            break;
        case 0:
            // child
            if (i%2)
            {
                return i+1;
            }
            else
            {
                
                while (1)
                {
                    d++;
                }
                
                return i*4+1;
            }
            break;
        default:
            // parent
            pid_stat[i].pid=p;
            pid_stat[i].stat_loc=0;
            break;
        }
    }
    signal(SIGALRM,interrupt);

    alarm(10);

    for ( i = 0; i < pnum; i++)
    {
        /* code */
        p=wait(&stat_loc);
        for (int j = 0; j < pnum; j++)
        {
            if (pid_stat[j].pid==p)
                {
                    pid_stat[j].stat_loc=stat_loc;
                    printf("Pid %d, state %d, return %d\n",p,WIFEXITED(stat_loc),WEXITSTATUS(stat_loc));

                    break;

                }
        }
        
    }
    printf("\ntable\n");
    for ( i = 0; i < pnum; i++)
    {
        printf("Pid %d, state %d, return %d\n",pid_stat[i].pid,WIFEXITED(pid_stat[i].stat_loc),WEXITSTATUS(pid_stat[i].stat_loc));
    }

    //printf("Process %d, return %d \n",p,WEXITSTATUS(stat_loc));
/*
    while (i<10);
    {
        printf("%d\n",i++);
    }
        
*/
    free(pid_stat);
    return 0;
}