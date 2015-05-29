字符串之分割strtok()
====================


分解字符串为一组字符串


* 头文件

		#include <strings.h>

* 函数原型
        char *strtok(char *s, char *delim);

* 参数说明
    * s为要分解的字符串
    * delim为分隔符字符串,例如":"

* 返回值
    从s开头开始的一个个被分割的串。当没有被分割的串时则返回NULL。
    所有delim中包含的字符都会被滤掉，并将被滤掉的地方设为一处分割的节点。

* 例子


        #include <string.h>
        #include <stdio.h>
        
        int main(void)
        {
            char input[16] = "abc,d";
            char *p;
        
            /* strtok places a NULL terminator
               in front of the token, if found */
            p = strtok(input, ",");
            if (p)      printf("%s ", p);
        
            /* A second call to strtok using a NULL
               as the first parameter returns a pointer
               to the character following the token  */
            p = strtok(NULL, ",");
            if (p)      printf("%s ", p);
        
            return 0;
        }

