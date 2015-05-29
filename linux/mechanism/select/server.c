#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#define SERV_PORT 5555
#define SERV_IP "127.0.0.1"
#define LISTENQ 1024
#define MAXLINE 1024

int count = 0;

int read_msg(int fd)
{
    char tmp[20];
    bzero(tmp, sizeof(20));
   
    int n = read(fd, tmp, sizeof(tmp));
    if(n==-1)
    {
        perror("read");
        return -1;
    }
    else if(n==0)
    {
        printf("client close");
        return 0;
    }

    printf("recv from [%d] [%s]\n", fd, tmp);

    count = atoi(tmp) + 1;

    return n;
}

int write_msg(int fd)
{
    char tmp[20];
    bzero(tmp, sizeof(20));

    sprintf(tmp , "%d", count);

    write( fd, tmp, sizeof(tmp));

    printf("send to [%d] [%s]\n", fd, tmp);

    return 0;
}

int procmsg(int fd, int (*callback)(int))
{
    return callback(fd);   
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    int clilen;
    char cliip[24];
    int opt = 1;
    int opt_len = sizeof(opt);
    int fd;
    int ret;
    fd_set rset, allset;

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0))==-1)
      perror("socket");

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, opt_len);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr);

    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))==-1)
      perror("bind");

    if(listen( listenfd, LISTENQ)==-1)
      perror("listen");

    printf("server listening [%s] [%d]\n", SERV_IP, SERV_PORT);

    int maxfd = listenfd;

    struct timeval tv;

    int nreadys = 0;
    int cliaddr_len = sizeof(cliaddr);
    int client[FD_SETSIZE];
    int i;
    int maxi=0;
    char buf[MAXLINE];
    int nready;
   
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for( i=0; i<FD_SETSIZE; i++)
        client[i]=-1;

    bzero(cliip, sizeof(cliip));

    for(;;)
    {
        FD_ZERO(&rset);

        rset = allset;

        tv.tv_sec=5;
        tv.tv_usec=0;

        if((nreadys = select( maxfd+1, &rset, NULL, NULL, &tv))==-1)
            perror("select()");
        else if(nreadys==0)
        {
            printf("No data within five seconds.\n");
            continue;
        }

        //新连接
        if(FD_ISSET(listenfd, &rset))
        {
            printf("maxfd->[%d] nreadys->[%d]\n", maxfd, nreadys);

            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

            for(i=0; i<FD_SETSIZE; i++)
            {
                if(client[i]==-1)
                {
                     client[i]=connfd;
                     if(i > maxi)
                        maxi = i;
                     break;
                }
                if(i==FD_SETSIZE)
                {
                    printf("too many connections.\n");
                    return -1;
                }
            }

            if(connfd > maxfd)
                  maxfd = connfd;

            FD_SET(connfd, &allset);

            bzero(cliip, sizeof(cliip));

            inet_ntop(AF_INET, &cliaddr.sin_addr,  cliip, sizeof(cliip));

            printf("New connection [%s] [%d]\n", cliip, ntohs(cliaddr.sin_port));

            if(--nready <= 0)
                continue;
        }
        //老连接
        else
        {
            //遍历连接
            for( i=0; i< maxi+1; i++)
            {
                fd = client[i];
                if(fd < 0)
                    continue;

                //如果是socket可读
                if(FD_ISSET(fd, &rset))
                {
                    bzero(buf, sizeof(buf));
                    //对方已关闭
                    if((ret = procmsg(fd, read_msg))==0)
                    {
                        client[i]=-1;
                        printf("socket->[%d] closed.\n", fd);
                        FD_CLR(fd, &allset);
                        close(fd);
                        printf("close client[%d]->[%d]\n", i, fd);
                    }
                    if(ret!=0)
                        procmsg(fd, write_msg);
                }
                if(--nready <= 0)
                  break;
            }
        }
    }
    for(i=0; i< maxi; i++)
      close(client[i]);

    return 0;   
}

