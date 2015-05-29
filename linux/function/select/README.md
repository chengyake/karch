多路复用之select
=============

系统提供select函数来实现多路复用输入输出模型。
select系统调用是用来让我们的程序监视多个文件句柄的状态变化的。程序会停在select这里等待，直到被监视的文件句柄有一个或多个发生了状态改变。


* 头文件

		#include <sys/select.h>

* 函数原型
  
		int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,struct timeval *timeout);

* 参数说明
	* maxfd：是需要监视的最大的文件描述符值+1；
	* rdset,wrset,exset：分别对应于需要检测的可读、可写、异常文件描述符的集合；
	* struct timeval：超时时，如需要监视的描述符没有事件发生则函数返回0

* 返回值
    * success, return num;
    * timeout, return 0;
    * error, return -1. 错误原因存于errno，此时参数readfds，writefds，exceptfds和timeout的值变成不可预测。
    错误值可能为：
        * EBADF 文件描述词为无效的或该文件已关闭
        * EINTR 此调用被信号所中断
        * EINVAL 参数n 为负值。
        * ENOMEM 核心内存不足




