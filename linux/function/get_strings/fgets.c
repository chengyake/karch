#include <stdio.h>  
int main ( ) {  
  
   char name[20];  
  
   printf("\n 输入任意字符 ： ");  
  
   fgets(name, 20, stdin);//stdin 意思是键盘输入  
  
   fputs(name, stdout); //stdout 输出  
  
   return 0;  
}  
