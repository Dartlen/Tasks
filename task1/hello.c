#include <stdio.h>
#include "libgoodbye.h"
#include "libhello.h"

int main()
{
	char *tmp = hello();
	printf("%s",tmp);
	
	tmp = goodbye();
	printf("%s",tmp);
	
	return 0;
}
