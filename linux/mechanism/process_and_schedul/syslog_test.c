#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<syslog.h>



int main() {

	openlog("demo_update",LOG_PID|LOG_PERROR, LOG_USER);
    syslog(LOG_INFO, "%s\n", "log info");
    syslog(LOG_ERR, "ok ?\n");
    syslog(LOG_EMERG, "chengyake %s\n", "log emerg");
    closelog();

}
