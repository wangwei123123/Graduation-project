/*
 * ============================================================
 *       Filename:  prompt.c
 *    Description:  Generate a string prompt in 
 *                  "[MyShell]username@hostname:pwd￥" format
 *                  and store it in the prompt. At the same 
 *                  time, the host name, current user 
 *                  information, and current path are stored 
 *                  in the structure current_user_info for use.
 *        Version:  1.0
 *        Created:  2018.11.16
 *         Author:  ww
 *        Purpose:  Graduation project
 * ============================================================
 */
#include "ww_shell.h"
//#define DEBUG

extern struct login_user_info login_user;

void ww_prompt(char *prompt)
{
    char cwd[MAX_CWD];
    
    if(!getcwd(cwd, sizeof(cwd)))
    {
        perror("getcwd");
    }
    #ifdef DEBUG
    printf("full_pwd:%s\n", cwd);
    #endif
    
    if(gethostname(current_user_info.hostname, sizeof(current_user_info.hostname)))
    {
        perror("gethostname");
    }
    #ifdef DEBUG
    printf("hostname:%s\n", current_user_info.hostname);
    #endif
    
    if (!(current_user_info.pwd = getpwuid(getuid())))
    {
        perror("getpwuid");
    }
    #ifdef DEBUG
    printf("username:%s\nhome_dir:%s\n", current_user_info.pwd->pw_name, current_user_info.pwd->pw_dir);
    #endif
    
    current_user_info.cwd = cwd;
    if(strstr(cwd, current_user_info.pwd->pw_dir))
    {
        current_user_info.cwd = &cwd[strlen(current_user_info.pwd->pw_dir)-1];
        cwd[strlen(current_user_info.pwd->pw_dir)-1]='~';
    }
    #ifdef DEBUG
    printf("fin_cwd:%s\n", current_user_info.cwd);
    #endif
    //printf("\033[1m[MyShell]\033[m\033[1;32m%s@%s\033[m:\033[1;36m%s\033[m￥\n", current_user_info.pwd->pw_name, current_user_info.hostname, current_user_info.cwd);
    
   // sprintf(prompt, "\001\033[1m\002[MyShell]\001\033[m\002\001\033[1;32m\002%s@%s\001\033[m\002:\001\033[1;36m\002%s\001\033[m\002$ ", current_user_info.pwd->pw_name, current_user_info.hostname, current_user_info.cwd);
    sprintf(prompt, "\001\033[1m\002[MyShell]\001\033[m\002\001\033[1;32m\002%s@%s\001\033[m\002:\001\033[1;36m\002%s\001\033[m\002$ ", login_user.name, current_user_info.hostname, current_user_info.cwd);
}

#ifdef DEBUG
void main()
{
    while(1)
    {
        free(prompt);
        ww_prompt();
        printf("%s\n", prompt);
    }
}
#endif
