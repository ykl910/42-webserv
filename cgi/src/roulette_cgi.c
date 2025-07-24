#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

bool is_valid_number(char *str)
{
    if(!str)
        return false;
    int size = strlen(str);
    if(size != 1)
        return false;
    else if(str[0] < '0' || str[0] > '9')
        return false;
    return true;
}

char *extract_value(char *env_var)
{
    char *eq = strchr(env_var, '=');
    if(!eq || *(eq + 1) == '\0')
        return NULL;
    return eq + 1;
}

int main(void)
{
    char *env_var = getenv("QUERY_STRING");
    if(!env_var)
    {
        printf("Oh, you're a smartass! Now die.");
        return 0;
    }
    char *value = extract_value(env_var);
    if(!is_valid_number(value))
    {
        printf("Oh, you're a smartass! Now die.");
        return 0;
    }
    srand(time(NULL));
    unsigned int guessed = atoi(value);
    unsigned int number = rand() % 2;
    if(guessed != number)
        printf("You live !");
    else
        printf("You die !");
    return 0;
}
