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

/**************************************************************
 *
 * Function name : sig_handle
 * Description : Signal processing function.
 *               Define hot-key by signal.
 * Parameter :
 *              @sig    Signal code.
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void sig_handle(int sig)
{
    /* Ctrl-C */
    if(sig == SIGINT)
    {
        ww_exit();
    }
}

/**************************************************************
 *
 * Function name : init_signal
 * Description : Init signal capture processing.
 * Parameter : NULL
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void init_signal()
{
    /* Ctrl-C  */
    signal(SIGINT, sig_handle);
}

/**************************************************************
 *
 * Function name : init_history_command
 * Description : When the program starts running, the history
 *               commands stored on the hard disk are read into
 *               the trie structure in the memory.
 * Parameter : NULL
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
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

/**************************************************************
 *
 * Function name : init_env_var
 * Description : Initialize environment variable structure.
 * Parameter : NULL
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void init_env_var()
{
    memset(&var_tree, 0, sizeof(Tree_var));
}

/**************************************************************
 *
 * Function name : init
 * Description : Initialization operation when the program
 *               starts running.
 * Parameter : NULL
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void init()
{
    init_signal();
    init_history_command();
    init_env_var();
}

/**************************************************************
 *
 * Function name : main
 * Description : The main function of the program, which is
 *               also the main loop of the program.
 * Parameter : NULL
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
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
