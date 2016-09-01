#include <msp430.h>

//50ms/per
#define	LED_ON_TIME	(20)
#define	LONG_PRESS_TIME	(30)
#define SAMPLE_AVG_NUM	(5)
#define BAT_LOW_VERSION
//#define LS_DEBUG

#define SLAVE_ADDR	(0xD4)	//0x6A

volatile unsigned int i;                  		// Use volatile to prevent removal



/*
 * 20160812 14:32
 * mode:
 * 0:idle    1:display once		2:charge	3:work
 */

//lts
unsigned char mode=0, reg_batv=0, batfet=0, votg=0, charge_complate=0;
unsigned char button=0, button_count=0, mode1_count=0;
unsigned short mode23_count=0;
unsigned short avg_sample[SAMPLE_AVG_NUM]={0};

#ifdef BAT_LOW_VERSION
//const 							 close 1led  2led  3led  4leds   always on
const unsigned short idle_th[] = 		{3650, 3700, 3800, 3950, 4110};
const unsigned short charge_th[] = 		{3650, 3700, 3800, 3950, 4110};
const unsigned short discharge_th[] = 	{3530, 3660, 3740, 3900, 4110};
#else
const unsigned short idle_th[] = 		{3650, 3800, 3950, 4100, 4240};
const unsigned short charge_th[] = 		{3650, 3800, 3950, 4100, 4240};
const unsigned short discharge_th[] = 	{3520, 3650, 3880, 4000, 4240};
#endif
//just for i2c
unsigned char reg, value_w, *value_r;      		// Variable for transmitted data
unsigned char I2C_State, Success, Transmit;     // State variable
//dump debug
#ifdef LS_DEBUG
unsigned char regs[21]={0};
#endif


void disable_batfet();
void enable_batfet();
void get_batv();
void mdelay(unsigned char n);


// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
    //min 3.6 max 4.2
    P1IE &= ~0x04;                             // P1.2 interrupt disabled
    P1IFG &= ~0x04;                           // P1.2 IFG cleared
    button = 1;
    LPM0_EXIT;
}

// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    P2IFG &= ~0x20;                           // P2.5 IFG cleared
    LPM0_EXIT;
}


// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    if(mode == 1) {
        mode1_count++;
        mode23_count=0;
        button_count=0;
    } else if(mode != 0) {
        if((P1IN&0x04) == 0) {
            button_count++;
        } else {
            button_count=0;
        }
        mode23_count++;
        mode1_count=0;
    }

    CCR0 += 50000;                              // Add Offset to CCR0
    LPM0_EXIT;									//double check
}

