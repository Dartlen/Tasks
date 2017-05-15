#include <stdio.h>
#include "libgoodbye.h"

char * hello();
char * goodbye();
int main()
{
	const char *tmp = hello();
	printf("%s",tmp);
	
	goodbye();
	
	return 0;
}
