#include <string.h>
#include <stdio.h>

int main(void)
{
    char buffer[]="Fred,John,Ann";
    char *p;
    char *buf = buffer;
    while((p=strtok(buf,","))!=NULL) 
    {
        buf=NULL;
        printf("%s\n", p);
    }
    return 0;
}
