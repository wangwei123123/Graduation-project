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

char pre_pwd[MAX_CWD];
/**************************************************************
 *
 * Function name : ww_help
 * Description : Print help informations.
 * Parameter : NULL
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void ww_help()
{
    FILE *fp, *ftp;
    char str[MAX_HELP_LINE];
    char para[MAX_HELP_LINE];

    fp = fopen(INTRODUCTION_PATH, "r");
    ftp = fopen(INTRODUCTION_PATH_TMP, "w+");

    sprintf(para, "%s %s", "less -r", INTRODUCTION_PATH_TMP);

    fgets(str, sizeof(str), fp);
    while(!feof(fp))
    {
        if(str[0]>64 && str[0]<91)
        {
            str[strlen(str)-1] = '\0';
            fprintf(ftp, "\033[1m%s\033[m\n", str);
        }
        else
            fprintf(ftp, "%s", str);
        fgets(str, sizeof(str), fp);
    }

    system(para);
    remove(INTRODUCTION_PATH_TMP);
}


/**************************************************************
 *
 * Function name : ww_exit
 * Description : The program exits and performs a custom action
 *               before exiting.E.g,save the commands entered
 *               during this execution to the history command
 *               file.
 * Parameter : NULL
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
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


/**************************************************************
 *
 * Function name : ww_history
 * Description : View the contents of the history command file
 *               or clear the built-in commands of the history
 *               command file.
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : If arg[1] is "clear" for clearing the history
 *         command file;
 *         else if arg[1] is NULL for print history command file.
 * ***********************************************************/
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

/**************************************************************
 *
 * Function name : ww_addgroup
 * Description : Add a user group. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
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

/**************************************************************
 *
 * Function name : ww_adduser
 * Description : Add a user. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
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

/**************************************************************
 *
 * Function name : ww_deluser
 * Description : Del a user. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
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

/**************************************************************
 *
 * Function name : ww_delgroup
 * Description : Del a group. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
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

/**************************************************************
 *
 * Function name : ww_su
 * Description : Change current user. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void ww_su(char *arg[])
{
    extern struct login_user_info login_user;
    struct login_user_info su_user;
    FILE *fp;
    char str_p[MAX_PASSWD_ITEM];
    int i, u_flag=0;

    memset(&su_user, 0, sizeof(su_user));
    
    strcpy(su_user.name, arg[1]);
    fp = fopen(PASSWD_PATH, "r");
    fgets(str_p, MAX_PASSWD_ITEM, fp);
    while(!feof(fp))
    {
        i = 0;
        while(str_p[i]!=';' && su_user.name[i]!='\0')
        {
            i++;
            if(str_p[i] != su_user.name[i])
                break;
        }
        if(str_p[i]==';' && su_user.name[i]=='\0')
        {
            u_flag = -1;
            break;
        }
        fgets(str_p, MAX_PASSWD_ITEM, fp);
    }
    fclose(fp);
    if(u_flag == 0)
    {
        printf("su: user %s does not exist.\n", su_user.name);
        return;
    } 

    set_disploy_mode(STDIN_FILENO, 0); 
    get_passwd(su_user.passwd, MAX_USER_PASSWD);
    set_disploy_mode(STDIN_FILENO, 1);
    printf("\n");

    if(check_user(su_user) == -1)
    {
        printf("su: Authentication failure.\n");
        return;
    }

    strcpy(login_user.name, su_user.name);
    strcpy(login_user.passwd, su_user.passwd);
}


/**************************************************************
 *
 * Function name : ww_cd
 * Description : Change current directory. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void ww_cd(char *arg[])
{
    char pwd[MAX_CWD];
    getcwd(pwd, sizeof(pwd));
    if(strcmp(arg[1], "-") == 0)
    {
        chdir(pre_pwd);
    }
    else
        chdir(arg[1]);
    strcpy(pre_pwd, pwd);
}

/**************************************************************
 *
 * Function name : del_var_tree
 * Description : Remove variables from the trie of the stored
 *               variable by the variable name.
 * Parameter : 
 *              @str    Specify variable name.
 * Return : 
 *              -1 : Specify variable name dosen't exist;
 *           other : Del success.
 * Other : NULL
 * ***********************************************************/
int del_var_tree(char *str)
{
    int i;
    Tree_var *cur;
    extern Tree_var var_tree;

    cur = &var_tree;
    
    for(i=0;str[i]!='\0';i++)
    {
        if(cur->next[ASCII_SUB_BASE(str[i])] == NULL)
            return -1;
        cur = cur->next[ASCII_SUB_BASE(str[i])];
    }
    cur->flag = 0;
}

