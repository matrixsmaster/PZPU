#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
#if 0
	int c;
	do {
		printf("Hello world!\n");
		printf("Press any key!\n");
		c=inbyte();
		printf("You pressed (%02x) '%c'\n", c, c);
		printf("Time is %u\n",time(NULL));
	} while (c != 'q');
#else
	printf("Hello world!\n");
#endif
	return 0;
}
