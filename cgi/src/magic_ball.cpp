#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <time.h>

int getSize()
{
    char *const envVar = getenv("CONTENT_LENGTH");
    if(!envVar)
        return -1;
    std::string sizeStr(envVar);
    return std::atoi(sizeStr.c_str());
}

int main()
{
    int size = getSize();
    if(size <= 0)
        exit(EXIT_FAILURE);

    char *buffer[size + 1];

    read(STDIN_FILENO, buffer, size);
    buffer[size + 1] = NULL;

    std::map<int, std::string> response;
    response[0] = "It is certain";
    response[1] = "It is decidedly so";
    response[2] = "Without a doubt";
    response[3] = "You may rely on it";
    response[4] = "As i see it, yes";
    response[5] = "Most likely";
    response[6] = "Outlook good";
    response[7] = "Yes";
    response[8] = "Signs point to yes";
    response[9] = "Reply hazy, try again";
    response[10] = "Ask again later";
    response[11] = "Better not tell you now";
    response[12] = "Cannot predict now";
    response[13] = "Concentrate and ask again";
    response[14] = "Don't count on it";
    response[15] = "My reply is no";
    response[16] = "My sources say no";
    response[17] = "Outlook not so good";
    response[18] = "Very doubtful";

    srand(time(NULL));
    unsigned int index = rand() % 19;

    printf("%s\n", response[index].c_str());
    return 0;
}