/**************************************************************
 *
 * Function name : insert_var_tree
 * Description : Save variables in the trie of the stored
 *               variable.
 * Parameter : 
 *              @str    Specify variable name.
 *              @value  Value of the variable.
 * Return : NULL 
 * Other : NULL
 * ***********************************************************/
void insert_var_tree(char *str, double value)
{
    int i;
    Tree_var *cur;
    extern Tree_var var_tree;

    cur = &var_tree;
    
    for(i=0;str[i]!='\0';i++)
    {
        if(cur->next[ASCII_SUB_BASE(str[i])] == NULL)
        {
            Tree_var *new = (Tree_var*)malloc(sizeof(Tree_var));
            memset(new, 0, sizeof(Tree_var));
            cur->next[ASCII_SUB_BASE(str[i])] = new;
        }
        cur = cur->next[ASCII_SUB_BASE(str[i])];
    }
    cur->flag=1;
    cur->value = value;
    return;
}

#ifdef DEBUG
/**************************************************************
 *
 * Function name : traverse_var_tree
 * Description : Traversing the trie of the stored variable.
 * Parameter : 
 *              @cur    The node of Tree_var.
 * Return : NULL 
 * Other : NULL
 * ***********************************************************/
void traverse_var_tree(Tree_var *cur)
{
    static char var_name[ENV_VAR_NAME_LEN];
    static int pos = 0;
    int i;

    if(cur == NULL)
        return;
    if(cur->flag)
    {
        var_name[pos] = '\0';
        printf("%s = %lf\n", var_name, cur->value);
    }
    for(i=0;i<ASCII_CHAR;i++)
    {
        var_name[pos++] = ASCII_ADD_BASE(i);
        traverse_var_tree(cur->next[i]);
        pos--;
    }
}
#endif

/**************************************************************
 *
 * Function name : ww_set
 * Description : Setting environment variables. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL 
 * Other : NULL
 * ***********************************************************/
void ww_set(char *arg[])
{
    int i;
    for(i=0;arg[i]!=NULL;i++);
    if(i!=4 || strcmp(arg[2],"="))
    {
        printf("enter format error\n"
               "Enter as follows : set a = 1\n");
        return;
    }
    insert_var_tree(arg[1], atof(arg[3]));
}

/**************************************************************
 *
 * Function name : ww_unset
 * Description : Unsetting environment variables. 
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL 
 * Other : NULL
 * ***********************************************************/
void ww_unset(char *arg[])
{
    int i;
    for(i=0;arg[i]!=NULL;i++);
    if(i!=2)
    {
        printf("enter format error\n"
               "Enter as follows : unset a\n");
        return;
    }
    del_var_tree(arg[1]);
}

/**************************************************************
 *
 * Function name : find_var
 * Description : Find environment variables.
 * Parameter : 
 *              @var    variable name.
 *              @value  The address to store the variable value.
 * Return :
 *              0 : Variable doesn't exist;
 *              1 : Find variable successfully.
 * Other : NULL
 * ***********************************************************/
int find_var(char *var, double *value)
{
    int i;
    Tree_var *cur;
    extern Tree_var var_tree;

    cur = &var_tree;
    for(i=0;var[i]!='\0';i++)
    {
        if(cur->next[ASCII_SUB_BASE(var[i])] == NULL)
        {
            return 0;
        }
        cur = cur->next[ASCII_SUB_BASE(var[i])];
    }
    *value = cur->value;
    return 1;
}

/**************************************************************
 *
 * Function name : ww_echo
 * Description : Print user input.
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void ww_echo(char *arg[])
{
    int i, j;
    double value;
    char str[ENV_VAR_NAME_LEN];

    for(i=1;arg[i]!=NULL;i++)
    {
        if(strncmp(arg[i], "$", 1) == 0)
        {
            strcpy(str, arg[i]+1);
            if(find_var(str, &value) == 1)
                printf("%lf ", value);
        }
        else
            printf("%s ", arg[i]);
    }
    printf("\n");
}

/**************************************************************
 *
 * Function name : init_stack_let_num
 * Description : Init data stack
 * Parameter : 
 *              @arg   The address of Stack_let_num. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void init_stack_let_num(Stack_let_num *stack)
{
    stack->top = -1;
}

/**************************************************************
 *
 * Function name : init_stack_let_num
 * Description : Init symbol stack
 * Parameter : 
 *              @arg   The address of Stack_let_sym. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void init_stack_let_sym(Stack_let_sym *stack)
{
    stack->top = -1;
}

/**************************************************************
 *
 * Function name : stack_push_let_num
 * Description : Push data onto Stack_let_num.
 * Parameter : 
 *              @stack  The address of Stack_let_num.
 *              @num    Data pushed onto the stack.
 * Return :
 *              -1 : pushing the stack failed.
 *               0 : pushing the stack successfully.
 * Other : NULL
 * ***********************************************************/
