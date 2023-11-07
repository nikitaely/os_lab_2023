#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char path[]="./sequential_min_max";
    char seed[10],array_size[10];
    printf("Input seed\n");
    scanf("%s", &seed);
    printf("Input array size\n");
    scanf("%s",&array_size);
    execl(path,"sequential_min_max",seed,array_size,0);
    return 0;
}

