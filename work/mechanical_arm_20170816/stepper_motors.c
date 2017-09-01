#include "stc.h"
/*

                main
                 |
                 |
                 |
                 |              switch clk    //(mini interval 100us)
                 ------------->    and
                 |             reload timer
       --------->|
       |         V
       |     recv bytes
       |    process cmds
       |  calc next delay us
       |         |
       -----------

mode : run/stop

Frame:
    0xEC | cmd | start/run/end | xor

    CMD:
        echo :0x01, echo 0xEC to confirm hardware ok
        start:0xFF, run mode(1)

        stop :0x00, stop mode(0)

        run  :0x55, fill action cmd data in both mode
                b：4bytes: 间隔, us/step
                b：4bytes: 间隔, us/step
                b：4bytes: 间隔, us/step
                b：4bytes: 间隔, us/step
                B：4bytes: 步数, +-
                B：4bytes: 步数
                B：4bytes: 步数
                B：4bytes: 步数
    ret:
           return code for each cmd
            RECV :  0x00
            DONE :  0xFF
            BUSY :  0xBB
            XORE :  0xEE
            ECHO :  0xEC

    xor: all ^ xor = 0;

ps: ((a-b)%A == 0) && ((c-b)%C == 0)
    move_to(p, a, t); //line in 3D space
    move_steps(n, t); //
    暂不考虑过冲情况
*/

//return code
#define RECV     (0x00)
#define DONE     (0xFF)
#define BUSY     (0xBB)
#define XORE     (0xEE)
#define ECHO     (0xEC)

//Position
sbit dir0=P2^0;
sbit clk0=P2^1;
sbit dir1=P2^2;
sbit clk1=P2^3;
sbit dir2=P2^4;
sbit clk2=P2^5;
//degree
sbit dir3=P2^6;
sbit clk3=P2^7;
//action
sbit dir4=P1^0;
sbit clk4=P1^1;
sbit dir5=P1^2;
sbit clk5=P1^3;

struct action_s {
    unsigned int interval[4];
    int steps[4];
} actions;
unsigned int timer_us[4]={0};
unsigned int delay_us=0;    //0 no action on

unsigned char recv_buff[64];
unsigned char recv_counter=0;
unsigned char mode;

void delay(unsigned char t) {
    while(--t);
}

void UARTInit(void) {
    SCON =0x50; //8bit data, allow recv
    T2CON=0x34; //T/C2 clk generator
    RCAP2L=0xD9;//9600 L 8bit
    RCAP2H=0xFF;//9600 H 8bit
}

void UARTSendByte(unsigned char byte) {
    SBUF=byte; 
    while(TI==0);
    TI=0;
}

void StepperInit(void) {

    P1 = 0xFF;
    P2 = 0xFF;
}

void init_timer(void) {
    TMOD = 0x01;
    ET0 = 0x01;
    EA = 0;
    TR0 = 1;        //if 1: run T/C0 
}

void send_pluse(sbit p) {
    
    p=0;
    delay(5);
    p=1;
}

void process_buffer() {
    unsigned char tmp, i;
    unsigned char *p = (unsigned char)&actions;

    tmp = recv_buff[2]^recv_buff[3];
    for(i=4; i<34; i++) {
        tmp ^= recv_buff[i];
    }
    if(tmp != recv_buff[34]) {
        UARTSendByte(XORE);
        return ;
    }
    
    for(i=0; i<32; i++) {
        p[i] = recv_buff[i+2];
    }

    UARTSendByte(RECV);
}

void main(void) {

    StepperInit();
    UARTInit(); 

    while(1) {
        if(RI) {
            unsigned char r = SBUF;

            if(r == 0xEC || recv_counter!=0) {
                recv_buff[recv_counter] = r;
                RI = 0;

                recv_counter++;

                if(recv_counter>=3) {
                    switch(recv_buff[1]) {
                        case 0xFF://stop 
                            if(recv_buff[1] == recv_buff[2]) {
                                mode = 0;
                                UARTSendByte(DONE);
                            } else {
                                UARTSendByte(XORE);
                            }
                            recv_counter=0;
                            break;

                        case 0x01://echo 
                            UARTSendByte(ECHO);
                            break;

                        case 0x55://fill actions
                            if(recv_counter >= 35) {
                                process_buffer();
                                recv_counter=0;
                            }
                            break;

                        case 0x00://run
                            if(recv_buff[1] == recv_buff[2]) {
                                mode = 1;
                                UARTSendByte(DONE);
                            } else {
                                UARTSendByte(XORE);
                            }
                            recv_counter=0;
                            break;
                        default:
                            UARTSendByte(XORE);
                            recv_counter=0;
                            break;

                    }

                }


            }

        }

    }

}

void update_timer_us() {


}

unsigned int get_mini_interval() {

    unsigned int mini=0;
    for(i=0; i<4; i++) {
        if (mini > timer_us[i]) {
            mini = timer_us[i];
        }
    }

    return mini;
}

void Timer0IRQ interrupt 1 
{
    if(mode == 0) {
        delay_us = 0;
    } else {

        update_timer_us();


        delay_us = get_mini_interval();
    }

    TH0 = (65535-delay_us)/256;
    TL0 = (65535-delay_us)%256;
    
}

