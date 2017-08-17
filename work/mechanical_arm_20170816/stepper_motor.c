#include "stc.h"


//1 cmd buffer; no buffer for ctrl cmd




//100kHz, min width=4us
//200clk = 1Round = 2mm
//10cm/2mm = 50Round = 10000Hz = 100us
//理论精度 0.0005mm

/*
    *c
    0:no cmd
    1:begin to recev
    5:recv all, need check
    6:check over, run



*/









sbit clk=p2^0;
sbit dir=p2^1;

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
}

void UARTSendByte(unsigned char byte) {
    SBUF=byte; 
    while(TI==0);
    TI=0;
}

void recv_cmd() {
    
    if(RI) {
        recv = SBUF;
        if((recv == 0xFF || recv == 0xFE) && c == 0) {
            cmd[0] = recv;
            dir = cmd[0]:0;
            c = 1;
        } else if (c > 0 && c < 5) {
            cmd[c++] = recv;
        }
    }

}



void main(void) {

    p2=0xFF;    //init all high
    UARTInit(); 
    while(1) {    

        us = 0; //default delay 10 ms

        if(c == 5) {
            if(cmd[1]^cmd[2]^cmd[3] == cmd[4]) {
                c = 6;
                step = 0;
                clk_all = 0;
                clk_num = 0;
            } else {
                c = 0;
                UARTSendByte(0xFF);
            }
        }
        if(c == 6) {
            clk_all = ((cmd[3]-cmd[1])/cmd[2]*step + cmd[1]);
            if(clk_all == 0) {
                us = 10000;
            } else {
                us = 10000/clk_all;

                clk=~clk;
                clk_num++;
                if(clk_num >= clk_all) {
                    step++;
                    if(step >= cmd[2]+2) {
                        c = 0;
                    }
                }

            }

        }

        delay_us(us);
    }

}








//和氏璧
//C-Arm
















