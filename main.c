#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

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
		//printf("%ld NEWCOMMAND: %s\n", strlen(newCommand), newCommand);
		if (line[i] == ' ' || line[i] == '\t'){
				lastSpace = line + i;
		} else {
			//printf("%d\n", line[i]);
			if (lastSpace && start){
				strcat(newCommand, " ");
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
	//printf("eNEWCOMMAND: %s\n", newCommand);
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

char * mystrsep(char ** string, char * delim, char * delim2){
	int i = 0;

	char * token = *string;

	//printf("%d %s\n", strlen(*string), *string);
	while (i < strlen(*string)){
		//printf("o%d %c\n", i, *(*string + i));
		if (*(*string + i) == *delim | *(*string + i) == *delim2){
			//printf("WEWEWE%s\n", *string);
			*(*string + i) = '\0';
			*string = *string + i + 1;

			return token;
		}

		i = i + 1;
	}
	//printf("end\n");
	*string = NULL;
	printf("mystrsep%s\n", token);
	return token;
}

int redirect_file(int this, int withThis){
	int newfd = dup2(this, withThis);

	return newfd;
}

/*
	Args:
		char * command: raw user inputted command that user wants to run
	Function:
		Executes the command that the user wants to run using helper functions that parse the args
	Returns:
		int: WEXITSTATUS
*/
/*int execute(char * command){
	//printf("raw cmd %ld: %s\n", strlen(command), command);
	char * formattedCommand = format_command(command);
	char ** args = parse_args(formattedCommand);

	if (strcmp(args[0], "exit") == 0){
		cexit();
		return 0;
	}

	//printf("%ld is len: %s\n", strlen(formattedCommand), formattedCommand);
	//printf("%s %s %s\n", args[0], args[1], args[2]);

	char * token = mystrsep(&formattedCommand, ">", "<");
	//printf("TOKENheader: %s\n", token);
	//printf("oooo\n");
	while(token){
		//printf("ENTEREWHILE\n");
		char * separatedCommand = calloc(strlen(token), 1);
		//printf("HALLLLO123123O\n");
		//printf("%s FOOOOO\n", formattedCommand);
		if (formattedCommand - token > 0){
			strncpy(separatedCommand, token, formattedCommand - token - 1);
		} else {
			strcpy(separatedCommand, token);
		}
		//printf("HALLLLOO\n");
		char * cleaned = format_command(separatedCommand);
		args = parse_args(cleaned);
		//printf("TOKEN: %s\n", separatedCommand);
		//printf("ARGS: %s %s\n", args[1], args[2]);
		//printf("BEFORE FORK\n");
		int subprocess = fork();

		//child process
		if (subprocess == 0){
			if (strcmp(args[0], "cd") == 0){
				//printf("%s\n", args[1]);
				args[1] = strsep(args + 1, "\t");
				return cd(args[1]);
			} else {
				//printf("ENTERED\n");
				//printf("FORMATTED CMD: %c\n", *(formattedCommand - 1));
				int duped = -1;
				if (formattedCommand - 1 && *(formattedCommand-1) == '>'){
					duped = dup(STDOUT_FILENO);
					//printf("FC\n");
					token = format_command(mystrsep(&formattedCommand, ">", "<"));
					printf("TOKEN:%s\n", token);
					printf("FORMED:%s\n", formattedCommand);
					char * filename = calloc(strlen(token), 1);
					//printf("CLEAN: %s\n", cleanedCommand);
					printf("EEEEK%ld", strlen(token) - strlen(formattedCommand));
					if (strlen(token) - strlen(formattedCommand) > 0){
						strncpy(filename, token, strlen(token) - strlen(formattedCommand) - 1);
					} else {
						strcpy(filename, token);
					}
					char * fixedfilename = format_command(filename);
					int replace = open(fixedfilename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
					printf("FILENAME: %s\n", filename);

					redirect_file(replace, STDOUT_FILENO);

					free(filename);
					free(fixedfilename);
				}

				int status = execvp(args[0], args);
				if (errno){
					if (duped > -1){
						redirect_file(duped, STDOUT_FILENO);
					}
					printf("%s\n", strerror(errno));
				}

				int i = 0;
				for (i = 0; i < (sizeof(args)/8) - 1; i++){
					if (args[i]){
						free(args[i]);
					}
				}
				free(separatedCommand);
				exit(status);
			}
		} else {
			int status = 0;
			int waitStatus = waitpid(subprocess, &status, 0);
			//printf("CHILD PROCESS HAS COMPLETED\n");
			int i = 0;
			for (i = 0; i < (sizeof(args)/8) - 1; i++){
				if (args[i]){
					free(args[i]);
				}
			}

			free(separatedCommand);

			return WEXITSTATUS(status);
		}

		token = mystrsep(&formattedCommand, ">", "<");
	}

	free(formattedCommand);
}*/

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
			int counter = 0;
			while (args[counter]){
				counter = counter + 1;
			}
			//printf("wherea am i %d\n", counter);
			int x = 0;
			//printf("%s %s %s!\n", args[0], args[1], args[2]);
			//printf("%ld\n", sizeof(*args));
			for (x = 0; x < counter; x++){
				//printf("aa%s\n", args[x]);
				if (strcmp(args[x], ">") == 0 && args[x] != NULL){
					char * filename = args[x + 1];
					int replace = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
					//printf("FILENAME: %s 0: %s, 1: %s\n", filename, args[x], args[x + 1]);

					redirect_file(replace, STDOUT_FILENO);

					args[x] = 0;
				} else if (strcmp(args[x], "<") == 0 && args[x] != NULL){
					char * filename = args[x + 1];
					int replace = open(filename, O_RDONLY, 0664);
					//printf("FILENAME: %s\n", filename);

					redirect_file(replace, STDIN_FILENO);

					args[x] = 0;
				}
			}
			//printf("aaa\n");
			/*char * tokenParse = calloc(strlen(formattedCommand), 1);
			strcpy(tokenParse, formattedCommand);
			char * token = mystrsep(&tokenParse, ">", "<");
			printf("TOKEN1%s\n", token);
			if (token){
				printf("TOKENPARSE%s\n", tokenParse);
				token = mystrsep(&tokenParse, ">", "<");
			}
			printf("TOKEN%s\n", token);
			printf("TOKENPARSE%s\n", tokenParse);
			while (token){
				printf("TOKEN%s\n", token);
				token = mystrsep(&tokenParse, ">", "<");
			}
			printf("HELLO\n");
			printf("0: %s 1: %s 2: %s\n", args[0], args[1], args[2]);
			*/
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
		char wd[256];
		printf("\033[0;31m");
		//printf("Shell $: ");
		printf("%s\n$ ", getcwd(wd, sizeof(wd)));
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

		free(holder);
	}

	/*char test[100] = "   ls       -a    -l";
	//char test[] = "ls";
	//printf("%s\n", format_command(test));

	char ** args = parse_args(format_command(test));

	execute(test);
	*/
	return 0;
}
