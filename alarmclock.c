#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#define LEN 256

char menu_select;
unsigned int alarm_count;

typedef struct alarm_t
{
    time_t time;
    unsigned int pid;
} alarm_t;

struct alarm_t alarmArray[20];

void alarm_ring()
{
    printf("Ring ring bitch");
    // execlp("mpg123", "mpg123", "-q", "./alarm.mp3", 0);
}

unsigned int fork_alarm(time_t timestamp)
{
    unsigned int pid = fork();

    if (pid != 0)
    {
        return pid;
    }

    else
    {
        time_t now = time(NULL);

        int diff = (int)difftime(timestamp, now);

        if (diff < 0)
        {
            // error handling
            exit(1);
        }
        else
        {
            printf("%d", diff);
            sleep(diff);
            alarm_ring();
            exit(0);
        }
    }
}

void menuFunc()
{
    time_t now = time(NULL);
    struct tm *time = localtime(&now);
    char s[100];
    strftime(s, 100, "%Y-%m-%d %X", time);
    printf("Welcome to the alarm clock! It is currently %s", s);
    printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel) or 'x' (exit) \n");
    scanf("%[^\n]%*c", &menu_select);

    char alarmInput[LEN] = {0};
    if (menu_select == 's')
    {
        char buf[255];
        printf("Schedule alarm at which date and time? ");
        scanf("%[^\n]%*c", alarmInput);
        struct tm result;
        strptime(alarmInput, "%Y-%m-%d %H:%M:%S", &result);
        
        
        // convert from tm struct to time_t variable
        time_t resultTime = mktime(&result);
        //print ut tid 


        // call function that forks and creates a child process for the alarm
        unsigned int pid = fork_alarm(resultTime);

        alarm_t new_alarm;
        new_alarm.time = resultTime;
        new_alarm.pid = pid;

        // put newly constructed alarm in array
        unsigned int alarm_id = alarm_count;
        alarmArray[alarm_id] = new_alarm;
        alarm_count += 1;
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
        // have to cancel all the alarms in the list
        printf("exit\n");
        void exit(int status);
    }
    // printf("%c\n", menu_select);
}

int main()
{
    alarm_t alarm_object;
    menuFunc();
    return 0;
}