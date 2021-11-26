#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/*
  Args:
    char * path: Path you wish to cd into
  Function:
    Changes directory to path
  Returns:
    returns success or error from chdir(path)
*/
int cd(char * path){
  int result = chdir(path);

  if (result == -1){
    printf("%s\n", strerror(errno));
  }

  return result;
}

/*
  Args:
  Function:
    calls exit(0);
  Returns:
    void
*/
void cexit(){
  exit(0);
}
