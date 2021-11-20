#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
	Cleans up char * line so that repetitive spaces are removed.
	Returns a cleaned up string that should only have spaces where neccessary.
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

/*
	Assumes that the given char * line was cleaned using format_command.
	returns char ** that is ready to be execvp'ed.
*/
char ** parse_args(char * line){
	char * copied = calloc(strlen(line), 1);
	strcpy(copied, line);

	int argCount = 0;
	while (strsep(&copied, " ")){
		argCount = argCount + 1;
	}

	//printf("%d\n", argCount);

	char ** args = calloc((argCount + 1), 8);

	char * token = strsep(&line, " ");

	int i = 0;

	if (token){
		args[0] = token;
		i = 1;
	}

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
	char test[100] = "   ls       -a    -l";
	//char test[] = "ls";
	//printf("%s\n", format_command(test));

	char ** args = parse_args(format_command(test));

	execvp(args[0], args);

	return 0;
}
