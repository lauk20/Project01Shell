#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int cd(char * path){
  int result = chdir(path);

  if (result == -1){
    printf("%s\n", strerror(errno));
  }

  return result;
}

void cexit(){
  exit(0);
}
