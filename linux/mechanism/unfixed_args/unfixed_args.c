#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int sumi(int c, ...)
{
    va_list argptr;
    va_start(argptr, c); //初始化变元指针
    int sum = c;
    c = va_arg(argptr, int); //作为下一个参数类型的参数类型，返回不定参数
    //c保存第一个不定参数
    printf("%d\n", c);
    while(0 != c) //末尾特定字符指示结束
    {
        sum = sum + c;
        c = va_arg(argptr, int);
    }
    va_end(argptr);
    return sum;
}

double sum_series(int num, ...)
{
    double sum = 0.0, t;
    va_list argptr;
    va_start(argptr, num);//初始化变元指针
    while (num--)
    {
        t = va_arg(argptr, double);
        sum = sum + t;
    }
    va_end(argptr);
    return sum;
}

void get_strings(char * first, ...)
{
	char *t;
    va_list argptr;
    va_start(argptr, first);//初始化变元指针

    //t = va_arg(argptr, char *);
    //if(t!= NULL)printf("%s\n", t);

    va_end(argptr);
}

int main()
{
    int i = sumi(1,2,3,4,5,6,7,8,9,0);
    printf("%d\n", i);
    double d = sum_series(4, 1/3.0, 1/4.0, 1/5.0, 1/6.0);
    printf("%f\n", d);

    get_strings("yake", "good");
    //get_strings("yake");
    //get_strings();

    return 0;
}

