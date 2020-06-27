#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

void myRead(char* str)
{
    for (int i = 0; i < 80; ++i)
    {
        read(0, &str[i], 1);
        if(str[i] == '\0') break;
    }
}

void myToUpper(char* str)
{
    for (int i = 0; i < strlen(str); ++i)
    {
        printf("%c", toupper(str[i]));
    }
}

int main(int argc, char **argv)
{
    char input[80] = {0};

    myRead(input);
    myToUpper(input);

    return 0;
}
