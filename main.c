#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char ** parse_args(char * line){
	char * copied = malloc(strlen(line));
	strcpy(copied, line);
	
	char * spaceToken = strsep(&copied, " ");
	int countArgs = 0;
	if (spaceToken || strlen(line) > 0){
		countArgs = countArgs + 1;
	}

	while (spaceToken){
		spaceToken = strsep(&copied, " ");
		countArgs = countArgs + 1;
	}
	
	printf("%d\n", countArgs);
	
	char ** args = malloc(7 * 8);

	char * token = 1;

	int i = 0;
	while (token){
		token = strsep(&line, " ");
		args[i] = token;
		//printf("%d: %s\n", i, args[i]);
		i = i + 1;
	}

	args[i] = NULL;
	return args;
}

int main(){
	char test[100] = "ls -a -l";

	char ** args = parse_args(test);
	
	execvp(args[0], args);
	
	return 0;
}
