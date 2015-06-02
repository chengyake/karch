#include <stdio.h>

int main() 
{
    char test[128]={0};

    gets(&test[0]);

    printf("%s", test);


}
