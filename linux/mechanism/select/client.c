#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>

#define SERV_PORT 5555
#define SERV_IP "127.0.0.1"

int count = 0;

void write_msg(int fd)
{
    char tmp[20];
    bzero(tmp, sizeof(tmp));
    sprintf(tmp, "%d", count);
    write(fd, tmp, strlen(tmp));
    printf("send->[%s]\n", tmp);
}

void read_msg(int fd)
{
    char tmp[20];
    bzero(tmp, sizeof(tmp));

    if(read( fd, tmp, sizeof(tmp))==-1)
        perror("read");

    printf("recv->[%s]\n", tmp);

    count = atoi(tmp) + 1;
}

int main(int argc, char **argv)
{
    int                    sockfd;
    struct sockaddr_in    servaddr;
    char strip[24];
    int i;

    bzero(strip, sizeof(strip));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr);

    printf("connect to [%s] [%d]\n", SERV_IP, SERV_PORT);

    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))==-1)
    {
        inet_ntop( AF_INET, &servaddr.sin_addr, strip, sizeof(strip));
        printf("connect to [%s] [%d]\n", strip, ntohs(servaddr.sin_port));
        perror("connect");
    }

    for( i=0; i< 5; i++)
    {
        write_msg(sockfd);
        sleep(1);
        read_msg(sockfd);
    }
   
    close(sockfd);

    return 0;
}
