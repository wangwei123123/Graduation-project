/*
 * ============================================================
 *       Filename:  read_command.c
 *    Description:  Thread 1 is used to call the readline library 
 *                  to get user input, and thread 2 is used to 
 *                  read keyboard device files to listen to user
 *                  input, in order to complete the function of 
 *                  automatic completion according to historical 
 *                  commands.
 *                  
 *                  PS:
 *                  Since the tab implementation of the readline 
 *                  library conflicts with the automatic completion 
 *                  of the history command, so now the tab completion 
 *                  command and file name is is annotated, now tab is
 *                  the default for the readline library,which is 
 *                  completed according to the file name.
 *        Version:  1.0
 *        Created:  2018.11.16
 *         Author:  ww
 *        Purpose:  Graduation project
 * ============================================================
 */
#include "ww_shell.h"
//#define DEBUG

char *s = NULL;

/*  call readline interface tab to finsh the 
 *  auto-completion of tab.
 *
static char *get_cmd_by_index(unsigned int index)
{
    if(index >= MAX_CMD_LIST)
        return NULL;
    return cmd_list[index];
}

static char *get_pwd_by_index(unsigned int index)
{
    if(index >= MAX_PWD_LIST)
        return NULL;
    return pwd_list[index];
}

// init the cmd_list
void cmd_list_init()
{
    DIR *dir;
    struct dirent *ptr;
    int i = 0;
    
    dir = opendir("/usr/bin/");
    while(ptr = readdir(dir))
    {
        if(strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, ".."))
        {
            strcpy(cmd_list[i++], ptr->d_name);
        }
    }
    closedir(dir);
}


//init the pwd_list
void pwd_list_init()
{
    DIR *dir;
    struct dirent *ptr;
    int i = 0;

    memset(pwd_list, 0, sizeof(pwd_list));
    dir = opendir(".");
    while(ptr = readdir(dir))
    {
        if(strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, ".."))
        {
            strcpy(pwd_list[i++], ptr->d_name);
        }
    }
    closedir(dir);
}

//matching pwd_dir generator; return possible dir
static char generator(const char *read_text, int state)
{
    static int list_index = 0, text_len = 0;
    static char *(*get_name_by_index)(unsigned int index);

    const char *name = NULL;
    
    if(!state)
    {
        list_index = 0;
        text_len = strlen(read_text);
    }
    if(strstr(read_text, " ") == NULL)
    {
        get_name_by_index = &get_cmd_by_index;
    }
    else
    {
        pwd_list_init();
        get_name_by_index = &get_pwd_by_index;
    }
    while(name = get_name_by_index(list_index))
    {
        list_index++;
        if(strncmp(name, read_text, text_len) == 0)
            return strdup(name);
    }
    return ((char*)NULL);
}

char **readline_cmd_completion(const char *read_text, int start, int end)
{
    char **matches = (char**)NULL;
    if(start == 0)
        matches = rl_completion_matches(read_text, generator);
    return matches;
}
*/
void readline_init()
{
    //cmd_list_init();
    //rl_attempted_completion_function = readline_cmd_completion;
    //rl_bind_key ('\t', NULL);
}


/* read history file in reverse row to match the history cmd by input */
int match_history(char *input, char *history_input, int len)
{
    FILE *f;
    int c;
    long offset;

    f = fopen(COMMAND_HISTORY, "rb");
    fseek(f, -1, SEEK_END);
    while(1)
    {
        c = fgetc(f);
        if(c=='\n')
        {
            offset = ftell(f);
            fgets(history_input, MAX_CMD_LEN, f);
           
            if(!strncmp(input, history_input, strlen(input)))
            {
                fclose(f);
                return 0;
            }
            
            fseek(f, offset-2, SEEK_SET);
        } else
            if(fseek(f, -2, SEEK_CUR))
            {
                fseek(f, 0, SEEK_SET);
                fgets(history_input, MAX_CMD_LEN, f);
                
                if(!strncmp(input, history_input, strlen(input)))
                {
                    fclose(f);
                    return 0;
                }

                break;
            }
    }
    fclose(f);
    return -1;
}


