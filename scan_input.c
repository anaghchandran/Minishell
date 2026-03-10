/*
name		:Anagh cs
date		:
description	:
sample input	:
sample output	:

--algorithm--

 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "header.h"

void own_handler(int signum);
char *get_command(char *input);
int check_command(char *cmd);
int printing_stack();
int signal_stop(char *last_cmd);
int pipe_handler(char* input);
char last_cmd[30];
static int flag = 0;
pid_t child_pid,pid;

list *head = NULL;

void scan_input(char *input, char *prompt) 
{

    //signal( SIGINT , SIG_IGN);
    while(1) 
    {

	//signal( SIGINT , SIG_IGN);
	//signal( SIGINT,own_handler);
	printf("%s :", prompt);
	int r = scanf("%[^\n]", input);
	if( r == -1 )
	{
	    continue;  // Clear newline from input buffer
	}
	else if( r == 0)
	{
	    getchar();
	    continue;
	}
	else
	    getchar();
	if (!strncmp(input, "PS1=", 4)) 
	{
	    if (input[4] != ' ') 
	    {
		strcpy(prompt, &input[4]);
	    } else 
	    {
		printf("command not found: - from ps1\n");
	    }
	}

	if (strchr(input, '|') != NULL) 
	{
	    printf("pipe\n");
	    pipe_handler(input);
	    continue;
	}
	char *cmd = get_command(input);
	printf("cmd -> %s\n", cmd);
	int res = check_command(cmd);
	strcpy(last_cmd, input);  // Store the entire input
	if (res == 0) 
	{
	    if (strncmp(input, "echo $", 6) == 0) 
	    {
		if (strcmp(input, "echo $$") == 0) 
		{
		    printf("%d\n", getpid());
		} 
		else if (strcmp(input, "echo $?") == 0) 
		{
		    printf("0\n");
		} 
		else 
		{
		    printf("Variable: %s\n", &input[6]);
		}
	    } 
	    else if (strcmp(input, "jobs") == 0) 
	    {
		printf("printing stack\n");
		printing_stack();
	    }
	    else if( strcmp(input ,"fg") == 0)
	    {
		printf("foreground\n");
	    }
	    else if( strcmp( input,"bg")== 0)
	    {
		printf("bg\n");
	    }
	    else 
	    {
		printf("cmd not found\n");
	    }
	} 
	else if (res == 1) 
	{
	    if (strcmp("exit", input) == 0) 
	    {
		printf("exit\n");

		signal(SIGTERM,own_handler);
		raise(SIGTERM);
	    } 
	    else if (strcmp("cd", input) == 0)
	    {
		printf("cd\n");
	    } 
	    else if (strcmp("pwd", input) == 0) 
	    {
		printf("pwd\n");
	    }
	} 
	else if (res == 2) 
	{
	    pid = fork();
	    if (pid == 0) 
	    {

		//signal( SIGINT , SIG_IGN);
		char *buff[20];
		int i = 0;
		char *token = strtok(input, " ");
		while (token != NULL && i < 19) 
		{
		    buff[i++] = token;
		    token = strtok(NULL, " ");
		}
		buff[i] = NULL;
		execvp(buff[0], buff);
		perror("execvp");  // If execvp fails
		exit(EXIT_FAILURE);
	    } 
	    else if (pid > 0)  // In parent process
	    {
		child_pid = pid;  // Set global child PID for signal handling
		int status;
		pid_t ret = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);  // Allow handling stopped processes

		// Reset child PID once process is stopped or completed
		if (WIFSTOPPED(status)) {
		    printf("Child %d stopped.\n", ret);
		    child_pid = -1;  // Clear child_pid to keep shell running
		} 
		else if (WIFEXITED(status) || WIFSIGNALED(status)) {
		    printf("Child %d terminated with exit code %d\n", ret, WEXITSTATUS(status));
		    child_pid = -1;  // Clear child_pid on termination
		}
	    }

	    else 
	    {
		perror("fork");
	    }
	}
	memset(input, 0, 25);  // Clear input buffer
    }
}

char *get_command(char *input) 
{
    char *str = malloc(25 * sizeof(char));
    for (int i = 0; i < strlen(input); i++) 
    {
	if (input[i] != ' ' && input[i] != '\0') 
	{
	    str[i] = input[i];
	} else if (input[i] == ' ') 
	{
	    str[i] = '\0';
	    break;
	}
    }
    return str;
}

int check_command(char *cmd) 
{
    char *builtins[] = 
    {
	"printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
	"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
	"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL
    };

    for (int y = 0; builtins[y] != NULL; y++) 
    {
	if (strcmp(builtins[y], cmd) == 0) 
	{
	    return BUILTIN;
	}
    }

    char buffer[50];
    int fd = open("external.txt", O_RDONLY);
    if (fd == -1) 
    {
	perror("open");
	return FAILURE;
    }

    int i = 0;
    char ch;
    while (read(fd, &ch, 1) == 1) 
    {
	if (ch == '\n' || ch == '\0') 
	{
	    buffer[i] = '\0';
	    if (strcmp(cmd, buffer) == 0) 
	    {
		close(fd);
		return EXTERNAL;
	    }
	    i = 0;
	} 
	else 
	{
	    buffer[i++] = ch;
	}
    }
    close(fd);
    return FAILURE;
}



void own_handler(int signum) 
{
    if (signum == SIGINT) {
	if (child_pid > 0) {
	    kill(child_pid, SIGKILL);  // Terminate only the child process on Ctrl+C
	    child_pid = -1;  // Reset child_pid after killing
	}
    } 
    else if (signum == SIGTSTP) {
	if (child_pid > 0) {  // Stop only if a child is running
	    printf("Process %d stopped, adding to background list\n", child_pid);
	    kill(child_pid, SIGTSTP);  // Stop the child process
	    signal_stop(last_cmd);      // Add command to background list
	    child_pid = -1;             // Reset child_pid so shell can continue
	}
    } 
    else if (signum == SIGTERM) {
	printf("TERMINATING FROM MINSHELL\n");
	sleep(1);
	printf("--TERMINATED--\n");
	exit(0);
    }
}


int signal_stop(char *last_cmd) 
{
    // printf("adding");
    list *new_node = malloc(sizeof(list));
    if (new_node == NULL) 
    {
	perror("malloc");
	return 0;
    }

    strcpy(new_node->process, last_cmd);
    new_node ->pid = child_pid;
    new_node->next = NULL;

    if (head == NULL) 
    {
	head = new_node;
	return 0;
    } 


    list *temp = head;
    while (temp->next != NULL) 
    {
	temp = temp->next;
    }
    temp->next = new_node;
    return 0;


}

int printing_stack() 
{
    if (head == NULL) 
    {
	printf("--NO BACKGROUND PROCESS--\n");
	return 0;
    }

    printf("RUNNING PROCESSES:\n");
    list *temp = head;
    while (temp != NULL) 
    {
	printf("-> %s---->", temp->process);
	printf("-> %d \n", temp->pid);
	temp = temp->next;
    }
    return 1;
}
int pipe_handler(char *input) 
{
    char str[100];  // Adjust size as needed
    strcpy(str, input);  // Copy input to str

    int maxCommands = 10;
    char *commands[maxCommands];
    int cmd_count = 0;

    // Split input string by '|', storing each command in commands[]
    char *token = strtok(str, "|");
    while (token != NULL && cmd_count < maxCommands) 
    {
        commands[cmd_count++] = token;
        token = strtok(NULL, "|");
    }

    int in_fd = 0;  // Input file descriptor for pipes

    for (int i = 0; i < cmd_count; i++) 
    {
        int pipe_fd[2];
        pipe(pipe_fd);

        if (fork() == 0) 
        {
            // Child process
            dup2(in_fd, 0);  // Set up input from previous pipe (or stdin)

            if (i < cmd_count - 1) 
            {
                dup2(pipe_fd[1], 1);  // Redirect output to the pipe for the next command
            }

            close(pipe_fd[0]);

            // Parse the command into arguments
            char *args[maxCommands];
            int argIndex = 0;
            char *arg = strtok(commands[i], " ");
            while (arg != NULL && argIndex < maxCommands - 1) 
            {
                args[argIndex++] = arg;
                arg = strtok(NULL, " ");
            }
            args[argIndex] = NULL;

            execvp(args[0], args);  // Execute command with arguments
            perror("execvp failed");  // Error if execvp fails
            exit(1);
        } 
        else 
        {
            // Parent process
            wait(NULL);         // Wait for the child to finish
            close(pipe_fd[1]);  // Close the write end of the current pipe
            in_fd = pipe_fd[0]; // Keep read end for the next command
        }
    }

    return 0;
}

