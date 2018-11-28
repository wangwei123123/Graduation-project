/*
 * ============================================================
 *       Filename:  parse.c
 *    Description:  split user input 
 *        Version:  1.0
 *        Created:  2018.11.16
 *         Author:  ww
 *        Purpose:  Graduation project
 * ============================================================
 */
#include "ww_shell.h"
//#define DEBUG

//extern char *arg[MAX_AGR];

void ww_parse(char *command, char *arg[])
{
    char *start = command;
    int i = 1;
    arg[0] = start;
    while(*start)
    {
        if(isspace(*start))
        {
            *start = 0;
            start++;
            arg[i++] = start;
        }
        else
            ++start;
    }
    arg[i]=NULL;
}

#ifdef DEBUG
void main()
{
    char *s = readline("input");
    char *arg[MAX_ARG];
    ww_parse(s, arg);
    
    int m = 0;
    //input "aa bb cc dd ee"
    for(m=0;m<5;++m)
        printf("debug:%s\n",arg[m]);
}
#endif