int stack_push_let_num(Stack_let_num *stack, double num)
{
    if(stack->top == MAX_LET_NUM-1)
    {
        printf("\033[31mData reaches the upper limit,input fails\033[0m\n");
        return -1;
    }
    stack->top++;
    stack->num[stack->top] = num;
    return 0;
}

/**************************************************************
 *
 * Function name : stack_push_let_sym
 * Description : Push symbol onto Stack_let_sym.
 * Parameter : 
 *              @stack  The address of Stack_let_sym.
 *              @num    Symbol pushed onto the stack.
 * Return :
 *              -1 : pushing the stack failed.
 *               0 : pushing the stack successfully.
 * Other : NULL
 * ***********************************************************/
int stack_push_let_sym(Stack_let_sym *stack, char sym)
{
    if(stack->top == MAX_LET_NUM-1)
    {
        printf("\033[31mSymbol reaches the upper limit,input fails\033[0m\n");
        return -1;
    }
    stack->top++;
    stack->sym[stack->top] = sym;
    return 0;
}

/**************************************************************
 *
 * Function name : stack_pop_let_num
 * Description : Pop the data from the Stack_let_num.
 * Parameter : 
 *              @stack  The address of Stack_let_num.
 * Return : Data popped from the Stack_let_num.
 * Other : NULL
 * ***********************************************************/
double stack_pop_let_num(Stack_let_num *stack)
{
    double num;

    num = stack->num[stack->top];
    stack->top--;
    return num;
}

/**************************************************************
 *
 * Function name : stack_pop_let_sym
 * Description : Pop the symbol from the Stack_let_sym.
 * Parameter : 
 *              @stack  The address of Stack_let_sym.
 * Return : Symbol popped from the Stack_let_sym.
 * Other : NULL
 * ***********************************************************/
char stack_pop_let_sym(Stack_let_sym *stack)
{
    char sym;

    sym = stack->sym[stack->top];
    stack->top--;
    return sym;
}

/**************************************************************
 *
 * Function name : judge_symbol
 * Description : Judge whether the character is a calculation
 *               symbol.
 * Parameter : 
 *              @ch The character.
 * Return :
 *              1 : The character is a calculation symbol;
 *              0 : The character isn't a calculation symbol.
 * Other : NULL
 * ***********************************************************/
int judge_symbol(char ch)
{
    if(ch=='+' || ch=='-' || ch=='*' || ch=='/' ||\
       ch=='(' || ch==')' || ch=='=')
        return 1;
    return 0;
}

/**************************************************************
 *
 * Function name : op_cmp
 * Description : Compare two operator precedence.
 * Parameter : 
 *              @op1 : The first operator.
 *              @op2 : The second operator.
 * Return :
 *              '<' : The priority of first operator is greater
 *                    than the priority of second operator.
 *
 *              '=' : The priority of first operator is equal
 *                    to the priority of second operator.
 *
 *              '>' : The priority of first operator is less
 *                    than the priority of second operator.
 * Other :
 *              op1 is the operator which is stack top pointer.
 *              op2 is the operator which is the next pointer.
 * ***********************************************************/
char op_cmp(char op1, char op2)
{
    char result;

    switch(op2)
    {
        case '+':
        case '-':
        {
            result = (op1 =='(' || op1 == '=')?'<':'>';
            break;
        }
        case '*':
        case '/':
        {
            result = (op1 == '*' || op1 == '/' || op1 == ')')?'>':'<';
            break;
        }
        case '(':
        {
            if(op1 == ')')
            {
                printf("input error\n");
                return -1;
            }
            else
            {
                result = '<';
            }
            break;
        }
        case ')':
        {
            switch(op1)
            {
                case '(':
                {
                    result = '=';
                    break;
                }
                case '=':
                {
                    printf("error input\n");
                    return -1;
                }
                default:
                    result = '>';
            }
            break;
        }
        case '=':
        {
            switch(op1)
            {
                case '=':
                {
                    result = '=';
                    break;
                }
                case '(':
                {
                    printf("error input\n");
                    return -1;
                }
                default:
                    result = '>';
            }
        }
    }
    return result;
}

