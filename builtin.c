/*
 * ============================================================
 *       Filename:  builtin.c
 *    Description:  builtin command 
 *        Version:  1.0
 *        Created:  2018.11.30
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


void ww_adduser(char *arg[])
{
    FILE *fp;
    int i, j,  g_flag=-1, u_flag=0, p_flag=0,flag=1;
    char name[MAX_USER_NAME];
    char passwd[MAX_USER_PASSWD];
    char str_g[MAX_GROUP_ITEM];
    char str_p[MAX_PASSWD_ITEM];
    char gid[MAX_GROUP];

    printf("password only supports numbers and letters\n");
    get_username(name, MAX_USER_NAME);
   
    fp = fopen(PASSWD_PATH, "r");
    while(!feof(fp))
    {
        i = 0;
        fgets(str_p, MAX_PASSWD_ITEM, fp);
        while(str_p[i]!=';' && name[i]!='\0')
        {
            i++;
            if(str_p[i] != name[i])
                break;
        }
        if(str_p[i]==';' && name[i]=='\0')
        {
            u_flag = -1;
            break;
        }
    }
    fclose(fp);
    if(u_flag != 0)
    {
        printf("\033[31madd user failed : user %s already exist.\033[0m\n", name);
        return;
    }
    
    flag = 1;
    while(flag<4)
    {
        p_flag = 0;
        set_disploy_mode(STDIN_FILENO, 0);
        get_passwd(passwd, MAX_USER_PASSWD);
        set_disploy_mode(STDIN_FILENO, 1);
        printf("\n");
        for(i=0;passwd[i]!='\0';i++)
        {
            if(!((passwd[i]>=48 && passwd[i]<=57) || (passwd[i]>=65 && passwd[i]<=90) \
                || (passwd[i]>=97 && passwd[i]<=122)))
            {
                p_flag = -1;
                printf("%d:\033[31mpassword only supports numbers and letters!!!\033[0m\n", flag);
                flag++;
                break;
            }
        }
        if(p_flag == 0)
            break;
    }
    if(p_flag != 0)
    {
        printf("\033[31madd user failed : wrong password format.\033[0m\n");
        return;
    }
    encryption(passwd);

    j=0;
    get_gid(gid, MAX_GROUP);
    fp = fopen(GROUP_PATH, "r");
    while(!feof(fp))
    {
        fgets(str_g, MAX_GROUP_ITEM, fp);  
        i=strlen(str_g) - 3;
        while(str_g[i] != ';')
            i--;
        i++;
        while(str_g[i]!=';' && gid[j]!='\0')
        {
            if(str_g[i] != gid[j])
            {
                flag = -1;
                break;
            }
            i++;
            j++;
        }
        if(flag==0 || str_g[i]==';' || gid[j]=='\0')
        {
            g_flag = 0;
            break;
        }
    }
    fclose(fp);
    if(g_flag != 0)
    {
        printf("\033[31mgid %s isn't exist! now,user %s's gid is NULL\033[0m\n", gid, name);
        gid[0] = '\0';
    }
    
    fp = fopen(PASSWD_PATH, "a");
    fprintf(fp, "%s;%s;%s;\n", name, passwd, gid);
    fclose(fp);
}

