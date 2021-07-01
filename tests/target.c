#include <stdio.h>
#include <unistd.h>

int g_MyInt = 100;

int 
main()
{
	while (1) {
		printf("MyInt:  %i\n", g_MyInt);
		printf("&MyInt: %p\n", (void *)&g_MyInt);
		sleep(1);
	}

	return 0;
}