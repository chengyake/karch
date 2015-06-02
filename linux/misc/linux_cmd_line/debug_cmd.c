#include <time.h>
#include <stdio.h>

/*
*   time
*
*/
int get_system_time() {
    time_t now;     
    struct tm *timenow;    
    time(&now);   

    timenow = localtime(&now);   
    printf("%s/n",asctime(timenow));   
    return 0;
}

int set_system_time(char *dt) {
    int ret;
    char time_cmd[32];
    char date_cmd[32];
    char year[3]={0};
    char mon[3]={0};
    char day[3]={0};
    char hour[3]={0};
    char min[3]={0};
    char sec[3]={0};


    sscanf(dt, "%2s-%2s-%2s|%2s-%2s-%2s", 
            year, mon, day, hour, min, sec);

 
    sprintf(time_cmd, "date -s %2s:%2s:%2s", hour, min, sec);
    sprintf(date_cmd, "date -s %2s/%2s/%2s", mon, day, year);
    //ret = system(time_cmd);
    if(ret<0) {
    	return ret;
    }
    //ret = system(date_cmd);
    if(ret<0) {
    	return ret;
    }

    //ret = system("hwclock  -w");  
    printf("%s\n", time_cmd);
    printf("%s\n", date_cmd);
    return ret;
}

void time_usage() {
    printf("Get or Set system time\n");
    printf("\n");
    printf("Usage:\n");
    printf("time [year-mon-day|hour-min-sec]\n");
    printf("\n");
    printf("for example:\n");
    printf("\t>>time\n");
    printf("\tget local time\n");
    printf("\n");
    printf("\t>>time 15-06-01|12-45-00\n");
    printf("\tset local time year:2015 mon:06 day:01 hour:12 min:24 sec:00\n");
}

int time_func(char *arg1, char *arg2, char *arg3, char *arg4) {
	if(arg1 == NULL){
        get_system_time();
    } else {
        set_system_time(arg1);
    }
}

