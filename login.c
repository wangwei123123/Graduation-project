/*
 * ============================================================
 *       Filename:  login.c
 *    Description:  user login
 *        Version:  1.0
 *        Created:  2018.11.28
 *         Author:  ww
 *        Purpose:  Graduation project
 * ============================================================
 */
#include "ww_shell.h"
#define DEBUG
/* Used to control whether the input echo is enabled 
 * option is 1 is enable echo,otherwise is disable.
 * */
int set_disploy_mode(int fd, int option)
{
    struct termios termios_p;
    int err;

    if(tcgetattr(fd, &termios_p) == -1)
    {
        perror("get the attribution of the terminal");
        return 1;
    }
    
    if(option)
        termios_p.c_lflag |= ECHO_FLAGS;
    else
        termios_p.c_lflag &= ~ECHO_FLAGS;
    
    err = tcsetattr(fd, TCSAFLUSH, &termios_p);
    if((err == -1) && (err == EINTR)) 
    {
        perror("set the attribution of the terminal");
        return 1;
    }
    return 0;
}

void encryption(char *info)
{
    while(*info)
    {
        *info ^= MAGICAL;
        info++;
    }
}

void get_username(char *name, int size)
{
    int c, n;

    printf("Input the username:");
    for(n=0;n<size;n++)
    {
        c = getchar();
        if(c!='\n')
            name[n] = c;
        else
        {
            name[n] = '\0';
            break;
        }
    }
}

void get_passwd(char *passwd, int size)
{
    int c, n;

    printf("Input the passwd:");
    for(n=0;n<size;n++)
    {
        c = getchar();
        if(c!='\n')
            passwd[n] = c;
        else
        {
            passwd[n] = '\0';
            break;
        }
    }
}


void get_groupname(char *name, int size)
{
    int c, n;

    printf("Input the group name:");
    for(n=0;n<size;n++)
    {
        c = getchar();
        if(c!='\n')
            name[n] = c;
        else
        {
            name[n] = '\0';
            break;
        }
    }
}


void get_gid(char *gid, int size)
{
    int c, n;

    printf("Input the gid:");
    for(n=0;n<size;n++)
    {
        c = getchar();
        if(c!='\n')
            gid[n] = c;
        else
        {
            gid[n] = '\0';
            break;
        }
    }
}

/* check if the input user information is correct
 * user exist       1.passwd is correct return 0.
 *                  2.passwd is wrong return -1
 * user isn't exist 3.return 1            
 * */
int check_user(struct login_user_info user)
{
    FILE *fp;
    char str[MAX_PASSWD_ITEM];
    char passwd[MAX_USER_PASSWD];
    int i, name_len;

    fp = fopen(PASSWD_PATH, "r");
    name_len = strlen(user.name);
    while(!feof(fp))
    {
        fgets(str, MAX_PASSWD_ITEM, fp);
        if(strncmp(str, user.name, name_len) == 0)
        {
            
            for(i=0;str[name_len+1+i]!=';';i++)
            {
                passwd[i] = str[name_len+1+i];
            }
            passwd[i] = '\0';
            encryption(passwd);
            if(strcmp(passwd, user.passwd) == 0)
                return 0;
            else
                return -1;
        }
    }
    return 1;
}

int login()
{
    struct login_user_info login_user;
    
    get_username(login_user.name, MAX_USER_NAME);
    set_disploy_mode(STDIN_FILENO, 0); 
    get_passwd(login_user.passwd, MAX_USER_PASSWD);
    set_disploy_mode(STDIN_FILENO, 1); 
    printf("\n");

    switch(check_user(login_user))
    {
        case 0 : 
            printf("\033[32mlogin successfully!\033[0m\n");
            return 0;
        case -1 :
            printf("\033[31mpasswd is wrong!\033[0m\n");
            return 1;
        case 1 :
            printf("\033[31muser isn't exist!\033[0m\n");
            return 1;
    }
}

#ifdef DEGUB
int main()
{
    int login_times = 0;
    while(login())
    {
        if(++login_times == MAX_LOGIN_TIMES)
        {
            printf("\033[31mlogin times out!\033[0m\n");
            return 0;
        }
    }
}
#endif
