#include <STC89C5xRC.H>

unsigned char i = 0;

short timer_us;

void init_timer(void) {
    TMOD = 0x01;
    ET0 = 0x01;
    EA = 0;
    TR0 = 0;        //if 1: run T/C0 
}



void main(void) {

    while(1) {
        if(i>7) i=0;
    }

}



void Timer0IRQ interrupt 1 
{
    TH0 = (65535-timer_us)/256;
    TL0 = (65535-timer_us)%256;
    
    P2=1<i;
    i++;
}

