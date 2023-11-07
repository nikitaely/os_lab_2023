#include <stdio.h>
#include <stdlib.h>

#include "find_min_max.h"
#include "utils.h"


//#define debug
#ifdef debug
#include "find_min_max.c"
#include "utils.c"
char st0[]=" ";
char st1[]="10";
char st2[]="12";
char *st[]={st0,st1 ,st2};
#endif

int main(int argc, char **argv) {
  #ifdef debuf
  argc=3;
  st[0]=argv[0];
  argv=st;
  #endif

  int i;
  if (argc != 3) {
    printf("Usage: %s seed arraysize\n", argv[0]);
    return 1;
  }

  int seed = atoi(argv[1]);
  if (seed <= 0) {
    printf("seed is a positive number\n");
    return 1;
  }

  int array_size = atoi(argv[2]);
  if (array_size <= 0) {
    printf("array_size is a positive number\n");
    return 1;
  }

  int *array = malloc(array_size * sizeof(int));
  GenerateArray(array, array_size, seed);
  struct MinMax min_max = GetMinMax(array, 0, array_size-1);
  printf("Source array \n");
  for(i=0;i<array_size;i++)
  {
	  printf("%d ",array[i]);
  }
  printf("\n");
  free(array);
  
  printf("min: %d\n", min_max.min);
  printf("max: %d\n", min_max.max);

  return 0;
}
