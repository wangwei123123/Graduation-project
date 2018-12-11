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
#include <signal.h>
#include <termios.h>
#include <errno.h>

#define COMMAND_HISTORY "./.history"
#define COMMAND_HISTORY_TMP "./.history.tmp"
#define DEV_PATH "/dev/input/event1"
#define PASSWD_PATH "./etc/passwd"
#define PASSWD_PATH_TMP "./etc/passwd.tmp"
#define GROUP_PATH "./etc/group"
#define GROUP_PATH_TMP "./etc/group.tmp"
#define INTRODUCTION_PATH "./etc/introduction.doc"
#define INTRODUCTION_PATH_TMP "./etc/introduction.doc.tmp"
#define MAX_CWD 50
#define MAX_HOSTNAME 50
#define MAX_CMD_LIST 2000
#define MAX_PWD_LIST 2000
#define MAX_ARG 20
#define MAX_CMD_LEN 100
#define MAX_PROMPT_LEN 100
#define MAX_HISTORY_COMMAND_LINE 500
#define MAX_USER_NAME 50
#define MAX_USER_PASSWD 50
#define MAX_GROUP_NAME 50
#define MAX_GROUP_PASSWD 50
#define MAX_GROUP 3
#define MAX_PASSWD_ITEM 100
#define MAX_GROUP_ITEM 100
#define MAX_HELP_LINE 100
#define ECHO_FLAGS (ECHO | ECHOE | ECHOK | ECHONL)
#define MAX_LOGIN_TIMES 3
#define MAGICAL 4
#define ASCII_CHAR 95
#define ASCII_SUB_BASE(x) (x-32)
#define ASCII_ADD_BASE(x) (x+32)
#define ENV_VAR_NAME_LEN 10
#define MAX_LET_NUM 50

char cmd_list[MAX_CMD_LIST][20];
char pwd_list[MAX_PWD_LIST][20];

/* function in the main loop */
void ww_prompt(char *);
void ww_read_command(char *);

/* Information about the current user */
struct current_user_info{
    char hostname[MAX_HOSTNAME];
    struct passwd *pwd;
    char *cwd;
}current_user_info;

/* login user information */
struct login_user_info{
    char name[MAX_USER_NAME];
    char passwd[MAX_USER_PASSWD];
};

/* trie to store history command */
typedef struct tree_node{
    int weight;
    struct tree_node *next[ASCII_CHAR];
}Tree;

/* store the input commands to store into 
 * history file before exit
 * */
typedef struct command_node{
    char command[MAX_CMD_LEN];
    struct command_node *next;
}Com_list;

/* Storage environment variable */
typedef struct var_node{
    int flag;
    char var_name[ENV_VAR_NAME_LEN];
    double value;
    struct var_node *next[ASCII_CHAR];
}Tree_var;

/* Storage data in expressions of let command */
typedef struct stack_let_num{
    double num[MAX_LET_NUM];                                                    
    int top;
}Stack_let_num;

/* Storage symbol in expressions of let command */
typedef struct stack_sym{
    char sym[MAX_LET_NUM];
    int top;
}Stack_let_sym;
