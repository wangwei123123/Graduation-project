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
void ww_exit()
{
    FILE *fp, *ftmp;
    Com_list *cur;
    int sum;
    int count;
    int flag=0;
    char ch;
    
    extern int command_num;
    extern int history_num;
    extern Com_list commands;
    sum = command_num + history_num;
    cur = commands.next;
    
    if(sum < MAX_HISTORY_COMMAND_LINE)
    {
        //Directly stored in the history command file
        printf("0\n");
        fp = fopen(COMMAND_HISTORY, "a");
        while(cur)
        {
            fprintf(fp, "%s\n", cur->command);
            cur = cur->next;
        }
        fclose(fp);
    }
    else if((sum < 2*MAX_HISTORY_COMMAND_LINE) && (sum >MAX_HISTORY_COMMAND_LINE))
    {
        /*
         * After deleting the contents of some historical 
         * command files, save the history command file.
         * */
        printf("1\n");
        fp = fopen(COMMAND_HISTORY, "r");
        ftmp = fopen(COMMAND_HISTORY_TMP, "w");
        
        count = sum - MAX_HISTORY_COMMAND_LINE;
        while(1)
        {
            ch = fgetc(fp);
            if((ch == EOF) || (ch == '\n'))
            {
                flag++;
                if(flag == count)
                    break;
            }
        }
        
        if(ch != EOF)
            while(1)
            {
                ch = fgetc(fp);
                if(ch == EOF)
                    break;
                fputc(ch, ftmp);
            }
        
        while(cur)
        {
            fprintf(ftmp, "%s\n", cur->command);
            cur = cur->next;
        }

        fclose(fp);
        fclose(ftmp);
        remove(COMMAND_HISTORY);
        rename(COMMAND_HISTORY_TMP, COMMAND_HISTORY);
    }
    else
    {
        /*
         * Directly generate a new file over the original 
         * history command file with the command of the 
         * last upper limit in the linked list.
         * */
        printf("2\n");
        count = sum - 2*MAX_HISTORY_COMMAND_LINE +1;
    
        for(flag=0;flag<count;flag++)
            cur = cur->next;

        fp = fopen(COMMAND_HISTORY, "w+");
        while(cur)
        {
            fprintf(fp, "%s\n", cur->command);
            cur = cur->next;
        }
        fclose(fp);
    }

    printf("the program will be exit!\n");
    exit(0);
}


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
        fgets(str, MAX_CMD_LEN, fp);
        while(!feof(fp))
        {
            printf("%d\t%s", ++i, str);
            fgets(str, MAX_CMD_LEN, fp);
        }
    }
    fclose(fp);
}

void ww_addgroup(char *arg[])
{
    FILE *fp;
    int g_flag=0, p_flag=0, flag=1;
    char name[MAX_GROUP_NAME];
    char str_g[MAX_GROUP_ITEM];
    char gid[MAX_GROUP];
    char passwd[MAX_GROUP_PASSWD];
    int i, j;
    int last_gid;

    printf("password only supports numbers and letters\n");
    
    get_groupname(name, MAX_GROUP_NAME);
    
    fp = fopen(GROUP_PATH, "r");
    fgets(str_g, MAX_GROUP_ITEM, fp);
    while(!feof(fp))
    {
        i = 0;
        while(str_g[i]!=';' && name[i]!='\0')
        {
            i++;
            if(str_g[i] != name[i])
                break;
        }
        if(str_g[i]==';' && name[i]=='\0')
        {
            g_flag = -1;
            break;
        }
        fgets(str_g, MAX_GROUP_ITEM, fp);
    }
    if(g_flag != 0)
    {
        printf("\033[31madd group failed : group %s already exist.\033[0m\n", name);
        return;
    }
    fclose(fp);
  
    while(flag<4)
    {
        p_flag = 0;
        set_disploy_mode(STDIN_FILENO, 0);
        get_passwd(passwd, MAX_GROUP_PASSWD);
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
        printf("\033[31madd group failed : wrong password format.\033[0m\n");
        return;
    }
    encryption(passwd);


    i=strlen(str_g) - 3;
    while(str_g[i] != ';')
        i--;
    i++;
    j = 0;
    while(str_g[i]!=';')
        gid[j++] = str_g[i++];
    last_gid = atoi(gid) + 1;
    sprintf(gid, "%d", last_gid);
    fp = fopen(GROUP_PATH, "a");
    fprintf(fp, "%s;%s;%s;\n", name, passwd, gid);
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
    fgets(str_p, MAX_PASSWD_ITEM, fp);
    while(!feof(fp))
    {
        i = 0;
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
        fgets(str_p, MAX_PASSWD_ITEM, fp);
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
    fgets(str_g, MAX_GROUP_ITEM, fp);  
    while(!feof(fp))
    {
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
        fgets(str_g, MAX_GROUP_ITEM, fp);  
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


void ww_deluser(char *arg[])
{
    FILE *fp, *ftmp;
    char str_p[MAX_PASSWD_ITEM];
    char name[MAX_USER_NAME];
    int i;

    strcpy(name, arg[1]);
    fp = fopen(PASSWD_PATH, "r+");
    ftmp = fopen(PASSWD_PATH_TMP, "w+");

    fgets(str_p, MAX_PASSWD_ITEM, fp);
    while(!feof(fp))
    {
        i = 0;
        while(str_p[i]!=';' && name[i]!='\0')
        {
            i++;
            if(str_p[i] != name[i])
                break;
        }
        if(str_p[i]!=';' || name[i]!='\0')
        {
           fprintf(ftmp, "%s", str_p); 
        }
        fgets(str_p, MAX_PASSWD_ITEM, fp);
    }
    fclose(ftmp);
    fclose(fp);
    remove(PASSWD_PATH);
    rename(PASSWD_PATH_TMP, PASSWD_PATH);
}

void ww_delgroup(char *arg[])
{
    FILE *fp, *ftmp;
    char str_g[MAX_GROUP_ITEM];
    char name[MAX_GROUP_NAME];
    int i;

    strcpy(name, arg[1]);
    fp = fopen(GROUP_PATH, "r+");
    ftmp = fopen(GROUP_PATH_TMP, "w+");

    fgets(str_g, MAX_GROUP_ITEM, fp);
    while(!feof(fp))
    {
        i = 0;
        while(str_g[i]!=';' && name[i]!='\0')
        {
            i++;
            if(str_g[i] != name[i])
                break;
        }
        if(str_g[i]!=';' || name[i]!='\0')
        {
           fprintf(ftmp, "%s", str_g); 
        }
        fgets(str_g, MAX_GROUP_ITEM, fp);
    }
    fclose(ftmp);
    fclose(fp);
    remove(GROUP_PATH);
    rename(GROUP_PATH_TMP, GROUP_PATH);
}

void ww_cd(char *arg[])
{
    if(chdir(arg[1]))
    {
        perror("chdir");
    }
    
}
