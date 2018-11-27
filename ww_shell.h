#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <linux/input.h>
#include<signal.h>

#define COMMAND_HISTORY "./.history"
#define COMMAND_HISTORY_TMP "./.history.tmp"
#define DEV_PATH "/dev/input/event1"
#define MAX_CWD 50
#define MAX_HOSTNAME 20
#define MAX_CMD_LIST 2000
#define MAX_PWD_LIST 2000
#define MAX_ARG 20
#define MAX_CMD_LEN 100
#define MAX_PROMPT_LEN 100
#define MAX_HISTORY_COMMAND_LINE 500


char cmd_list[MAX_CMD_LIST][20];
char pwd_list[MAX_PWD_LIST][20];

/* function in the main loop */
void ww_prompt(char *);
void ww_read_command(char *);

/* Information about the current user */                                                     struct current_user_info{
    char hostname[MAX_HOSTNAME];
    struct passwd *pwd;
    char *cwd;
}current_user_info;
