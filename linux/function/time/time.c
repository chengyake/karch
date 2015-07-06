#include <time.h>
#include <stddef.h>
#include <stdio.h>

#if 0
struct tm {  
        int tm_sec;     /* seconds after the minute - [0,59] */  
        int tm_min;     /* minutes after the hour - [0,59] */  
        int tm_hour;    /* hours since midnight - [0,23] */  
        int tm_mday;    /* day of the month - [1,31] */  
        int tm_mon;     /* months since January - [0,11] */  
        int tm_year;    /* years since 1900 */  
        int tm_wday;    /* days since Sunday - [0,6] */  
        int tm_yday;    /* days since January 1 - [0,365] */  
        int tm_isdst;   /* daylight savings time flag */  
        };  

#endif


time_t tm_to_sec(struct tm *t) {

    return t->tm_mday*24*60*60 + t->tm_hour*60*60 + t->tm_min*60 + t->tm_sec;

}



int main(void)
{

    time_t t, f; 
    struct tm *tblock;

    t = time(NULL);
    sleep(10);
    f = time(NULL);

    f=f-t;



    printf("Local time is: %ld\n", f);


    return 0;

}
