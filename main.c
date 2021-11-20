#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
cleans up char * line so that repetitive spaces are removed.
returns a cleaned up string that should only have spaces where neccessary.
*/
char * format_command(char * line){
	char * newCommand = calloc(strlen(line), 1);

	int i = 0;
	char * start = 0;
	char * lastSpace = 0;
	while (i < strlen(line)){
		if (line[i] == ' '){
				lastSpace = line + i;
		} else {
			if (lastSpace && start){
				strncat(newCommand, lastSpace, 1);
				lastSpace = 0;
			}

			if (!start){
				start = line + i;
				lastSpace = 0;
			}

			strncat(newCommand, line + i, 1);
		}

		i = i + 1;
	}

	return newCommand;
}

char ** parse_args(char * line){
	char * copied = malloc(strlen(line));
	strcpy(copied, line);

	char * spaceToken = strsep(&copied, " ");
	int countArgs = 0;
	/*if (spaceToken || strlen(line) > 0){
		countArgs = countArgs + 1;
	}*/

	while (spaceToken){
		spaceToken = strsep(&copied, " ");
		countArgs = countArgs + 1;
	}

	//printf("%d\n", countArgs);

	char ** args = malloc((countArgs + 1) * 8);

	char * token = strsep(&line, " "); //" ls -a"

	//printf("%s\n", token);

	int i = 0;

	if (!token){
		args[0] = token;
		i = 1;
	}

	//printf("%d: %s\n", 0, args[0]);


	while (token){
		token = strsep(&line, " ");
		//printf("%d\n", strcmp(token, " "));
		args[i] = token;
		//printf("%d: %s\n", i, args[i]);
		i = i + 1;
	}

	args[i] = NULL;


	return args;
}

int main(){
	char test[100] = "   ls       -a    -b";

	printf("%s\n", format_command(test));

	char ** args = parse_args(test);

	execvp(args[0], args);

	return 0;
}
