#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
	Args:
		char * line: string to be cleaned
	Function:
		Cleans up char * line so that repetitive spaces are removed.
	Returns:
		a cleaned up string that should only have spaces where neccessary.
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
	Args:
		char * line: string to be parsed into a char ** using spaces as separators
	Function:
		Assumes that the given char * line was cleaned using format_command.
		Parses string and puts argument into a char ** that is execvp-ready.
	Returns:
		char ** that is ready to be execvp'ed.
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

/*
	Args:
		char * command: raw user inputted command that user wants to run
	Function:
		Executes the command that the user wants to run using helper functions
	Returns:
		void
*/
void execute(char * command){
	char ** args = parse_args(format_command(command));

	int subprocess = fork();

	//child process
	if (subprocess == 0){
		execvp(args[0], args);
	} else {
		int status = 0;
		int waitStatus = wait(&status);
		printf("CHILD PROCESS HAS COMPLETED\n");
	}
}

int main(){
	char test[100] = "   ls       -a    -l";
	//char test[] = "ls";
	//printf("%s\n", format_command(test));

	char ** args = parse_args(format_command(test));

	execute(test);

	return 0;
}
