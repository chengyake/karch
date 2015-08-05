#include <time.h>
#include <stddef.h>
#include <stdio.h>

int main() {
    char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    time_t timep;
    struct tm *p;
    time(&timep);
    p=localtime(&timep);
    printf("%4d%02d%02d_%02d%02d.gsmr",(1900+p->tm_year), p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min);
}
