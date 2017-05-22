#include <stdio.h>
#include "libgoodbye.h"

char * hello();
char * goodbye();
int main()
{
	const char *tmp = hello();
	printf("%s",tmp);
	
	const char *tmpgoodbye = goodbye();
	printf("%s",tmpgoodbye);
	
	return 0;
}
