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
		int: WEXITSTATUS
*/
int execute(char * command){
	//printf("raw cmd %ld: %s\n", strlen(command), command);
	char * formattedCommand = format_command(command);
	char ** args = parse_args(formattedCommand);

	if (strcmp(args[0], "exit") == 0){
		cexit();
		return 0;
	}

	//printf("%ld is len: %s\n", strlen(formattedCommand), formattedCommand);
	//printf("%s %s %s\n", args[0], args[1], args[2]);

	int subprocess = fork();

	//child process
	if (subprocess == 0){
		if (strcmp(args[0], "cd") == 0){
			//printf("%s\n", args[1]);
			args[1] = strsep(args + 1, "\t");
			return cd(args[1]);
		} else {
			int status = execvp(args[0], args);
			if (errno){
				printf("%s\n", strerror(errno));
			}

			free(formattedCommand);
			int i = 0;
			for (i = 0; i < (sizeof(args)/8) - 1; i++){
				if (args[i]){
					free(args[i]);
				}
			}
			exit(status);
		}
	} else {
		int status = 0;
		int waitStatus = waitpid(subprocess, &status, 0);
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

/*
	Args:
		char * command: raw user inputted command that user wants to run
	Function:
		Executes the command that the user wants to run using execute function.
		Can handle multiple commands separated by the ";"
	Returns:
		int: 0
*/
int multiexecute(char * command){
	char * heapCommand = calloc(strlen(command), 1);
	strcpy(heapCommand, command);
	//printf("heapCommand: %s\n", heapCommand);
	char * token = strsep(&heapCommand, ";");

	while (token){
		//printf("token: %s\n", token);
		execute(token);
		token = strsep(&heapCommand, ";");
	}

	free(heapCommand);

	return 0;
}

int main(){
	int running = 1;

	printf("\033[0;31m");
	printf("\033[1m");
	printf("WELCOME TO SHELL\n");
	printf("\033[0");

	while (running){
		char * holder = calloc(256, 1);
		printf("\033[0;31m");
		printf("Shell $: ");
		//printf("PID: %d ", getpid());
		printf("\033[0m");
		fflush(stdout);
		//read(STDIN_FILENO, holder, 256);
		fgets(holder, 256, stdin);

		//printf("before: %s\n", holder);

		int i = 0;
		while (i < strlen(holder)){
			if (holder[i] == '\n'){
				holder[i] = '\0';
			}
			i = i + 1;
		}
		//printf("after : %s\n", holder);
		multiexecute(holder);

		if (errno){
			printf("%s\n", strerror(errno));
		}
	}

	/*char test[100] = "   ls       -a    -l";
	//char test[] = "ls";
	//printf("%s\n", format_command(test));

	char ** args = parse_args(format_command(test));

	execute(test);
	*/
	return 0;
}