/******************************************************
// USI interrupt service routine
// Data Transmit : state 0->1->2->3->4->5->6->7-
// Data Recieve  : state 0->1->2->3->7->8->9->10->11->7->8
 ******************************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USI_VECTOR
__interrupt void USI_TXRX (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USI_VECTOR))) USI_TXRX (void)
#else
#error Compiler not supported!
#endif
{
    switch(I2C_State) {
        case 0: // Generate Start Condition & send address to slave
            USISRL = 0x00;                // Generate Start Condition...
            USICTL0 |= USIGE+USIOE;
            USICTL0 &= ~USIGE;

            USISRL = SLAVE_ADDR;

            I2C_State = 1;                // next state: rcv address (N)Ack
            USICNT = (USICNT & 0xE0) + 0x08; // Bit counter = 8, TX Address
            break;

        case 1: // Receive Address Ack/Nack bit
            I2C_State = 2;                // Go to next state: check (N)Ack
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 2: // Process Address Ack/Nack & handle reg TX
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
                Success=2;
            } else { // Ack received, TX data to slave...
                USISRL = reg;          		// Load data byte
                I2C_State = 3;
                USICNT |=  0x08;              // Bit counter = 8, start TX
            }
            break;

        case 3: //Get reg Ack/Nack bit
            if(Transmit == 1) {
                I2C_State = 4;               // Go to next state: check (N)Ack
            } else {
                I2C_State = 8;               // Go to next state: check (N)Ack
            }
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 4: // Process Address Ack/Nack & handle data TX
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;               // Go to next state: check (N)Ack
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
                Success=2;
            } else { 	// Ack received, TX data to slave...
                USISRL = value_w;            // Load data byte
                I2C_State = 5;               // Go to next state: check (N)Ack
                USICNT |=  0x08;              // Bit counter = 8, start TX
            }
            break;


        case 5: // Get Ack/Nack bit
            I2C_State = 6;                // Go to next state: check (N)Ack
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 6: // Check ACK and Prep Stop Condition
            USICTL0 |= USIOE;             // SDA = output
            USISRL = 0x00;
            USICNT |=  0x01;              // Bit counter= 1, SCL high, SDA low
            I2C_State = 7;               // Go to next state: generate Stop
            break;


        case 7:// Generate Stop Condition
            USISRL = 0x0FF;               // USISRL = 1 to release SDA
            USICTL0 |= USIGE;             // Transparent latch enabled
            USICTL0 &= ~(USIGE+USIOE);    // Latch/SDA output disabled
            I2C_State = 0;                // Reset state machine for next xmt
            Success++;
            //LPM0_EXIT;                    // Exit active for next transfer
            break;


        case 8:
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;               // Go to next state: check (N)Ack
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
                Success=2;
                break;
            }

            USICTL0 |= USIOE;             // SDA = output
            USISRL = 0xFF;              // Send NAck
            I2C_State = 9;              // stop condition
            USICNT |= 0x01;               // Bit counter = 1, send (N)Ack bit
            break;


        case 9: // Generate Start Condition & send address to slave
            USISRL = 0x00;                // Generate Start Condition...
            USICTL0 |= USIGE+USIOE;
            USICTL0 &= ~USIGE;

            USISRL = SLAVE_ADDR+1;

            I2C_State = 10;                // next state: rcv address (N)Ack
            USICNT = (USICNT & 0xE0) + 0x08; // Bit counter = 8, TX Address
            break;


        case 10: // Receive Address Ack/Nack bit
            I2C_State = 11;                // Go to next state: check (N)Ack
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 11: // Process Address Ack/Nack & handle data Rx
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;               // Go to next state: check (N)Ack
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
                Success=2;
            } else { 	// Ack received, RX data from slave...
                USICTL0 &= ~USIOE;                  // SDA = input --> redundant
                I2C_State = 12;                      // Next state: Test data and (N)Ack
                USICNT |=  0x08;                    // Bit counter = 8, RX data
            }
            break;


        case 12: // Send Data Ack/Nack bit
            *value_r = USISRL;             //get value from register

            USICTL0 |= USIOE;             // SDA = output
            USISRL = 0xFF;              // Send NAck
            I2C_State = 6;              // stop condition
            USICNT |= 0x01;               // Bit counter = 1, send (N)Ack bit
            break;

    }
    USICTL1 &= ~USIIFG;                       // Clear pending flag
}



void Setup_USI_Master() {
    USICTL0 = USIPE6+USIPE7+USIMST+USISWRST;  // Port & USI mode setup
    USICTL1 = USII2C+USIIE;                   // Enable I2C mode & USI interrupt
    USICKCTL = USIDIV_7+USISSEL_2+USICKPL;    // USI clk: SCL = SMCLK/128
    USICNT |= USIIFGCC;                       // Disable automatic clear control
    USICTL0 &= ~USISWRST;                     // Enable USI
    USICTL1 &= ~USIIFG;                       // Clear pending flag
}



void Setup_USI_Master_TX ()
{
    __disable_interrupt();
    Transmit = 1;
    Setup_USI_Master();
    __enable_interrupt();
}


void Setup_USI_Master_RX ()
{
    __disable_interrupt();
    Transmit = 0;
    Setup_USI_Master();
    __enable_interrupt();

}

unsigned char write_bq2589x(unsigned char r, unsigned char v) {
    unsigned char j;
    reg = r;
    value_w = v;
    Success = 0;
    Setup_USI_Master_TX();
    USICTL1 |= USIIFG;                      // Set flag and start communication

    for(j=6; j>0; j--) {
        __delay_cycles(1000);
        if(Success==1) return 0;
    }
    __delay_cycles(20000);
    return 1;
}

unsigned char read_bq2589x(unsigned char r, unsigned char *v) {
    unsigned char j;
    reg = r;
    value_r = v;
    Success = 0;
    Setup_USI_Master_RX();
    USICTL1 |= USIIFG;                        // Set flag and start communication

    for(j=6; j>0; j--) {
        __delay_cycles(10000);
        if(Success==1) return 0;
    }
    __delay_cycles(20000);
    return 1;
}


void get_batv() {

    unsigned char check,j;
    //start adc
    if(charge_complate == 1) {
        reg_batv = 0x5F;
        return;
    }
    for(i=5; i>0; i--) {
        read_bq2589x(0x02, &check);
        write_bq2589x(0x02, check|0x80);
        for(j=5; j>0; j--) {
            __delay_cycles(10000);
            read_bq2589x(0x02, &check);
            if((check&0x80) == 0) {
                read_bq2589x(0x0E, &check);
                break;
            }
        }
        if(check != 0x00) {break;}
    };

    reg_batv=check;
}

void disable_batfet() {
    unsigned char v;
    batfet=0;
    read_bq2589x(0x09, &v);
    write_bq2589x(0x09, (v|0x20)&(~0x08));
    //P1OUT &= (~0x08);
}

void enable_batfet() {
    unsigned char v;
    batfet=1;
    read_bq2589x(0x09, &v);
    write_bq2589x(0x09, v&(~0x20));
    //P1OUT |= 0x08;

}

void delay_cycles(unsigned char t) {

    for(i=t; i>0; i--) {
        __delay_cycles(10000);
    }


}

void enable_timer() {
    CCR0 = 50000;
    CCTL0 |= CCIE;                            // CCR0 interrupt enabled
    TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode
}

void disable_timer() {
    CCTL0 &= ~CCIE;                             // CCR0 interrupt enabled
}

unsigned char get_idx(unsigned short percent, const unsigned short *array) {

    if(charge_complate) {
        return 5;
    }


    if(percent <= array[0]) {
        return 0;
    } else if(percent <= array[1]) {
        return 1;
    } else if(percent <= array[2]) {
        return 2;
    } else if(percent <= array[3]) {
        return 3;
    } else if(percent <= array[4]) {
        return 4;
    } else {
        return 5;
    }

}


/*
 *  led-mode 0:all on, 1:latest flash
 *
 */
