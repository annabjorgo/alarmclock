#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
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
    printf("awesome alarmclock!!\n");
    printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel) or 'x' (exit) \n");
    scanf("%[^\n]%*c", &menu_select);
    // printf("%c\n", menu_select);

    char string[LEN] = {0};

    if (menu_select == 's')
    {

        int time;
        printf("Schedule alarm at which date and time? ");
        scanf("%s", &string);
        // parse something
        printf("\nScheduling alarm in %s seconds\n", string);
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