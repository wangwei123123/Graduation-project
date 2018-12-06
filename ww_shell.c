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

extern Tree root;
extern Com_list commands;
extern Com_list *rear;
extern int command_num;
extern int history_num;
Tree_var var_tree;

/* signal processing function */
void sig_handle(int sig)
{
    /* Ctrl-C */
    if(sig == SIGINT)
    {
        ww_exit();
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

void init_history_command()
{
    FILE *fp;
    char history_command[MAX_CMD_LEN];  

    memset(&root, 0, sizeof(Tree));
    memset(&commands, 0, sizeof(Com_list));
    commands.next = NULL;
    rear = &commands;

    /* check if .history exists */
    if(access(COMMAND_HISTORY, F_OK) == -1)
    {
        fp = fopen(COMMAND_HISTORY, "w+");
        fclose(fp);
    }
    else
    {
        fp = fopen(COMMAND_HISTORY, "r");
        fgets(history_command, MAX_CMD_LEN, fp);

        while(!feof(fp))
        {
            history_num++;
            insert_tree(history_command);
            fgets(history_command, MAX_CMD_LEN, fp);
        }
        fclose(fp);
    }
}

/* Initialize environment variable structure */
void init_env_var()
{
    memset(&var_tree, 0, sizeof(Tree_var));
}

void init()
{
    init_signal();
    init_history_command();
    init_env_var();
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
    
    init();

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
