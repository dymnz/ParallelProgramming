#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int pid = fork();
    
  if (pid == 0)
      printf("I am the child.\n");
	else 
	    printf("I am the parent.\n");
	  
  if (pid == 0)
      exit(0);
  else
      wait(0);
                          
	return 0;
}
