#include <stdio.h>

int main(int argc, char **argv)
{
    char input[80] = {0};

    for(int i = 0; i < 80; ++i)
    {
        char c = getchar();
        if (c == '\n') break;
        input[i] = c;
    }

    printf("%s\n", input);

    return 0;
}
