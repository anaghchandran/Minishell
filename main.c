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
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
void own_handler( int signum);
void scan_input( char *input_string,char *prompt);
int main()
{
    system("clear");
    char input_string[25];
    char prompt[25] = "minishell$";
    signal(SIGINT,own_handler);
    signal(SIGTSTP,own_handler);
    scan_input( input_string,prompt);

}