/**************************************************************
 *
 * Function name : calculate
 * Description : Calculate addition, subtraction, multiplication
 *               and division.
 * Parameter : 
 *              @num1   The first operand.
 *              @num2   The second operand.
 *              @op     Operator.
 *              @result The final calculation result.
 * Return :
 *              -1 : Calculation error.
 *               0 : Complete calculation.
 * Other : NULL
 * ***********************************************************/
int calculate(double num1, double num2, char op,double *result)
{
    switch(op)
    {
        case '+':
        {
            *result = num1 + num2;
            break;
        }
        case '-':
        {
            *result = num1 - num2;
            break;
        }
        case '*':
        {
            *result = num1 * num2;
            break;
        }
        case '/':
        {
            if(num2 == 0)
            {
                return -1;
            }
            *result = num1 / num2;
            break;
        }
        default:
            return -1;
    }
    return 0;
}

/**************************************************************
 *
 * Function name : ww_let
 * Description : Print calculation results based on the
 *               calculation expression entered by the user.
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void ww_let(char *arg[])
{
    int i, j=0, flag=0;
    char str[20];
    char exp[MAX_CMD_LEN];
    char sign;
    double num, num1, num2, result;
    Stack_let_num stack_num;
    Stack_let_sym stack_sym;

/*
    strcpy(exp, arg[1]);
    i = strlen(exp);
    exp[i] = '=';
    exp[i+1] = '\0';
*/
    for(i=0;arg[1][i]!='\0';i++)
    {
        if(i>0)
        {
            if(arg[1][i]=='(' && arg[1][i-1]==')')
            {
                exp[j++] = '*';
                exp[j++] = arg[1][i];
                continue;
            }
        }
        exp[j++] = arg[1][i];
    }
    exp[j++] = '=';
    exp[j] = '\0';

    //printf("----input is%s-----\n",arg[1]);
    //printf("--------%s---------\n",exp);

    init_stack_let_num(&stack_num);
    init_stack_let_sym(&stack_sym);
    memset(str, 0, sizeof(str));

    stack_push_let_sym(&stack_sym, '=');
    i = 0;
    flag = 0;
    while((exp[i]!='\0') && ((exp[i] != '=') ||stack_sym.sym[stack_sym.top] != '='))
    {
        if(judge_symbol(exp[i]) == 0)
        {
            j = 0;
            while(judge_symbol(exp[i]) == 0)
            {
                if(exp[i]<48 || exp[i] >57)
                    flag = 1;
                str[j] = exp[i];
                j++;
                i++;
            }
            str[j] = '\0';
            if(flag == 1)
            {
                if(find_var(str, &num) == 0)
                {
                    printf("variable %s does not exist!\n", str);
                    return;
                }
            }
            else
                num = atof(str);
            flag = 0;
            if(stack_push_let_num(&stack_num, num) == -1)
                return;
            
            //printf("%lf is store\n",stack_num.num[stack_num.top]);

            memset(str, 0, sizeof(str));
        }
        else
        {
            switch(op_cmp(stack_sym.sym[stack_sym.top], exp[i]))
            {
                case '<':
                {
                    //printf("input %c,store\n", exp[i]);
                    stack_push_let_sym(&stack_sym, exp[i]);
                    i++;
                    break;
                }
                case '=':
                {
                    //printf("input %c,pop %c\n", exp[i],stack_sym.sym[stack_sym.top]);
                    sign = stack_pop_let_sym(&stack_sym);
                    i++;
                    break;
                }
                case '>':
                {
                    //printf("input %c,ji suan\n", exp[i]);
                    sign = stack_pop_let_sym(&stack_sym);
                    num1 = stack_pop_let_num(&stack_num);
                    num2 = stack_pop_let_num(&stack_num);
                    //printf("num1=%lf,num2=%lf,sign=%c\n",num1,num2,sign);
                    if(calculate(num1, num2, sign, &result) == -1)
                        return;
                    if(stack_push_let_num(&stack_num, result) == -1)
                        return;
                    //printf("%lf is store\n", stack_num.num[stack_num.top]);
                    break;
                }
            }
        }
    }
    //printf("fin\n");
    //for(j=0;j<=stack_num.top;j++)
    //    printf("%lf\n",i,stack_num.num[j]);
    printf("%lf\n", stack_num.num[stack_num.top]);
}


