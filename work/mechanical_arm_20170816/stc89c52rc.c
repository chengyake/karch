#include <STC89C5xRC.H>

//100kHz, min width=4us

//200clk = 1Round = 2mm

//10cm/2mm = 50Round = 10000Hz = 100us
//理论精度 0.0005mm

sbit clk=P2^0;
sbit dir=P2^1;


void delay_us(unsigned char i){
    while(--i);
}


void Delay(void) //定义Delay 函数,延时500ms
{
    unsigned char i,j; //声明变量i,j
    for(i=0;i<255;i++) //进行循环操作,以达到延时的效果
        for(j=0;j<255;j++);
    for(i=0;i<255;i++) //进行循环操作,以达到延时的效果
        for(j=0;j<255;j++);
    for(i=0;i<255;i++) //进行循环操作,以达到延时的效果
        for(j=0;j<140;j++);
}

void UARTInit(void) //定义串口初始化函数
{
    SCON =0x50; //8 位数据位, allow recv
    T2CON=0x34; //由T/C2 作为波特率发生器
    RCAP2L=0xD9; //波特率为9600 的低8 位
    RCAP2H=0xFF; //波特率为9600 的高8 位
    ES = 1;
    EA = 1;
}

void UARTSendByte(unsigned char byte)//串口发送单字节函数
{
    SBUF=byte; //缓冲区装载要发送的字节
    while(TI==0); //等待发送完毕,TI 标志位会置1
    TI=0; //清零发送完成标志位
}

void UartIRQ(void) interrupt 4
{
    unsigned char recv;
    if(RI) {
        recv = SBUF;
        RI = 0;
        UARTSendByte(recv); //串口发送单字节数据
        //Delay(); //延时500ms
    }

}

void main(void) //进入Main 函数
{

    //P2=0xFF;    //init all high

    UARTInit(); //串口初始化
    while(1) //进入死循环
    {
        Delay(); //延时500ms
    }
}