void light_leds(unsigned int mode, unsigned short percent) {

    static unsigned char flash=0;
    unsigned char a,b, idx, led_mode;

    if(mode > 1 && charge_complate == 1) {
        P2OUT|=0x0F;
        return;
    }
    switch (mode) {

        case 0:
            led_mode=0;
            idx=0;
            break;
        case 1:
            led_mode=0;
            idx=get_idx(percent, &idle_th[0]);
            break;
        case 2:
            led_mode=1;
            idx=get_idx(percent, &charge_th[0]);
            break;
        case 3:
            led_mode=1;
            idx=get_idx(percent, &discharge_th[0]);
            break;
        default:
            led_mode=0;
            idx=0;
            break;
    }
    switch (idx) {
        case 0:
            a=0x00;
            b=0x00;
            break;
        case 1:
            a=0x00;
            b=0x04;
            break;
        case 2:
            a=0x04;
            b=0x06;
            break;
        case 3:
            a=0x06;
            b=0x07;
            break;
        case 4:
            a=0x07;
            b=0x0F;
            break;
        default:
            led_mode =0;
            a=0xFF;
            b=0xFF;
            break;
    }

    if(led_mode) {
        flash^=0x01;
        if(flash)
            P2OUT = ((P2OUT&(~0x0F))|a);
        else
            P2OUT = ((P2OUT&(~0x0F))|b);
    } else {
        P2OUT = ((P2OUT&(~0x0F))|b);
    }
}

