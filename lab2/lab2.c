/*2.         Время в Калифорнии
Измените программу ex_time.c, чтобы она выводила дату и время в Калифорнии (Pacific Standard Time, PST). 
Подсказка: Если время UTC 20 часов, то в Калифорнии 12 часов.*/

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char* tzname[];

int main()
{
	time_t now;
	struct tm* sp;

	setenv("TZ", "PST8PST", 1);
	tzset();

	(void)time(&now);
	printf("%s", ctime(&now));
	
	sp = localtime(&now);
	printf("%d/%d/%02d %d:%02d %s\n",
	sp->tm_mon + 1, sp->tm_mday,
	sp->tm_year, sp->tm_hour,
	sp->tm_min, tzname[sp->tm_isdst]);
	
	return 0;
}