/**************************************************************
 *
 * Function name : which
 * Description : Print the specified command file path.
 * Parameter : 
 *              @str   Command name.
 *              @cmd_path The address to store the path of command.
 * Return :
 *              0 : Find success.
 *              1 : Find failure.
 * Other : NULL
 * ***********************************************************/
int which(char *str,char *cmd_path)
{
    int j;
    DIR *dir;
    struct dirent *ptr;
    char *path[PATH_NUM] = {PATH};

    for(j=0;j<PATH_NUM;j++)
    {
        if((dir = opendir(path[j])) == NULL)
        {
            perror("Open dir");
            return;
        }

        while((ptr = readdir(dir)) != NULL)
        {
            if(strcmp(ptr->d_name, ".")==0 || strcmp(ptr->d_name, "..")==0)
            {
                continue;
            }

            if(ptr->d_type == 8)//file
            {
                if(strcmp(ptr->d_name, str) == 0)
                {
                    sprintf(cmd_path, "%s/%s", path[j], ptr->d_name);
                    return 0;
                }
            }
            /*
            else if(ptr->d_type == 10)//link file
            {
                //.....
            }
            else if(ptr->d_type == 4)//dir
            {
                //.....
            }
            */
        }
        closedir(dir);
    }
    return 1;
}

/**************************************************************
 *
 * Function name : ww_which
 * Description : Print the specified command list file path.
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void ww_which(char *arg[])
{
    int i;
    char cmd_path[MAX_CMD_LEN];

    for(i=1;arg[i]!=NULL;i++)
    {
        if(which(arg[i], cmd_path) == 1)
            printf("which: no %s in (/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin)\n", arg[i]);
        else
            printf("%s\n", cmd_path);
    }
}

/**************************************************************
 *
 * Function name : insert_builtin_tree
 * Description : Save built-in command in the trie.
 * Parameter : 
 *              @str    Specify built-in command name.
 * Return : NULL 
 * Other : NULL
 * ***********************************************************/
void insert_builtin_tree(char *str)
{
    int i;
    Tree_builtin *cur;
    extern Tree_builtin builtin_tree;

    cur = &builtin_tree;
    
    for(i=0;str[i]!='\0';i++)
    {
        if(cur->next[ASCII_SUB_BASE(str[i])] == NULL)
        {
            Tree_builtin *new = (Tree_builtin*)malloc(sizeof(Tree_builtin));
            memset(new, 0, sizeof(Tree_builtin));
            cur->next[ASCII_SUB_BASE(str[i])] = new;
        }
        cur = cur->next[ASCII_SUB_BASE(str[i])];
    }
    cur->flag=1;
    return;
}

/**************************************************************
 *
 * Function name : traverse_builtin_tree
 * Description : Traversing the trie stored the name of built-in 
 *               commands.
 * Parameter : 
 *              @cur    The node of the Tree_builtin.
 * Return : NULL 
 * Other : NULL
 * ***********************************************************/
#ifdef DEBUG
void traverse_builtin_tree(Tree_builtin *cur)
{
    static char name[MAX_BUILTIN_LEN];
    static int pos = 0;
    int i;

    if(cur == NULL)
        return;
    if(cur->flag)
    {
        name[pos] = '\0';
        printf("%s\n", name);
    }
    for(i=0;i<ASCII_CHAR;i++)
    {
        name[pos++] = ASCII_ADD_BASE(i);
        traverse_builtin_tree(cur->next[i]);
        pos--;
    }
}
#endif


/**************************************************************
 *
 * Function name : find_builtin
 * Description : Find built-in command.
 * Parameter : 
 *              @str    Command name.
 * Return :
 *              1 : Str does not exist in built-in commands trie.
 *              0 : Str exist in built-in commands trie.
 * Other : NULL
 * ***********************************************************/
int find_builtin(char *str)
{
    int i;
    Tree_builtin *cur;
    extern Tree_builtin builtin_tree;

    cur = &builtin_tree;
    for(i=0;str[i]!='\0';i++)
    {
        if(cur->next[ASCII_SUB_BASE(str[i])] == NULL)
        {
            return 1;
        }
        cur = cur->next[ASCII_SUB_BASE(str[i])];
    }
    return 0;
}

