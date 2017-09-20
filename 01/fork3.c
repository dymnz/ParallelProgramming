#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  pid_t pid;
  int status;

  switch (pid = fork()){
    case -1:  /* parent error */
      printf("fork error");
      exit (EXIT_FAILURE); 
    case 0: /* child */
      sleep (5);  // slow down child process
      printf ("Child:\n");
      printf ("PID: %d\n", getpid());
      printf ("PPID: %d\n", getppid());
      return 3;
    default: /* parent */
      printf ("Parent:\n");
      printf ("PID: %d\n", getpid());
      printf ("PPID: %d\n", getppid());
      wait (&status);
      printf ("Child terminated with status: %d\n", WEXITSTATUS(status));
  }
  return 0;
}
