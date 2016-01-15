#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void set_timer(int);

void my_alarm_handler(int a) {

    printf("I'm chengyake\n");
    set_timer(0);

}




void set_timer(int a) {

    struct itimerval t;
    t.it_interval.tv_usec = 0;
    t.it_interval.tv_sec = 0;
    t.it_value.tv_usec = 0;
    t.it_value.tv_sec = a;

    if(setitimer(ITIMER_REAL, &t, NULL) < 0) {
        printf("set timer error\n");
        return;
    }

    signal(SIGALRM, my_alarm_handler);

}


int main() {

    set_timer(1);

    sleep(10);  //interrupt by signal

    sleep(10);

    set_timer(5);

    sleep(10);  //interrupt by signal

    sleep(100);
    return 0;

}



