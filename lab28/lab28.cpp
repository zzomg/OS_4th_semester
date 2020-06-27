#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <string>

using namespace std;

void remove_nl(char* str)
{
    for (int i = 0; i < strlen(str); ++i)
    {
        if (str[i] == '\n') {
            str[i] = '\0';
        }
    }
}

int main()
{
    FILE *fptrs[2];
    int i, pid;

    char buf[4];

    p2open("/bin/sort -n", fptrs);

    srand(50);

    int digit;
    for(i = 0; i < 100; i++) {
        digit = 1 + (int)(99.0 * rand() / (RAND_MAX + 1.0));
        fputs((to_string(digit)+"\n").c_str(), fptrs[0]);
    }
    fclose(fptrs[0]);

    i = 0;
    while(fgets(buf, 4, fptrs[1]) != NULL) {
        remove_nl(buf);
        if (i % 10 == 0) {
            printf("\n");
        }
        printf("%s ", buf);
        ++i;
    }
    printf("\n");

    fclose(fptrs[1]);

    return 0;
}
