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

void ww_history(char *arg[])
{
    char str[MAX_CMD_LEN];
    FILE *fp;
    int i=0;
    
    if(arg[1] && (strcmp(arg[1], "clear") == 0))
        fp = fopen(COMMAND_HISTORY, "w+");
    else
    {
        fp = fopen(COMMAND_HISTORY, "r");
        while(!feof(fp))
        {
            fgets(str, MAX_CMD_LEN, fp);
            printf("%d\t%s", ++i, str);
        }
    }
    fclose(fp);
}

void ww_exec(char *arg[])
{
    int status;
    pid_t child_pid;
   
    if(strcmp(arg[0], "history") == 0)
        ww_history(arg);
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