#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	int c;
	do {
		printf("Hello world!\n");
		printf("Press any key!\n");
		c=inbyte();
		printf("You pressed (%02x) '%c'\n", c, c);
		printf("Time is %u\n",time(NULL));
	} while (c != 'q');
	return 0;
}