/**************************************************************
 *
 * Function name : ww_type
 * Description : Determine whether the specified command is a
 *               built-in command or an external command.
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void ww_type(char *arg[])
{
    int i;
    char cmd_path[MAX_CMD_LEN];

    for(i=1;arg[i]!=NULL;i++)
    {
        if(find_builtin(arg[i]) == 0)
            printf("%s is a shell builtin\n", arg[i]);
        else
            if(which(arg[i], cmd_path) == 1)
                printf("type: %s: not found\n", arg[i]);
            else
                printf("%s is %s\n", arg[i], cmd_path);
    }
}

/**************************************************************
 *
 * Function name : ww_test
 * Description : Used to check if a condition is true,it can
 *               test both values and files.
 * Parameter : 
 *              @arg   User-entered string. 
 * Return : NULL
 * Other :
 *          values
 *                  -eq     VALUE1 is equal to VALUE2;
 *
 *                  -ne     VALUE1 is not equal to VALUE2;
 *
 *                  -gt     VALUE1 is greater than VALUE2;
 *
 *                  -ge     VALUE1 is greater than or equal
 *                          to VALUE2;
 *
 *                  -lt     VALUE1 is less than VALUE2;
 *
 *                  -le     VALUE1 is less than or equal to
 *                          VALUE2;
 *
 *          files
 *                  -e      FILE exists;
 *
 *                  -r      FILE exists and read permission is
 *                          granted;
 *
 *                  -w      FILE exists and write permission is
 *                          granted;
 *
 *                  -x      FILE exists and execute (or search)
 *                          permission is granted;
 *
 *                  -s      FILE exists and has a size greater
 *                          than zero;
 *
 *                  -d      FILE exists and is a directory;
 *
 *                  -f      FILE exists and is a regular file;
 *
 *                  -c      FILE exists and is character special;
 *
 *                  -b      FILE exists and is block special.
 * ***********************************************************/
void ww_test(char *arg[])
{
    int i;
    int flag = 0;
    double n1, n2;
    int result;//0 is true ; -1 is false.

    for(i=0;arg[i]!=NULL;i++)
    {
        if(!strcmp(arg[i],"-eq") || !strcmp(arg[i],"-ne") || !strcmp(arg[i],"-gt") \
            || !strcmp(arg[i],"-ge")|| !strcmp(arg[i],"-lt") || !strcmp(arg[i],"-le"))
            flag = 1;
        if(!strcmp(arg[i],"-e") || !strcmp(arg[i],"-r") || !strcmp(arg[i],"-w") \
            || !strcmp(arg[i],"-x")|| !strcmp(arg[i],"-s") || !strcmp(arg[i],"-d") \
            || !strcmp(arg[i],"-f")|| !strcmp(arg[i],"-c") || !strcmp(arg[i],"-b"))
            flag = 2;
    }

    if(flag == 1)
    {
        n1 = atof(arg[1]);
        n2 = atof(arg[3]);

        if(!strcmp(arg[2], "-eq"))
        {
            if(n1 == n2)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[2], "-ne"))
        {
            if(n1 != n2)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[2], "-gt"))
        {
            if(n1 > n2)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[2], "-ge"))
        {
            if(n1 >= n2)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[2], "-lt"))
        {
            if(n1 < n2)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[2], "-le"))
        {
            if(n1 <= n2)
                result = 0;
            else
                result = -1;
        }
    }
    else if(flag == 2)
    {
        if(!strcmp(arg[1], "-e"))
        {
            if(access(arg[2],F_OK) != -1)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-r"))
        {
            if(access(arg[2],R_OK) != -1)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-w"))
        {
            if(access(arg[2],W_OK) != -1)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-x"))
        {
            if(access(arg[2],X_OK) != -1)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-s"))
        {
            FILE *fp;
            char ch;

            fp = fopen(arg[2], "r");
            ch = fgetc(fp);
            if(ch != EOF)
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-d"))
        {
            struct stat buf;
            
            lstat(arg[2], &buf);
            if(S_ISDIR(buf.st_mode))
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-f"))
        {
            struct stat buf;
            
            lstat(arg[2], &buf);
            if(S_ISREG(buf.st_mode))
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-c"))
        {
            struct stat buf;
            
            lstat(arg[2], &buf);
            if(S_ISCHR(buf.st_mode))
                result = 0;
            else
                result = -1;
        }
        else if(!strcmp(arg[1], "-b"))
        {
            struct stat buf;
           
            lstat(arg[2], &buf);
            if(S_ISBLK(buf.st_mode))
                result = 0;
            else
                result = -1;
        }
    }
    else
    {
        printf("Input format error\n");
        return;
    }
    if(result == 0)
        printf("TRUE\n");
    else
        printf("FALSE\n");
}