void reset_avg_sample() {
    for(i=0; i<SAMPLE_AVG_NUM; i++) {
        avg_sample[i]=0;
    }
}

unsigned short get_and_update_avg_sample(unsigned short voltage) {
    unsigned char num=0;
    unsigned int sum=0;
    for(i=0; i<SAMPLE_AVG_NUM; i++) {
        if(avg_sample[i] > 3000) {
            sum+=avg_sample[i];
            num++;
        }
    }

    if(voltage > 3000 && voltage != avg_sample[SAMPLE_AVG_NUM-1]) {
        sum+=voltage;
        num++;
        for(i=0; i<SAMPLE_AVG_NUM-1; i++) {
            avg_sample[i]=avg_sample[i+1];
        }
        avg_sample[SAMPLE_AVG_NUM-1]=voltage;
    }
    if(num==0) return num;
    return sum/num;


}


void init_bq2589x() {

    unsigned char v;

    write_bq2589x(0x05, 0x12);           //Ipre and Iterm:128mA

#ifndef BAT_LOW_VERSION
    write_bq2589x(0x06, 0x82);//voltage limit 4.352V = 3.840 + 0.512
#else
    write_bq2589x(0x06, 0x5E);//voltage limit 4.208V by default
#endif

    //disable wdog and stat pin
    read_bq2589x(0x07, &v);
    write_bq2589x(0x07, (v|0x40)&(~0x30));

    //min sys 3.6v
    read_bq2589x(0x03, &v);
    write_bq2589x(0x03, (v&(~0x2E))|0x0C);//mini sys 3.6V && disable OTG

    //thermal threshold 60`;
    read_bq2589x(0x08, &v);
    write_bq2589x(0x08, v&(~0x03));


    write_bq2589x(0x0D, 0x80);		      //some how?? 0xFF

}

/*
 * P1.2                Button
 * P1.3                VOTG
 * P2.5                INT BQ
 * P1.6-P1.7   I2C
 * P2.0-P2.3   LEDs
 */
