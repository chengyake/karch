###gets

* 头文件:
    stdio.h(c中)，c++不需包含此头文件

* 原型：
    char *gets( char *buffer );

* 说明:
    不建议使用，API已经过期
    /tmp/ccumlB0V.o: In function `main':
    gets.c:(.text+0x40): warning: the `gets' function is dangerous and should not be used.
    可以接收空格，它以回车作为结束符，自动在后面加上‘\0'作为结束符。表示空字符串。

* 示例：

            #include <stdio.h>
            
            int main() 
            {
                char test[128]={0};
            
                gets(&test[0]);
            
                printf("%s", test);
            
            
            }



###fgets


* 头文件:
    stdio.h(c中)，c++不需包含此头文件

* 原型：
    char *fgets( char *buffer );

* 说明:
    建议使用，replace gets()
    可以接收空格，它以回车作为结束符，自动在后面加上‘\n'作为结束符。表示空字符串。

* 示例：

            #include <stdio.h>  
            int main () 
            {  
              
               char name[20];  
              
               printf("\n 输入任意字符 ： ");  
              
               fgets(name, 20, stdin);//stdin 意思是键盘输入  
              
               fputs(name, stdout); //stdout 输出  
              
               return 0;  
            }  





###cin.getline

cin.getline(数组名，接收长度)。也可以接收空格，以回车作为结束符。
示例：

char a[100];

cin.getline(a,100);

接收字符串到a数组，实际最大接收长度为99位，最后一位保存字符串结束符‘\0’。所以在用它接收字符串时，要预设的长一点，不然会对后面的cin、cin.getline等函数产生莫名其妙的影响（后面接收不到）。


###scanf

* 格式：scanf(格式控制，地址表列)

    scanf接收字符串时，以空格、回车、Tab等作为结束符。

    示例：

    char a[100];

    scanf("%s",a);

