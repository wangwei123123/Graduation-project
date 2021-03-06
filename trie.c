/*
 * ============================================================
 *       Filename:  trie.c
 *    Description:  The operation of the dictionary tree, using
 *                  the dictionary tree to optimize the process
 *                  of automating history commands and accessing 
 *                  and storing history commands.
 *        Version:  1.0
 *        Created:  2018.12.4
 *         Author:  ww
 *        Purpose:  Graduation project
 * ============================================================
 */
#include "ww_shell.h"
//#define DEBUG

Tree root;
Com_list commands;
Com_list *rear;
int command_num;
int history_num;

/**************************************************************
 *
 * Function name : store_command
 * Description : Store the commands entered by the current user
 *               into memory so that they are dumped to the
 *               hard disk when the program finishes running.
 * Parameter : 
 *              @str    User-entered command. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void store_command(char *str)
{
    Com_list *cur;
    
    cur = (Com_list*)(malloc(sizeof(Com_list)));
    strcpy(cur->command, str);
    cur->next = NULL;
    
    rear->next = cur;
    rear = cur;
    
    command_num++;
}

/**************************************************************
 *
 * Function name : match_history
 * Description : Find the matching string in trie according to
 *               the string that the user has entered,and store
 *               it in the specified address.
 * Parameter : 
 *              @input  The string that the user has entered.
 *              @history_input  The address to store the matched
 *                              string.
 * Return :
 *              -1 : Match failed.
 *               0 : Match successfully.
 *
 * Other : NULL
 * ***********************************************************/
int match_history(char *input, char *history_input)
{
    int i;
    Tree *cur;
    int max;
    char max_ch[2]={'\0'};

    cur = &root;
    for(i=0;input[i]!='\0';i++)
    {
        if(cur->next[ASCII_SUB_BASE(input[i])] == NULL)
        {    
            return -1;
        }
        cur = cur->next[ASCII_SUB_BASE(input[i])];
    }
    strcpy(history_input, input);

    max = find_max_next(cur);
    if(max == -1)
    {  
        return -1;
    }
    while(max != -1)
    {
        max_ch[0] = ASCII_ADD_BASE(max);
        strcat(history_input, max_ch);
        cur = cur->next[max];
        max = find_max_next(cur);
    }
    return 0;
}

/**************************************************************
 *
 * Function name : find_max_next
 * Description : Find the next most weighted character on the
 *               specified node.
 * Parameter : 
 *              @cur   The node of trie that stores history
 *                     command. 
 * Return :
 *              -1 : After the node is all NULL.
 *           other : Find success and return its index.
 * Other : NULL
 * ***********************************************************/
int find_max_next(Tree *cur)
{
    int i;
    int flag = -1;
    
    for(i=0;i<ASCII_CHAR;i++)
    {
        if(cur->next[i]!=NULL)
        {
            if(flag == -1)
                flag = i;
            else
                flag = (cur->next[i]->weight) > (cur->next[flag]->weight) ? i : flag;
        }
    }
    return flag;
}

/**************************************************************
 *
 * Function name : traverse_tree
 * Description : Traversing the trie that stores history
 *               commands to debug.
 * Parameter : 
 *              @cur   The node of trie that stores history
 *                     command. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
#ifdef DEBUG
void traverse_tree(Tree *cur)
{
    static char command[MAX_CMD_LEN];
    static int pos = 0;
    int i;

    if(cur == NULL)
        return;
    if(cur->weight)
    {
        command[pos] = '\0';
        printf("%s:%d\n", command, cur->weight);
    }
    for(i=0;i<ASCII_CHAR;i++)
    {
        command[pos++] = ASCII_ADD_BASE(i);
        traverse_tree(cur->next[i]);
        pos--;
    }
}
#endif

/**************************************************************
 *
 * Function name : insert_tree
 * Description : insert command into the trie that stores history.
 * Parameter : 
 *              @str    User-entered command. 
 * Return : NULL
 * Other : NULL
 * ***********************************************************/
void insert_tree(char *str)
{
    int i;
    Tree *cur;

    cur = &root;
    for(i=0;str[i]!='\0';i++)
    {
        if(cur->next[ASCII_SUB_BASE(str[i])] == NULL)
        {
            Tree *new = (Tree*)malloc(sizeof(Tree));
            memset(new, 0, sizeof(Tree));
            new->weight = 1;
            cur->next[ASCII_SUB_BASE(str[i])] = new;
        }
        else
            ((cur->next[ASCII_SUB_BASE(str[i])])->weight)++;
        cur = cur->next[ASCII_SUB_BASE(str[i])];
    }
}

#ifdef DEBUG
void main()
{   
    char s[]="vim builtin.c";
    char his[100];
    FILE *fp;
    char history_command[MAX_CMD_LEN];
    Com_list *cur;

    memset(&root, 0, sizeof(Tree));
    memset(&commands, 0, sizeof(Com_list));
    commands.next = NULL;
    rear = &commands;
    /*
    fp = fopen(COMMAND_HISTORY, "r");
    fgets(history_command, MAX_CMD_LEN, fp);
    while(!feof(fp))
    {
        insert_tree(history_command);
        fgets(history_command, MAX_CMD_LEN, fp);
    }
    
    if(match_history("sudo ", his) == 0)
        printf("match success :%s\n", his);
    else
        printf("failed\n");
    */
    store_command("asdfg");
    store_command("qwert");
    store_command("trytu");
    store_command("zxvbgv");
    
    cur = commands.next;
    printf("have %d command\n", command_num);
    while(cur)
    {
        printf("%s\n", cur->command);
        cur = cur->next;
    }
}
#endif
