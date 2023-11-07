#include "revert_string.h"
#include <string.h>
void RevertString(char *str)
{
	char temp;
	int i,len;
	len=strlen(str);
	for(i=0;i<len/2;i++)
	{
		temp=str[i];
		str[i]=str[len-i-1];
		str[len-i-1]=temp;
	}
	// your code here
}

