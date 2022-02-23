#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#define LEN 256
#define NUMBER_OF_ALARMS 20
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

struct alarm_t alarms[NUMBER_OF_ALARMS];
char alarmtones_array[NUMBER_OF_ALARMTONES][ALARM_TONE_LENGTH];

void update_list(int num)
{
    alarm_count--;
    for (int i = num - 1; i < NUMBER_OF_ALARMS - 1; i++)
    {
        alarms[i] = alarms[i + 1];
    }
}

void remove_passed_alarms()
{
    for (int i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if (alarms[i].pid != 0 && alarms[i].time < time(NULL))
        {
            update_list(i);
        }
    }
}


// for storing text-based alarm tones in array
void read_alarmtones_file()
{
    FILE *file;
    char buf[1000];
    int i = 0;
    int total = 0;

    file = fopen("alarmtones.txt", "r");

    while (fgets(alarmtones_array[i], ALARM_TONE_LENGTH, file))
    {
        alarmtones_array[i][strlen(alarmtones_array[i]) - 1] = '\0';
        i++;
    }
    total_alarmtones = i;
    int fclose(FILE * alarmtones);
}

void alarm_ring()
{
    // using rand() to generate a random text-based alarmtone
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
        printf("\nCouldn't schedule this alarm. Make sure to enter a time that hasn't passed\n"
               "Hit 's' to schedule a new alarm\n");
    }
    else
    {
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
    remove_passed_alarms();
    bool no_alarms = true;
    printf("Scheduled alarms:\n");
    for (int i = 0; i < NUMBER_OF_ALARMS; i++)
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
    if (no_alarms)
    {
        printf("Couldn't find any scheduled alarms:(\n");
    }
}

void kill_alarm(int alarm_id)
{
    alarm_t cancelling_alarm = alarms[alarm_id];
    kill(cancelling_alarm.pid, SIGKILL);
}



void cancel_alarm(char alarmInput[LEN])
{
    printf("Cancel which alarm? ");
    int num;
    char term;
    if (scanf("%d%c", &num, &term) != 2 || term != '\n' || alarms[num - 1].pid == 0)
    {
        printf("Unvalid number or the alarm does not exists\n");
    }

    else
    {
        // killing the child process
        kill_alarm(num - 1);

        // deleting alarm from array
        update_list(num);
        printf("Remaining alarms: %d\n", alarm_count);
    }
}

void terminate_program()
{
    printf("Goodbye!\n");
    for (int i = 0; i < NUMBER_OF_ALARMS - 1; i++)
    {
        if (alarms[i].pid != 0)
        {
            kill_alarm(i);
        }
    }
    exit(EXIT_SUCCESS);
}

void catch_zombies()
{
    pid_t pid = waitpid(-1, NULL, WNOHANG);

    // If no child has terminated
    if (pid == 0)
    {
        return;
    }

    unsigned int alarm_id = -1;
    for (int i = 0; i < alarm_count; i++)
    {
        if (alarms[i].pid == pid)
        {
            alarm_id = i;
            break;
        }
    }

    if (alarm_id == -1)
    {
        return;
    }

    // Making sure to update list
    update_list(alarm_id);
}

int menuFunc()
{
    // prøvd CZ her

    scanf("%[^\n]%*c", &menu_select);
    char alarmInput[LEN] = {0};

    // Bug her -> lista oppdateres ikke dersom to alarmer ringer på rad uten input mellom
    catch_zombies();

    if (menu_select == 's')
    {
        // Prøvd CZ her
        if (alarm_count < NUMBER_OF_ALARMS)
        {
            schedule_alarm(alarmInput);
        }
        else
        {
            printf("Too many scheduled alarms.\nDelete an alarm or wait for alarm to go off\n");
        }
        return 1;
    }

    if (menu_select == 'l')
    {
        // Har prøvd å ha catch_zombies her
        list_alarms();
        return 1;
    }
    if (menu_select == 'c')
    {
        cancel_alarm(alarmInput);
        return 1;
    }
    if (menu_select == 'x')
    {
        terminate_program();
        return 1;
    }

    if (menu_select != 'x' && menu_select != 's' && menu_select != 'l' && menu_select != 'c')
    {
        // Prøvd CZ her
        printf("\nInput not supported. Please enter s,l,c or x\n");
        return 1;
    }
    return 1;

    // Prøvd CZ her
}

int main()
{
    read_alarmtones_file();
    welcome_message();
    // Har prøvd catch_zombies her
    /*
    while (1)
    {
        // Har prøvd catch_zombies her
        menuFunc();
        catch_zombies();
    }
    */
    while (menuFunc())
    {
        catch_zombies();
    }

    return EXIT_SUCCESS;
}