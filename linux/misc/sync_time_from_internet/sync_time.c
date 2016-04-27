#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>


#define  int8      char
#define  uint8     unsigned char
#define  uint32    unsigned int
#define  ulong32   unsigned long
#define  long32    long
#define  int32     int
#define  long64    long long

#define  debug

//3600s*24h*(365days*70years+17days)
#define  From00to70 0x83aa7e80U

//#define  NTPSVR1  "163.117.202.33"        //USA
#define  NTPSVR1  "202.112.1.34"        //USA
//#define  NTPSVR1  "132.163.4.102"        //USA
#define  NTPSVR2  "132.163.135.132"      //USA
#define  NTPSVR3  "192.53.103.103"       //Germany
#define  NTPSVR4  "210.72.145.44"

#define  NTPPORT  123
typedef struct NTPPACKET
{
    uint8     li_vn_mode;
    uint8     stratum;
    uint8     poll;
    uint8     precision;
    ulong32   root_delay;
    ulong32   root_dispersion;
    int8      ref_id[4];
    ulong32   reftimestamphigh;
    ulong32   reftimestamplow;
    ulong32   oritimestamphigh;
    ulong32   oritimestamplow;
    ulong32   recvtimestamphigh;
    ulong32   recvtimestamplow;
    ulong32   trantimestamphigh;  
    ulong32   trantimestamplow;
} NTPPacket;

NTPPacket  ntppack,newpack;

//定义为long64,解决32位数的符号位问题
long64   firsttimestamp,finaltimestamp;
long64   diftime,delaytime;

void NTP_Init()
{
    bzero(&ntppack,sizeof(ntppack));
    ntppack.li_vn_mode=0x1b;//0|(3<<2)|(3<<5);
    //获取初始时间戳T1
    firsttimestamp=From00to70+time(NULL);//-8*3600;
    ntppack.oritimestamphigh=htonl(firsttimestamp);
}

int ntpdate(char *ntpserver)
{

    //  ulong32 clienttime;
    //  ulong32 diftime,firsttimestamp,finaltimestamp;
    int ret;
    fd_set  inset1;
    int32  sockfd;
    struct timeval tv, tv1;
    struct timezone tz;
    struct sockaddr_in addr;
    // printf("%d,%d,%d\n",&ntppack.li_vn_mode,&ntppack.stratum,&ntppack.poll);
    // printf("%d  %ld\n",sizeof(NTPPacket),sizeof(ntppack));

    //  printf("%ld\n",time(NULL));

    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        printf("create socket error!\n");
        return -1;
    }

    addr.sin_family=AF_INET;   //IPV4协议
    addr.sin_port =htons(NTPPORT);   //NTP专用的123端口
    addr.sin_addr.s_addr=inet_addr(ntpserver);   //校时服务器
    bzero(&(addr.sin_zero),8);   //清零

    //wait 5s
    tv.tv_sec=10; 
    tv.tv_usec=0;

    FD_ZERO(&inset1);
    FD_SET(sockfd,&inset1);

    NTP_Init();
    //发送数据请求包
    sendto(sockfd,&ntppack,sizeof(ntppack),0,(struct sockaddr *)&addr,sizeof(struct sockaddr));
    //select巡视
    ret = select(sockfd+1,&inset1,NULL,NULL,&tv);
    if(ret<=0)
    {
        printf("select %s error!\n", ntpserver);
        return -1;
    }
    else
    {
        if(FD_ISSET(sockfd,&inset1))
        {
            bzero(&newpack,sizeof(newpack));
            ret = recv(sockfd, &newpack, 48,0);
            if(ret <= sizeof(newpack))        //接收数据在newpack中。
            {
                printf("recv %s error! recv %lu bytes\n", ntpserver, sizeof(newpack));
                return -1;
            }
        }
    }
    //到达客户机时间戳T4
    finaltimestamp=time(NULL)+From00to70;//-8*3600;

    //将网络上传送的大端数据改为小端形式。
    newpack.root_delay= ntohl(newpack.root_delay);
    newpack.root_dispersion= ntohl(newpack.root_dispersion);
    newpack.reftimestamphigh=ntohl(newpack.reftimestamphigh);
    newpack.reftimestamplow= ntohl(newpack.reftimestamplow);
    newpack.oritimestamphigh= ntohl(newpack.oritimestamphigh);
    newpack.oritimestamplow= ntohl(newpack.oritimestamplow);
    newpack.recvtimestamphigh= ntohl(newpack.recvtimestamphigh);
    newpack.recvtimestamplow= ntohl(newpack.recvtimestamplow);
    newpack.trantimestamphigh= ntohl(newpack.trantimestamphigh);
    newpack.trantimestamplow= ntohl(newpack.trantimestamplow);

    //求出客户机跟服务器的时间差=((T2-T1)+(T3-T4))/2
    diftime=((newpack.recvtimestamphigh-firsttimestamp)+(newpack.trantimestamphigh-finaltimestamp))>>1;
    //求出延时
    delaytime=((newpack.recvtimestamphigh-firsttimestamp)-(newpack.trantimestamphigh-finaltimestamp))>>1;
    //diftime=(5-9)>>1;

    //求出真正时间的时间戳
    tv1.tv_sec=time(NULL)+diftime+delaytime;
    tv1.tv_usec=0;
    //tz.

#ifdef debug
    printf("\n\ndebug information ...\n\n");
    printf("time(NULL) is %d\n",time(NULL));
    printf("different time is %lld\n",diftime);
    printf("delaytime is %lld\n",delaytime);
    printf("time(NULL)+diftime+delaytime=%lld\n",time(NULL)+diftime+delaytime);
    printf("tv1.tv_sec is %ld\n\n", tv1.tv_sec);
#endif

    settimeofday(&tv1,NULL);
    //diftime=diftime-From00to70;

#ifdef debug
    //printf("different time is %ld\n",diftime);
    printf("delay  time is %lld\n",delaytime);
    //printf("firsttimestamp is %x\n",time(NULL));
    printf("newpack.tran is %ld\n",newpack.trantimestamphigh);
    printf("newpack.recv is %ld\n",newpack.recvtimestamphigh);
    printf("firsttimestamp is %lld\n",firsttimestamp);
    printf("finaltimestamp is %lld\n",finaltimestamp);
    printf("newpack.recv-firsttimestamp is %lld\n",newpack.recvtimestamphigh-firsttimestamp);
    printf("newpack.tran-finaltimestamp is %lld\n",newpack.trantimestamphigh-finaltimestamp);
    printf("(recv-first)+(ftran-final) is %lld\n",(newpack.recvtimestamphigh-firsttimestamp)+(newpack.trantimestamphigh-finaltimestamp));
    printf("((recv-first)+(ftran-final))>>1 is %lld\n",((newpack.recvtimestamphigh-firsttimestamp)+(newpack.trantimestamphigh-finaltimestamp))>>1);
    printf("different time is %lld\n\n",diftime);
    printf("sizeof(long long)  is:%lu\n", sizeof(long long));
    printf("Current time is...\n");
    system("date");
#endif
    printf("update time success using %s\n", ntpserver);
    return 0;
}


int main() {

    if(ntpdate(NTPSVR1) == 0) {
        return 0;
    }
    /*
    if(ntpdate(NTPSVR2) == 0) {
        return 0;
    }
    if(ntpdate(NTPSVR3) == 0) {
        return 0;
    }
    if(ntpdate(NTPSVR4) == 0) {
        return 0;
    }*/
}
