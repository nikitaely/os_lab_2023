#include "revert_string.h"
#include <string.h>
void RevertString(char *str)
{
	char x;
	int i, len;
	len=strlen(str);
	for (i=0;i<len/2;i++)
	{
		x=str[i];
		str[i]=str[len-i-1];
		str[len-i-1]=x;
	} 
}

