#include <stdio.h>  
#include <unistd.h>  

int main(int argc, char *argv[])
{
    int ch;
    opterr = 0;  
    while ((ch = getopt(argc, argv, "a:bcde"))!=-1) {
        switch(ch) {
            case 'a':  
                printf("option a:'%s'\n",optarg);
                break;  
            case 'b':  
                printf("option b :b\n");  
                break;  
            default:  
                printf("other option :%c\n",ch);  
        }  
    }  
    printf("optopt +%c\n", optopt);  
}




#if 0

相关函数表头文件
        #include<unistd.h>
定义函数
        int getopt(int argc,char * const argv[ ],const char * optstring);
函数说明
        getopt()用来分析命令行参数。参数argc和argv是由main()传递的参数个数和内容。参数optstring 则代表欲处理的选项字符串。此函数会返回在argv 中下一个的选项字母，此字母会对应参数optstring 中的字母。如果选项字符串里的字母后接着冒号“:”，则表示还有相关的参数，全域变量optarg 即会指向此额外参数。如果getopt()找不到符合的参数则会印出错信息，并将全域变量optopt设为“?”字符，如果不希望getopt()印出错信息，则只要将全域变量opterr设为0即可。
 
短参数的定义
       getopt()使用optstring所指的字串作为短参数列表，象"1ac:d::"就是一个短参数列表。短参数的定义是一个'-'后面跟一个字母或数字，象-a, -b就是一个短参数。每个数字或字母定义一个参数。 
　　其中短参数在getopt定义里分为三种：
　　1. 不带值的参数，它的定义即是参数本身。
　　2. 必须带值的参数，它的定义是在参数本身后面再加一个冒号。
　　3. 可选值的参数，它的定义是在参数本身后面加两个冒号 。
　　在这里拿上面的"1ac:d::"作为样例进行说明，其中的1,a就是不带值的参数，c是必须带值的参数，d是可选值的参数。
　　在实际调用中，'-1 -a -c cvalue -d', '-1 -a -c cvalue -ddvalue', '-1a -ddvalue -c cvalue'都是合法的。这里需要注意三点：
　　1. 不带值的参数可以连写，象1和a是不带值的参数，它们可以-1 -a分开写，也可以-1a或-a1连写。
　　2. 参数不分先后顺序，'-1a -c cvalue -ddvalue'和'-d -c cvalue -a1'的解析结果是一样的。
　　3. 要注意可选值的参数的值与参数之间不能有空格，必须写成-ddvalue这样的格式，如果写成-d dvalue这样的格式就会解析错误。

返回值
　  getopt()每次调用会逐次返回命令行传入的参数。
　  当没有参数的最后的一次调用时，getopt()将返回-1。
　   当解析到一个不在optstring里面的参数，或者一个必选值参数不带值时，返回'?'。
　  当optstring是以':'开头时，缺值参数的情况下会返回':'，而不是'?' 。

#endif
