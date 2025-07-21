#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

bool is_valid_number(char *str)
{
    int size = strlen(str);
    if(size != 1)
        return false;
    else if(str[0] < '0' || str[0] > '9')
        return false;
    return true;
}

void print_res(char *msg)
{
    printf("Content-Type:text/html\n");
    printf("<TITLE>");
    printf("%s", msg);
    printf("</TITLE>");
}

int main(int argc, char **argv)
{
    char *env_var = getenv("number");
    if(!is_valid_number(env_var))
        print_res("Oh, you're a smartass! Now die.");

    srand(time(NULL));
    unsigned int guessed = atoi(env_var);
    unsigned int number = rand() % 10;

    if(guessed != number)
        print_res("You live !");
    else
        print_res("You die !");
}
