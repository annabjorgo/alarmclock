#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define LEN 256

char menu_select;

struct alarm
{
    time_t time;
    int PID;
};

struct alarm alarmArray[20];

void menuFunc()
{
    time_t now = time(NULL);
    struct tm *time = localtime(&now); //to get a nice string of time
    char s[100];
    strftime(s, 100,"%Y-%m-%d %X", time);
    printf("Welcome to the alarm clock! It is currently %s", s);
    printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel) or 'x' (exit) \n");
    scanf("%[^\n]%*c", &menu_select);
    // printf("%c\n", menu_select);

    char alarmInput[LEN] = {0};
    if (menu_select == 's')
    {
        char buf[255];
        printf("Schedule alarm at which date and time? ");
        scanf("%[^\n]%*c", alarmInput);
        struct tm result;
        strptime(alarmInput, "%Y-%m-%d %H:%M:%S", &result);
        strftime(buf, sizeof(buf), "%d %b %Y %H:%M", &result);
        puts(buf);
        

        //strptime(string, "%Y-%m-%d %X", &result);
        //strftime(buf, sizeof(buf), "%Y-%m-%d %X", &result);
        //printf("%s", buf);
        
        
        
    }
    if (menu_select == 'l')
    {
    }
    if (menu_select == 'c')
    {
        printf("c was selected\n");
    }
    if (menu_select == 'x')
    {
        printf("exit\n");
        void exit(int status);
    }
    // printf("%c\n", menu_select);
}

int main()
{
    menuFunc();
    return 0;
}