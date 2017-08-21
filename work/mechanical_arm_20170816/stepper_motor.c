#include "stc.h"

//1 cmd buffer; no buffer for ctrl cmd

//100kHz, min width=4us
//200clk = 1Round = 2mm
//10cm/2mm = 50Round = 10000Hz = 100us
//理论精度 0.0005mm

/*
FANBC
F：固定头0xFD
A：A个方波/10ms
N：A到B的自动计算填充N个数（10ms）
B：A个方波/10ms
C：异或校验

clk_all = A+B+N;



recv:
    0 - 0xFA(250), clk num
    0xFF:   dir 0
    0xFE:   dir 1

    cmd_c var:
        0:no cmd
        1:begin to recev
        5:recv all, need check
        6:check over, run

return;
    0xFB:   recv and crc ok
    0xFC:   CRC Error
    0xFD:   exec over;

*/

#define RECV_OK     (0xFB)
#define CRC_ERROR   (0xFC)
#define EXEC_DONE   (0xFD)
#define INTERVAL    (10000)


sbit dir=P2^0;
sbit clk=P2^1;

unsigned char recv, us, step;
unsigned char clk_num, clk_all;

unsigned char cmd[5]={0};
unsigned char cmd_c=0;;

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
    RCAP2L=0xD9; //9600 L 8bit
    RCAP2H=0xFF; //9600 H 8bit
    ES = 1;
    EA = 1;
}

void UARTSendByte(unsigned char byte) {
    SBUF=byte; 
    while(TI==0);
    TI=0;
}

void StpperInit(void) {


    P2=0xFF;    //init all high


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


void UartIRQ(void) interrupt    4 
{

    if(RI) {
        recv = SBUF;
        RI = 0;
        if((recv == 0xFF || recv == 0xFE) && cmd_c == 0) {
            cmd[0] = recv;
            dir = cmd[0]:0;
            cmd_c = 1;
        } else if (cmd_c > 0 && cmd_c < 5) {
            cmd[cmd_c++] = recv;
        }
    }


}








//和氏璧
//C-Arm
















