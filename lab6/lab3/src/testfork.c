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

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#include "find_min_max.c"
#include "utils.c"




int main(int argc, char **argv) 
{
    int k=0, pnum=10,p=0,output=0,count;
    int file_pipes[2];
    int stat_val;
    char buffer[BUFSIZ+1];
    memset(buffer,"\0",sizeof(buffer));
    sprintf(buffer,"%d", pnum);
    sscanf(buffer,"%d",&p);
    memset(buffer,"\0",sizeof(buffer));
    k+=2;

    
    
    count=3;
    int *pipes_ar=malloc(sizeof(int)*count*2);
    
    for (int i = 0; i < count; i++)
    {
        if (pipe(&pipes_ar[i*2])==0)
        {
            printf("pipe created %d \n",i);
        }
        /* code */
    }
    
    


    for (int i = 0; i < count; i++)
    {
        /* code */
    
    
        pid_t child_pid2 = fork();
        switch (child_pid2)
        {
        case -1:
            printf("Failure");
            /* code */
            break;
        case 0:
            // child
            k=k+1+i;
            memset(buffer,"\0",sizeof(buffer));
            printf ("Child-%d = %d \n",i,k);
            sprintf(buffer,"%d ",k);
            write(pipes_ar[i*2+1], buffer, strlen(buffer));

            return 0;
            break;
        default:
            // parent

            break;
        }
    }
    for (int i = 0; i < count; i++)
    {
    wait(&stat_val);
    }
    for (int i = 0; i < count; i++)
    {
        memset(buffer,"\0",sizeof(buffer));
        read(pipes_ar[i*2], buffer, BUFSIZ);
        printf("str - %s \n",buffer);
    }
    free(pipes_ar);
    return 0;
}
