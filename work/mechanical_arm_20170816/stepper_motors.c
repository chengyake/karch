#include "stc.h"


/*

mode : run/suspend

Frame:
    0xEC | cmd | start/run/end/return | xor

    CMD:
        start:0xFF, run mode

        stop :0x00, suspend mode

        run  :0x55, fill action cmd data in both mode
                T: 1byte : address
                D: 1byte : dir
                a：4bytes: 启动速度, us/step
                A：4bytes: 启动步数
                b：4bytes: 匀速速度, us/step
                B：4bytes: 匀速步数
                c：4bytes: 截至速度, us/step
                C：4bytes: 减速步数
        
        ret  :return code for each cmd
                RECV :  0x00
                DONE :  0xFF
                BUSY :  0xBB
                XORE :  0xEE

ps: ((a-b)%A == 0) && ((c-b)%C == 0)

*/

#define RECV     (0x00)
#define DONE     (0xFF)
#define BUSY     (0xBB)
#define XORE     (0xEE)


sbit dir0=P2^0;
sbit clk0=P2^1;
sbit dir1=P2^2;
sbit clk1=P2^3;
sbit dir2=P2^4;
sbit clk2=P2^5;
sbit dir3=P2^6;
sbit clk3=P2^7;
sbit dir4=P1^0;
sbit clk4=P1^1;
sbit dir5=P1^2;
sbit clk5=P1^3;

struct cmd_s {
    unsigned char dir;
    unsigned int start_speed;
    unsigned int start_steps;
    unsigned int keep_speed;
    unsigned int keep_steps;
    unsigned int stop_speed;
    unsigned int stop_steps;
}cmds[6];

unsigned char mode;
unsigned char recv_buff[28];
unsigned char recv_counter=0;


void delay_us(unsigned short t) {
    while(--t);
}


void delay_ms(unsigned short t) { //ms
    while(--t) {
        delay_us(990);
    }
}

void UARTInit(void) {
    SCON =0x50; //8bit data, allow recv
    T2CON=0x34; //T/C2 clk generator
    RCAP2L=0xD9;//9600 L 8bit
    RCAP2H=0xFF;//9600 H 8bit
    ES = 1;
    EA = 1;
}

void UARTSendByte(unsigned char byte) {
    SBUF=byte; 
    while(TI==0);
    TI=0;
}

void StpperInit(void) {

    P1 = 0xFF;
    P2 = 0xFF;
}


void main(void) {
    
    StepperInit();
    UARTInit(); 

    while(1) {

        us = INTERVAL;

        if(cmd_c == 5) {
            if(cmd[1]^cmd[2]^cmd[3] == cmd[4]) {
                cmd_c = 6;
                step = 0;
                clk_all = 0;
                clk_num = 0;
                UARTSendByte(RECV_OK);
            } else {
                cmd_c = 0;
                UARTSendByte(CRC_ERROR);
            }
        }
        if(cmd_c == 6) {
            clk_all = (cmd[1] + (cmd[3]-cmd[1])/cmd[2]*step);
            if(clk_all == 0) {
                us = INTERVAL;
            } else {
                us = INTERVAL/clk_all;

                clk=~clk;
                clk_num++;
                if(clk_num >= clk_all) {
                    step++;
                    clk_num = 0;
                    if(step >= cmd[2]+2) {
                        cmd_c = 0;
                    }
                }

            }

        }

        delay_us(us);
    }

}


void load_cmd_from_buff(void) {


}

void UartIRQ(void) interrupt    4
{

    if(RI) {
        recv = SBUF;
        if(recv == 0xEC && recv_counter == 0) {
            recv_counter = 1;

        } else if(recv_counter == 1) {
            recv_conter = 2;
            recv_buff[recv_counter-2] = recv;
            if(recv == 0x55) {
                recv_max = 28;
            } else if(recv == 0xFF || recv == 0x00) {
                recv_max = 2;
            } else {
                recv_max = 0;
                recv_counter = 0;
                UARTSendByte(XORE);
            }
        } else if (recv_counter > 1 && recv_conter < recv_max) {
            recv_counter++;
            recv_buff[recv_conter-2] = recv;
            if(recv_conter > recv_max) {
                
            }
        }

        RI = 0;
    }


}








//和氏璧
//C-Arm
















