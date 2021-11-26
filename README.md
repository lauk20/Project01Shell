# Project01Shell

## Features
#### Single Command Execution

*Example:* ```ls -a -l``` will run ```ls -a -l```


#### Multiple Command Execution

*Example:* ```ls -a -l ; echo hello``` will run ```ls -a -l``` and then run ```echo hello```


#### Simple Redirection

*Example:* ```ls -a -l > file.txt``` will write the output into ```file.txt```

*Example:* ```cat < file.txt``` will run ```cat``` from the data it reads from file1.txt


#### Multiple Redirections

*Example:* ```echo hello > file1.txt > file2.txt > file3.txt``` will create the files if they do not exist, but will only write to the last file (file3.txt). If any of the files that come before file3.txt exist, then they will be wiped (O_TRUNC).

*Example:* ```cat < file1.txt < file2.txt < file3.txt``` will make ```cat``` read from the last given file. If any of the files do not exist, then the command will not run and output the error, "No such file or directory".


#### Simple Pipes

*Example:* ```ls | wc``` will pipe the output from ```ls``` into ```wc```. ```wc``` will use the output as its input.


#### Multiple Pipes

*Example:* ```ls | grep READ | wc -l``` will run ```ls``` and use that output as input for ```grep READ```, which will find any text that has "READ" and that result will be input for ```wc -l```. In this case, the output would be the number of files that have "READ" in their name, such as "README.md".


#### Extra Spaces Do Not Matter

*Example:* ```   cat>file1.txt >file2.txt>   file3.txt   >   file4.txt>  file5.txt   ``` will behave exactly the same as ```cat > file1.txt > file2.txt > file3.txt > file4.txt > file5.txt```

*Example:* ``` ls       -a         -l     ``` is the same as ```ls -a -l```


#### Combining Redirection and Pipes

*Example:* ```cat < file.txt | cat > samecontent.txt``` will ```cat file.txt``` and then pipe the result into ```cat```, which will output the result into ```samecontent.txt```

## Attempted Features

## Bugs/Limitations
- There is a limit of how long your command is; that limit is 254. (This is just a limit I put, it can always be changed if needed). If you go over this limit, expect weird behavior.
- Tabbing will cause your command line input to be different from what you see in the terminal. Tabbing does not auto-complete
- The arrow keys do not move around in the shell.
- Ending a command with a pipe does not work like BASH.
  *Example:* ```cat file.txt | grep . |``` will output "No such file or directory". In BASH, it prompts for input.
- Quotations do not work like BASH.
  *Example:* ```echo "hello world"``` and ```echo 'hello world'``` will output ```"hello world"``` and ```'hello world'```
- Ctrl + D will send the program into an infinite "No such file or directory" loop. If you use it on the command line. (Or whatever shortcut you have for ```^D```)

## Function Headers
```c
/*
	Args:
		char * line: string to be cleaned
	Function:
		Cleans up char * line so that repetitive spaces are removed.
		Spaces are added, if needed, before and after the redirection and piping characters.
	Returns:
		a cleaned up string that should only have spaces where neccessary.
*/
char * format_command(char * line)
```
```c
/*
	Args:
		char * line: string to be parsed into a char ** using spaces as separators
	Function:
		Assumes that the given char * line was cleaned using format_command.
		Parses string and puts argument into a char ** that is execvp-ready.
	Returns:
		char ** that is ready to be execvp'ed.
*/
char * parse_args(char * line, char ** args)
```
```c
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
char * mystrsep(char ** string, char * delim, char * delim2)
```
```c
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
int redirect_file(int this, int withThis)
```
```c
/*
	Args:
		char * command: raw user inputted command that user wants to run
	Function:
		Executes the command that the user wants to run using helper functions that parse the args
		Redirection and piping are handled in this function along with the execution of commands.
	Returns:
		int: WEXITSTATUS - exit status of the child processes
*/
int execute(char * command)
```
```c
/*
	Args:
		char * command: raw user inputted command that user wants to run
	Function:
		Executes the command that the user wants to run using execute function.
		Can handle multiple commands separated by the ";"
	Returns:
		int: 0
*/
int multiexecute(char * command)
```
```c
/*
	Args:
	Function:
		Begins shell program, loops the shell program
	Returns:
		0;
*/
int main()
```
```c
/*
  Args:
    char * path: Path you wish to cd into
  Function:
    Changes directory to path
  Returns:
    returns success or error from chdir(path)
*/
int cd(char * path)
```
```c
/*
  Args:
  Function:
    calls exit(0);
  Returns:
    void
*/
void cexit()
```
