#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int k=0,stat;
    pid_t pid=fork();
    switch (pid)
    {
    case -1:
        printf("Error fork \n");
        break;
    case 0:
        
        return 1;
        break;
    
    default:
        printf("\nParents process pid=%d\n",getpid());
        printf("Child   process pid=%d\n",pid);
        
        while (1)
        {
            k++;
        }
        
        break;
    }


    return k;
}