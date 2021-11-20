#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "commands.h"

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

	char * permLine = calloc(strlen(line), 1);
	strcpy(permLine, line);

	char * token = strsep(&permLine, " ");

	int i = 0;

	if (token){
		args[0] = token;
		i = 1;
	}

	while (token){
		token = strsep(&permLine, " ");
		//printf("%d\n", strcmp(token, " "));
		args[i] = token;
		//printf("%d: %s\n", i, args[i]);
		i = i + 1;
	}

	args[i] = NULL;

	free(copied);

	return args;
}

/*
	Args:
		char * command: raw user inputted command that user wants to run
	Function:
		Executes the command that the user wants to run using helper functions that parse the args
	Returns:
		void
*/
int execute(char * command){
	char * formattedCommand = format_command(command);
	char ** args = parse_args(formattedCommand);

	int subprocess = fork();

	//child process
	if (subprocess == 0){
		if (strcmp(args[0], "cd") == 0){
			//printf("%s\n", args[1]);
			args[1] = strsep(args + 1, "\t");
			return cd(args[1]);
		} else if (strcmp(args[0], "exit") == 0){
			cexit();
		} else {
			int status = execvp(args[0], args);

			if (errno){
				printf("%s\n", strerror(errno));
			}
			return status;
		}
	} else {
		if (strcmp(args[0], "exit") == 0){
			cexit();
		}
		int status = 0;
		int waitStatus = wait(&status);
		//printf("CHILD PROCESS HAS COMPLETED\n");
		free(formattedCommand);
		int i = 0;
		for (i = 0; i < (sizeof(args)/8) - 1; i++){
			if (args[i]){
				free(args[i]);
			}
		}

		return WEXITSTATUS(status);
	}
}

int main(){
	int running = 1;

	while (running){
		char * holder;
		printf("\033[0;31m");
		printf("Shell $: ");
		printf("\033[0m");
		fflush(stdout);
		//read(STDIN_FILENO, holder, 256);
		fgets(holder, 256, stdin);

		//printf("%s\n", holder);
		holder = strsep(&holder, "\n");
		//printf("%s\n", holder);

		execute(holder);
		//printf("\n");
	}

	/*char test[100] = "   ls       -a    -l";
	//char test[] = "ls";
	//printf("%s\n", format_command(test));

	char ** args = parse_args(format_command(test));

	execute(test);
	*/
	return 0;
}