int main(void)
{
    unsigned char v;
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog
    /*if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
      {
      while(1);                               // do not load, trap CPU!!
      }*/
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;

    P1DIR |= 0x08;                            // Set P1.3 to output direction
    P1OUT = 0xC0;                             // P1.6 & P1.7 Pullups, others to 0
    P1REN |= 0xC0;                            // P1.6 & P1.7 Pullups
    P1IE |= 0x04;                             // P1.2 interrupt enabled
    P1IES |= 0x04;                            // P1.2 Hi/lo edge
    P1IFG &= ~0x04;                           // P1.2 IFG cleared

    P2DIR |= 0x0F;                            // Set P2.0 to output direction
    P2REN |= 0x0F;
    P2OUT &= ~0x0F;							  //clear all leds
    P2IE |= 0x20;                             // P2.5 interrupt enabled
    P2IES |= 0x20;                            // P2.5 Hi/lo edge
    P2IFG &= ~0x20;                           // P2.5 IFG cleared

    init_bq2589x();

    __no_operation();

    //enble batfet if need
    if(batfet == 0) {
        enable_batfet();
        delay_cycles(10);
    }
    read_bq2589x(0x03, &v);
    write_bq2589x(0x03, v&(~0x10));
    get_batv();
    write_bq2589x(0x03, v|0x10);


#ifdef LS_DEBUG
    //debug:dump all register
    for(i=sizeof(regs); i>0; i--) {
        read_bq2589x(i-1, &regs[i-1]);
    }
#endif

    while(1) {


        unsigned char reg_stat, reg_fault;


        P1IE &= ~0x04;                            // Button P1.2 interrupt disabled
        P1IFG &= ~0x04;                           // P1.2 IFG cleared

        write_bq2589x(0x0D, 0x80);				  //some how?? 0xFF

        //error handle
        read_bq2589x(0x0C, &reg_fault);
        if(reg_fault != 0x00) {
            read_bq2589x(0x0C, &reg_fault);
        }

        //mode, votg
        read_bq2589x(0x0B, &reg_stat);
        if((reg_stat&0x18) == 0x18) {
            charge_complate=1;
        }
        if((reg_stat&0x0E4) != 0x00) { 									   //power good
            button=0;//useless
            if(mode < 2) mode = 2;
            if((reg_stat&0xE0) == 0x20 || (reg_stat&0xE0) == 0x40) {       //usb pc
                if(button_count == LONG_PRESS_TIME) {
                    votg^=0x01;
                }
                if(votg) {
                    unsigned short t = get_and_update_avg_sample((reg_batv&0x7F)*20 + 2304);
                    if(mode==2 &&  t> charge_th[0]) {
                        mode = 3;
                    }
                    if(mode==3 && t < discharge_th[0]) {
                        votg=0;
                        mode=2;
                    }
                } else {
                    mode = 2;
                }
            } else {                            //adapter
                votg = 0;
                mode = 2;
            }
        } else {								//no power
            votg = 0;
            if(button == 1 || mode == 1) {
                mode = 1;
            } else {
                mode = 0;
            }
            button=0;
        }


        switch (mode) {
            case 0:
                button_count=0;
                mode1_count=0;
                mode23_count=0;
                charge_complate=0;
                reset_avg_sample();
                disable_timer();
                P2OUT &= ~0x0F;
                P1OUT &= (~0x08);
                if(batfet) {
                    disable_batfet();
                }

                break;

            case 1:
                button_count=0;
                mode23_count=0;
                charge_complate=0;
                P1OUT &= (~0x08);
                reset_avg_sample();
                if(mode1_count == 0) {
                    //enble batfet if need
                    if(batfet == 0) {
                        enable_batfet();
                        delay_cycles(10);
                    }
                    get_batv();
                    mode1_count++;
                    enable_timer();
                } else if(mode1_count > 0 && mode1_count < LED_ON_TIME) {
                    if(batfet) disable_batfet();
                    mode1_count++;
                    enable_timer();
                } else if(mode1_count >= LED_ON_TIME) {
                    mode1_count=0;
                    disable_timer();
                    P2OUT &= ~0x0F;
                    if(batfet) {
                        disable_batfet();
                    }
                    mode = 0;
                }
                light_leds(mode, (reg_batv&0x7F)*20 + 2304);
                break;

            case 2:
                votg=0;
                P1OUT &= (~0x08);
                enable_timer();
                mode23_count++;
                if(mode23_count>=20*15) {
                    read_bq2589x(0x03, &v);
                    write_bq2589x(0x03, v&(~0x10));
                    get_batv();
                    write_bq2589x(0x03, v|0x10);
                    mode23_count=0;
                }
                light_leds(mode, get_and_update_avg_sample((reg_batv&0x7F)*20 + 2304));
                break;

            case 3:
                if(batfet == 0) enable_batfet();
                votg=1;
                P1OUT |= 0x08;
                enable_timer();
                mode23_count++;
                if(mode23_count>=20*15) {
                    read_bq2589x(0x03, &v);
                    write_bq2589x(0x03, v&(~0x10));
                    get_batv();
                    write_bq2589x(0x03, v|0x10);
                    mode23_count=0;
                }
                light_leds(mode, get_and_update_avg_sample((reg_batv&0x7F)*20 + 2304));
                break;
        }

        P1IE |= 0x04;                             // P1.2 interrupt enabled
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM4 w/interrupt
    }
}
//edit at 2016/08/31 11:20
