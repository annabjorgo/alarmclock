#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include<sys/wait.h>
#define LEN 256
#define SIZE 20
#define NUMBER_OF_ALARMTONES 10
#define ALARM_TONE_LENGTH 100

char menu_select;
unsigned int alarm_count;
int total_alarmtones = 1;

typedef struct alarm_t
{
    time_t time;
    pid_t pid;
} alarm_t;

struct alarm_t alarms[SIZE];
char alarmtones_array[NUMBER_OF_ALARMTONES][ALARM_TONE_LENGTH];

//for storing text-based alarm tones in array
void read_alarmtones_file() {
    FILE* file;
    char buf[1000];
    int i = 0;
    int total = 0;

    file = fopen("alarmtones.txt", "r");

    while(fgets(alarmtones_array[i], ALARM_TONE_LENGTH, file))
    {
        alarmtones_array[i][strlen(alarmtones_array[i]) - 1] = '\0';
        i++;

    }
    total_alarmtones = i;
    int fclose( FILE *alarmtones);
}


void alarm_ring()
{
    //using rand() to generate a random text-based alarmtone
    srand(time(NULL));
    int random = rand() % total_alarmtones;

    printf("%s\n", alarmtones_array[random]);

    // Sound wont work because of WSL
    // execlp("mpg123", "mpg123", "-q", "./alarm.mp3", NULL);
}

unsigned int alarm_diff_time(time_t timestamp)
{
    time_t now = time(NULL);
    int diff = (int)difftime(timestamp, now);
    return diff;
}

unsigned int fork_alarm(time_t timestamp, int diff_time)
{
    pid_t pid = fork(); 

    if (pid != 0)
    {
        return pid;
    }

    else
    {  
        printf("Scheduling alarm in %d seconds\n", diff_time);
        sleep(diff_time);
        alarm_ring();
        exit(EXIT_SUCCESS);
    }
}

void welcome_message()
{
    time_t now = time(NULL);
    struct tm *time = localtime(&now);
    char s[100];
    strftime(s, 100, "%Y-%m-%d %X", time);
    printf("Welcome to the alarm clock! It is currently %s\n", s);
    printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel) or 'x' (exit) \n");
}

void schedule_alarm(char alarmInput[LEN])
{
    // REMEMBER TO ADD FUNC FOR FULL LIST (<20 elements)

    char buf[255];
    printf("Schedule alarm at which date and time? Format:YYYY-MM-DD hh:mm:ss ");
    scanf("%[^\n]%*c", alarmInput);
    struct tm result;
    result.tm_isdst = -1;
    strptime(alarmInput, "%Y-%m-%d %H:%M:%S", &result);
    
    time_t resultTime = mktime(&result);

    int diff_time = alarm_diff_time(resultTime);
    if (diff_time < 0)
    {
        printf("Couldn't schedule this alarm. Make sure to enter a time that hasn't passed\n"
        "Hit 's' to schedule a new alarm\n");
    }
    else {
        // call function that forks and creates a child process for the alarm
        pid_t pid = fork_alarm(resultTime, diff_time);

        alarm_t new_alarm;
        new_alarm.time = resultTime;
        new_alarm.pid = pid;

        // put newly constructed alarm in array
        unsigned int alarm_id = alarm_count;
        alarms[alarm_id] = new_alarm;
        alarm_count += 1;
    }
    
}

void list_alarms()
{
    bool no_alarms = true;
    printf("Scheduled alarms:\n");
    for (int i = 0; i < SIZE; i++)
    {
        if (alarms[i].pid != 0)
        {
            no_alarms = false;
            time_t alarm_time = alarms[i].time;
            struct tm *time = localtime(&alarm_time);
            char s[100];
            strftime(s, 100, "%Y-%m-%d %X", time);
            printf("Alarm %d at %s\n", i + 1, s);
        }
    }
    if (no_alarms) {
        printf("Couldn't find any scheduled alarms:(\n");
    }
}

void kill_alarm(int alarm_id)
{
    alarm_t cancelling_alarm = alarms[alarm_id];
    kill(cancelling_alarm.pid,SIGKILL);
}

void cancel_alarm(char alarmInput[LEN])
{
    printf("Cancel which alarm? ");
    int num;
    char term;
    if (scanf("%d%c", &num, &term) != 2 || term != '\n' || alarms[num-1].pid == 0)
    {
        printf("Unvalid number or the alarm does not exists\n");
    }

    else
    {
        //killing the child process 
        kill_alarm(num-1);

        //deleting alarm from array
        alarm_count--;
        printf("Remaining alarms: %d\n", alarm_count);
        for (int i = num - 1; i < SIZE - 1; i++)
        {
            alarms[i] = alarms[i + 1];     
        }
    }
}

void terminate_program()
{
    printf("Goodbye!\n");
    for (int i = 0; i < SIZE - 1; i++)
    {
        if (alarms[i].pid != 0) 
        {
            kill_alarm(i);
        }
    }
    exit(EXIT_SUCCESS);
}


void menuFunc()
{
    scanf("%[^\n]%*c", &menu_select);
    char alarmInput[LEN] = {0};

    if (menu_select == 's')
    {
        schedule_alarm(alarmInput);
    }

    if (menu_select == 'l')
    {
        list_alarms();
    }
    if (menu_select == 'c')
    {
        cancel_alarm(alarmInput);
    }
    if (menu_select == 'x')
    {
        terminate_program();
    }
    //this is not working as it should
    //if (menu_select != 'x' || menu_select != 's' || menu_select != 'l' ||menu_select != 'c') 
    //{
    //  printf("Input not supported. Please enter s,l,c or x\n");
    //}
    
}

int main()
{
    read_alarmtones_file();
    welcome_message();
    while (1)
    {
        menuFunc();
    }
    return 0;
}