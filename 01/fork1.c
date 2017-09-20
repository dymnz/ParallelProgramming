#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	printf("hello\n");
	fork();
	printf("bye\n");
	  
	return 0;
}