/* auto completion command and auto completion of tab */
void auto_cmd()
{
    int keys_fd;
    int flag = -1;
    struct input_event current_key;
    char history_text[MAX_CMD_LEN];
    char insert_text[MAX_CMD_LEN];
    char *input;
    
    if((keys_fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
    {
        perror("open character block device");
        return;
    }

    while(1)
    {
        if(read(keys_fd, &current_key, sizeof(current_key)) == sizeof(current_key))
        {  
            if(current_key.type == EV_KEY)
            {
            /*
                if(current_key.code == KEY_TAB)
                {
                    if()
                }
            */
                if(current_key.value==0 && current_key.code==KEY_RIGHT)
                {    
                    flag = 0;
                    rl_point = rl_end;
                }
                if(current_key.value==0 && current_key.code==KEY_BACKSPACE)
                {    
                    flag = -1;
                }
              
              /*
                if(current_key.value==0 && current_key.code==KEY_ENTER)
                {
                    if(flag == -1);
                        rl_delete_text(rl_point, rl_end+1);
                }
                */

                if(current_key.value==1 && current_key.code==KEY_ENTER)
                {
                    if(flag == -1)
                        rl_delete_text(rl_point, rl_end+1);
                    pthread_exit(NULL);
                }
               

                if(current_key.value==0 && current_key.code!=KEY_ENTER)
                {
                    rl_delete_text(rl_point, rl_end+1);
                    
                    input = rl_copy_text(0,MAX_CMD_LEN);
                    if(strlen(input)==0)
                        continue;
                   
                    if(match_history(input, history_text, strlen(input)) == 0)
                    {
                        strcpy(insert_text, history_text+strlen(input));
                        insert_text[strlen(insert_text)-1] = '\0';
                        rl_mark = rl_point;
                        rl_insert_text(insert_text);
                        rl_point = rl_mark;
                    }
                }
                rl_redisplay();
            }
        }
    }
    close(keys_fd);
}

void write_into_history(char *s)
{
    FILE *fp, *ftmp;
    int flag = -1;
    int line = 0;
    int ch;
    
    /* Calculate whether the number of historical command 
     * lines exceeds the specified upper limit.  
     * */
    fp = fopen(COMMAND_HISTORY, "r+");
    while((ch = fgetc(fp)) != EOF)
        if(ch == '\n')
            line++;
    if(line == MAX_HISTORY_COMMAND_LINE)
        flag = 0;
    fclose(fp);

    /* Delete the number of history that exceeds the limit */ 
    if(flag == 0)
    {
        fp = fopen(COMMAND_HISTORY, "r");
        ftmp = fopen(COMMAND_HISTORY_TMP, "w");
        while(1)
        {
            ch = fgetc(fp);
            if((ch == EOF) || (ch == '\n'))
                break;
        }
        if(ch != EOF)
            while(1)
            {
                ch = fgetc(fp);
                if(ch == EOF)
                    break;
                fputc(ch, ftmp);
            }
        fclose(fp);
        fclose(ftmp);
        remove(COMMAND_HISTORY);
        rename(COMMAND_HISTORY_TMP, COMMAND_HISTORY);
    }

    /* write the current command to the history command file. */
    fp = fopen(COMMAND_HISTORY, "a");
    fprintf(fp, "%s\n", s);
    fclose(fp);
}

void ww_read_command(char *prompt)
{
    pthread_t thread_id;

    readline_init();
    read_history(COMMAND_HISTORY);
    pthread_create(&thread_id, NULL, &auto_cmd, NULL);
    if(s)
    {
        free(s);
        s = (char*)malloc(MAX_CMD_LEN*sizeof(char));
    }
    s = readline(prompt);
    pthread_join(thread_id, NULL);
    if(s && *s)
    {
        add_history(s);
        write_into_history(s);
    }
    
}


#ifdef DEBUG
void print_list(int status)
{
    int i;
    if(status)
        for(i=0;pwd_list[i]!=NULL;i++)
            printf("%s\n", pwd_list[i]);
    else
        for(i=0;cmd_list[i]!=NULL;i++)
            printf("%s\n", cmd_list[i]);
}

int main()
{
   ww_read_command("[myshell]"); 
}

#endif
