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
		Spaces are added, if needed, before and after the redirection and piping characters.
	Returns:
		a cleaned up string that should only have spaces where neccessary.
*/
char * format_command(char * line){
	char * newCommand = calloc(strlen(line) + 1, 1);

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

			if (line[i] == '>' || line [i] == '<' || line[i] == '|'){
				if (i > 1 && line[i - 1] != ' ' && line[i - 1] != '>'){
					strcat(newCommand, " ");
				}

				strncat(newCommand, line + i, 1);

				if (i < strlen(line) - 1 && line[i + 1] != ' ' && line[i + 1] != '>'){
					strcat(newCommand, " ");
				}
			} else {
				strncat(newCommand, line + i, 1);
			}
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
char * parse_args(char * line, char ** args){
	char * copied = calloc(strlen(line) + 1, 1);

	strcpy(copied, line);

	int argCount = 0;
	while (strsep(&copied, " ")){
		argCount = argCount + 1;
	}

	//printf("%d\n", argCount);

	//args = calloc((argCount + 1) + 1, 8);

	char * permLine = calloc(strlen(line) + 1, 1);
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

	//printf("parg %s\n", line);

	return permLine;
}

/*
	THIS FUNCTION ENDED UP NEVER BEING USED SO IT MAY NOT WORK AS INTENDED
		BECAUSE IT WAS BARELY TESTED.
	Args:
		char * string: string to find tokens in
		char * delim: first delimiter
		char * delim2: second delimiter
	Function:
		Like the strsep function except it can take in two delimiters and use both to find tokens.
	Returns:
		char *: the token,
*/
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

/*
	This function is useless, it just calls dup2 and returns what dup2 returns.
	It used to do something but it has since been changed.
	It is still in use, so removing it would require editing other parts of the program.
	So, it stays.
	Args:
		int this: File Descriptor of new file that will replace withThis.
		int withThis: File Descriptor of the file to be replaced
	Function:
		dup2(this, withThis)
	Returns:
		returns dupe2(this, withThis).
*/
int redirect_file(int this, int withThis){
	int newfd = dup2(this, withThis);

	return newfd;
}

/*
	Args:
		char * command: raw user inputted command that user wants to run
	Function:
		Executes the command that the user wants to run using helper functions that parse the args
		Redirection and piping are handled in this function along with the execution of commands.
	Returns:
		int: WEXITSTATUS - exit status of the child processes
*/
int execute(char * command){
	//printf("raw cmd %ld: %s\n", strlen(command), command);
	char * formattedCommand = format_command(command);
	//printf("OGBP: %s\n", formattedCommand);
	char * copied = calloc(strlen(formattedCommand) + 1, 1);

	strcpy(copied, formattedCommand);

	int argCount = 0;
	while (strsep(&copied, " ")){
		argCount = argCount + 1;
	}

	//printf("%d\n", argCount);

	char ** args = calloc((argCount + 1) + 1, 8);
	char * permline = parse_args(formattedCommand, args);

	free(copied);
	//printf("raw: %s\n", command);
	//printf("OG: %s\n", formattedCommand);

	if (strcmp(args[0], "exit") == 0){
		cexit();
		return 0;
	} else if (strcmp(args[0], "cd") == 0){
		//printf("%s\n", args[1]);
		args[1] = strsep(args + 1, "\t");
		return (cd(args[1]));
	}
	//printf("%ld is len: %s\n", strlen(formattedCommand), formattedCommand);
	//printf("%s %s %s\n", args[0], args[1], args[2]);
	int subprocess = fork();
	//child process
	if (subprocess == 0){
		if (0){ // always false clean up later if remember and if time, but not important
			//printf("%s\n", args[1]);
			args[1] = strsep(args + 1, "\t");
			exit(cd(args[1]));
		} else {
			int duped = -1;
			int replaced = -1;

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

					duped = dup(STDOUT_FILENO);
					replaced = STDOUT_FILENO;
					redirect_file(replace, STDOUT_FILENO);

					args[x] = 0;
				} else if (strcmp(args[x], ">>") == 0 && args[x] != NULL){
					char * filename = args[x + 1];
					int replace = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
					//printf("FILENAME: %s 0: %s, 1: %s\n", filename, args[x], args[x + 1]);

					duped = dup(STDOUT_FILENO);
					replaced = STDOUT_FILENO;
					redirect_file(replace, STDOUT_FILENO);

					args[x] = 0;
				} else if (strcmp(args[x], "<") == 0 && args[x] != NULL){
					char * filename = args[x + 1];
					int replace = open(filename, O_RDONLY, 0664);
					//printf("FILENAME: %s\n", filename);
					if (replace == -1){
						if (errno){
							printf("%s\n", strerror(errno));
						}
						free(formattedCommand);
						free(args);
						free(permline);

						exit(replace);
					}
					duped = dup(STDIN_FILENO);
					replaced = STDIN_FILENO;
					redirect_file(replace, STDIN_FILENO);

					args[x] = 0;
				} else if (strcmp(args[x], "|") == 0 && args[x] != NULL){
					char cmd[256];
					char cmd2[256];

					int iter = 0;
					strncpy(cmd, "", 256);
					strncpy(cmd2, "", 256);

					while (iter < x){
						//printf("%d %s\n", iter, args[iter]);
						if (args[iter] != NULL){
							strcat(cmd, args[iter]);
							strcat(cmd, " ");
						}
						iter = iter + 1;
					}

					iter = iter + 1;
					while (args[iter]){//} && *args[iter] != '>' && *args[iter] != '<' && *args[iter] != '|'){
						strcat(cmd2, args[iter]);
						strcat(cmd2, " ");
						//printf("ADDED: %s\n", args[iter]);
						iter = iter + 1;
					}

					//printf("CMD:>%s\n", cmd);
					//printf("CMD2:>%s\n", cmd2);

					/*duped = dup(STDOUT_FILENO);
					replaced = STDOUT_FILENO;
					redirect_file(STDIN_FILENO, STDOUT_FILENO);*/
					int pipefd[2];

					if (pipe(pipefd) == -1){
						exit(-1);
					}

					int pid_firstcmd;
					int pid_secondcmd;

					pid_secondcmd = fork();

					if (pid_secondcmd == 0){ //child process
						dup2(pipefd[0], STDIN_FILENO);
						close(pipefd[0]);
						close(pipefd[1]);

						//free(formattedCommand);
						//printf("BFfc %s\n", formattedCommand);
						free(formattedCommand);
						formattedCommand = format_command(cmd2);
						//args = parse_args(formattedCommand);
						//printf("AA0: %s 1: %s 2: %s\n", args[0], args[1], args[2]);
						//execvp(args[0], args);
						//printf("fc: %s\n", formattedCommand);
						//printf("fc %s\n", formattedCommand);
						//printf("cmd2 %s\n", cmd2);
						int stat = execute(formattedCommand);
						//printf("ERROR\n");
						//printf("Freeing in 0\n");
						/*if (formattedCommand){
							formattedCommand = NULL;
							free(formattedCommand);
						}
						if (args[0]){
							args[0] = NULL;
							free(args[0]);
						}
						if (args){
							args = NULL;
							free(args);
						}*/

						free(formattedCommand);
						free(args);
						free(permline);

						exit(stat);
					}

					pid_firstcmd = fork();

					if (pid_firstcmd == 0){ //child process
						dup2(pipefd[1], STDOUT_FILENO);
						close(pipefd[0]);
						close(pipefd[1]);

						//free(formattedCommand);
						//free(args);
						formattedCommand = format_command(cmd);
						char * permline = parse_args(formattedCommand, args);
						//printf("AA0: %s 1: %s 2: %s\n", args[0], args[1], args[2]);
						execvp(args[0], args);

						if (errno){
							printf("%s\n", strerror(errno));
						}
						//printf("Freeing in 1\n");
						/*if (formattedCommand){
							formattedCommand = NULL;
							free(formattedCommand);
						}
						if (args[0]){
							args[0] = NULL;
							free(args[0]);
						}
						if (args){
							args = NULL;
							free(args);
						}*/

						free(formattedCommand);
						free(args);
						free(permline);

						exit(-1);
					}

					close(pipefd[0]);
					close(pipefd[1]);

					int status1 = 0;
					int status2 = 0;

					waitpid(pid_firstcmd, &status1, 0);
					waitpid(pid_secondcmd, &status2, 0);

					args[x] = 0;

					free(formattedCommand);
					free(args);

					exit(0);
				}
			}
			/*printf("ABC\n");
			printf("0: %s 1: %s 2: %s\n", args[0], args[1], args[2]);*/
			//printf("0: %s 1: %s 2: %s\n", args[0], args[1], args[2]);
			int status = execvp(args[0], args);

			if (duped > -1){
				redirect_file(duped, replaced);
			}

			if (errno){
				printf("%s\n", strerror(errno));
			}
			//printf("Freeing in 2\n");
			/*if (formattedCommand){
				formattedCommand = NULL;
				free(formattedCommand);
			}
			if (args[0]){
				args[0] = NULL;
				free(args[0]);
			}
			if (args){
				args = NULL;
				free(args);
			}*/

			free(formattedCommand);
			free(args);
			free(permline);

			exit(status);
		}
	} else {
		int status = 0;
		int waitStatus = waitpid(subprocess, &status, 0);
		//printf("CHILD PROCESS HAS COMPLETED\n");
		//printf("Freeing in 3\n");
		/*if (formattedCommand){
			//printf("fc\n");
			free(formattedCommand);
		}*/
		/*if (args[0]){
			printf("a0 %s\n", args[0]);
			free(args[0]);
		}*/
		/*if (args){
			printf("args\n");
			free(args);
			//args = NULL;
		}*/
		//printf("completed free\n");

		free(formattedCommand);
		free(args);
		free(permline);

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
	char * heapCommand = calloc(strlen(command) + 1, 1);
	strcpy(heapCommand, command);
	//printf("heapCommand: %s\n", heapCommand);
	char * token = strsep(&heapCommand, ";");

	while (token){
		//printf("token: %s\n", token);
		int status = execute(token);
		if (status != 0){
			return status;
		}
		token = strsep(&heapCommand, ";");
	}

	free(heapCommand);

	return 0;
}

/*
	Args:
	Function:
		Begins shell program, loops the shell program
	Returns:
		0;
*/
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
			errno = 0;
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
