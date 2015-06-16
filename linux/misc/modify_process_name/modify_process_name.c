#include <sys/prctl.h>
#include <stdio.h>



int main() {
    sleep(15);
    prctl(PR_SET_NAME, "myname", 0, 0, 0);
    sleep(10);
    return 0;
}
