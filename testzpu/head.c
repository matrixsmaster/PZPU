#include <stdio.h>

int main(int argc, char **argv)
{
	int c;
	printf("Hello world!\n");
	printf("Press any key!\n");
	c=inbyte();
	printf("You pressed (%02x) '%c'\n", c, c);
	return 0;
}
