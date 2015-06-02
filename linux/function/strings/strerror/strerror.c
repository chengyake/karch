#include <stdio.h>
#include <string.h>


int main()
{
    int i=0;
    for(i=-3; i<135; i++) {

        printf("%d: %s\n", i, strerror(i));
    }
}
