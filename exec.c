/*
 * ============================================================
 *       Filename:  exec.c
 *    Description:  run command  
 *        Version:  1.0
 *        Created:  2018.11.16
 *         Author:  ww
 *        Purpose:  Graduation project
 * ============================================================
 */
#include "ww_shell.h"
//#define DEBUG

/**************************************************************
 *
 * Function name : ww_exec
 * Description : Distinguish between built-in commands and
 *               external commands, and execute.
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : Arg[0] is the commands,and the others is command
 *         parameter.
 * ***********************************************************/
void ww_exec(char *arg[])
{
    int status;
    pid_t child_pid;
   
    if(strcmp(arg[0], "exit") == 0)
        ww_exit();
    else if(strcmp(arg[0], "help") == 0)
        ww_help();
    else if(strcmp(arg[0], "history") == 0)
        ww_history(arg);
    else if(strcmp(arg[0], "adduser") == 0)
        ww_adduser(arg);
    else if(strcmp(arg[0], "addgroup") == 0)
        ww_addgroup(arg);
    else if(strcmp(arg[0], "deluser") == 0)
        ww_deluser(arg);
    else if(strcmp(arg[0], "delgroup") == 0)
        ww_delgroup(arg);
    else if(strcmp(arg[0], "su") == 0)
        ww_su(arg);
    else if(strcmp(arg[0], "cd") == 0)
        ww_cd(arg);
    else if(strcmp(arg[0], "set") == 0)
        ww_set(arg);
    else if(strcmp(arg[0], "unset") == 0)
        ww_unset(arg);
    else if(strcmp(arg[0], "echo") == 0)
        ww_echo(arg);
    else if(strcmp(arg[0], "let") == 0)
        ww_let(arg);
    else if(strcmp(arg[0], "which") == 0)
        ww_which(arg);
    else if(strcmp(arg[0], "type") == 0)
        ww_type(arg);
    else if(strcmp(arg[0], "test") == 0)
        ww_test(arg);
    else
    {
        /* fork off child process */
        if((child_pid=fork())!=0) {
            /* Parent code */
        
            /* wait for child to exit */
            waitpid(child_pid,&status,0);
	    } else {
		    /* Child code */

		    /* execute command */
		    execvp(arg[0], arg);
//		    printf("%s,%s\n",arg[0],arg[1]);
            exit(0);
	    }
    }
}

#ifdef DEBUG
void main()
{
    char *arg[]={"ls","-l",0};
    ww_exec(arg);
}
#endif
