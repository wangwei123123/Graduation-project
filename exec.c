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

void ww_exec(char *arg[])
{
    int status;
    pid_t child_pid;
   
    if(strcmp(arg[0], "history") == 0)
        ww_history(arg);
    else if(strcmp(arg[0], "exit") == 0)
        exit(0);
    else if(strcmp(arg[0], "adduser") == 0)
        ww_adduser(arg);
    else if(strcmp(arg[0], "addgroup") == 0)
        ww_addgroup(arg);
    else if(strcmp(arg[0], "deluser") == 0)
        ww_deluser(arg);
    else if(strcmp(arg[0], "delgroup") == 0)
        ww_delgroup(arg);
    else if(strcmp(arg[0], "cd") == 0)
        ww_cd(arg);
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
