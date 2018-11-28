/*
 * ============================================================
 *       Filename:  ww_shell.c
 *    Description:  main loop
 *        Version:  1.0
 *        Created:  2018.11.16
 *         Author:  ww
 *        Purpose:  Graduation project
 * ============================================================
 */
#include "ww_shell.h"
#define TRUE 1

/* signal processing function */
void sig_handle(int sig)
{
    /* Ctrl-C */
    if(sig == SIGINT)
    {
        printf("\nprogram exit\n");
        exit(0);
    }
}


/* define hot-key by signal
 * init signal capture processing
 * */ 
void init_signal()
{
    /* Ctrl-C  */
    signal(SIGINT, sig_handle);
}

void init_other()
{
   /* check if .history exists */
   if(access(COMMAND_HISTORY, F_OK) == -1)
   {
        FILE *fp;
        fp = fopen(COMMAND_HISTORY, "w+");
        fclose(fp);
   }
}

int main()
{
    extern char *s;
    char *prompt = NULL;
    char *arg[MAX_ARG];
    int login_times = 0;
    
    while(login())
    {
        if(++login_times == MAX_LOGIN_TIMES)
        {
            printf("\033[31mlogin times out!\033[0m\n");
            return 0;
        }
    }
    
    init_signal();
    init_other();

    /* main loop , repeat forever */
	while(TRUE) 
	{
        free(prompt);
        prompt = (char*)malloc(MAX_PROMPT_LEN);
        /* display prompt on the screen */
		ww_prompt(prompt);
		/* read input from terminal */
        ww_read_command(prompt);
		/* parse the command  */
        ww_parse(s, arg);
        /* exe the command */
        ww_exec(arg);
    }
}
