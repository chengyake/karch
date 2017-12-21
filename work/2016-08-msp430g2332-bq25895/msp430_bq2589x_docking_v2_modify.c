
#include <msp430.h>



/*
 * 2017-12-18
 * rules:
 * usb only:   4 flash once,Power Good
 * bat only:   n flash once,Power Level
 * usb &bat:   n flash always,charging
 * usb &bat:   4 led on, charge complate
 */

#define SLAVE_ADDR  (0xD4)  //0x6A
//#define LS_DEBUG
#ifdef LS_DEBUG
unsigned char regs[21]={0};
#endif

//misc function
unsigned char read_bq2589x(unsigned char r, unsigned char *v);

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


static unsigned short ticks=0;
static unsigned char led=0x0F, led1=0x0F;
static unsigned char flash_once_on=0;
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
    ticks++;
    if(ticks>=210) {
        ticks=0; 
        if(flash_once_on>0) {
            flash_once_on--;
            if(flash_once_on==0) {
                led=0x0F;led1=0x0F;
            }
        }
    }

    if(ticks<100) {
        P2OUT =  (P2OUT|0x0F)&(~led);
    } else {
        P2OUT =  (P2OUT|0x0F)&(~led1);
    }

    CCR0 += 10000;                              // Add Offset to CCR0
    LPM0_EXIT;
}
//just for i2c
unsigned char reg, value_w, *value_r;           // Variable for transmitted data
unsigned char I2C_State, Success, Transmit;     // State variable
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
                USISRL = reg;               // Load data byte
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
            } else {    // Ack received, TX data to slave...
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
            } else {    // Ack received, RX data from slave...
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

void enable_timer() {
    CCR0 = 10000;
    CCTL0 |= CCIE;                            // CCR0 interrupt enabled
    TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode
}

void disable_timer() {
    CCTL0 &= ~CCIE;                             // CCR0 interrupt enabled
}

/*
 * flash times:
 * 0x00:all leds off
 * 0x01:flash times
 * 0xFF:flash always
 *
 * batv:
 * 0xFFFF:charge complate
 * others:bat level
 *
 * x-1 x0 l1, l2, l3, l4, x5
 *
 * on:-1,f,0    all off
 * on:0, f,1
 * on:1, f,2
 * on:2, f,3
 * on:3, f,4
 * on:4, f,5    all on
 *
 */
static unsigned char stage1=0; stage2=0;
void set_leds_mode(unsigned short batv, unsigned char flash_times, unsigned char iscomplate) {

    unsigned char old_led = led;

    stage2=stage1;
    stage1=flash_times;

    if(iscomplate) {                    // 4 on, no flash
        led=0x00;
        led1=0x00;
        return;
    }

    if (batv<=2304 || flash_times==0) {//no battery or error
        led=0x0F;
        led1=0x0F;
        return;
    }

    if(stage2==0x00 && stage1==0x01) {
        flash_once_on=3;
    }

    if(flash_once_on!=0 || flash_times==0xFF) {
        led=0x07; led1=0x0F;                    //0 on, 1 flash
        if(batv > 3800) {led=0x05;led1=0x07;}   //1 on, 2 flash
        if(batv > 4000) {led=0x01;led1=0x05;}   //2 on, 3 flash
        if(batv > 4200) {led=0x00;led1=0x01;}   //3 on, 4 flash
        if(flash_once_on!=0) {led1=0x0F;}
    } else {
        led=0x0F;
        led1=0x0F;
    }

    if(old_led==0x0F && led != 0x0F) {
        ticks=0;
    }

}


void init_bq2589x() {

    unsigned char v;

    write_bq2589x(0x02, 0xFD);//start 1Hz ADC
    //write_bq2589x(0x06, 0x82);//voltage limit 4.352V = 3.840 + 0.512
    write_bq2589x(0x06, 0x8A);//voltage limit 4.384V = 3.840 + 0.512 + 0.032

    //disable wdog and stat pin
    read_bq2589x(0x07, &v);
    write_bq2589x(0x07, (v|0x40)&(~0x30));

    //min sys 3.6v
    read_bq2589x(0x03, &v);
    write_bq2589x(0x03, (v&(~0x2E)));//mini sys 3.0V && disable OTG

    //thermal threshold 60`;
    read_bq2589x(0x08, &v);
    write_bq2589x(0x08, v&(~0x03));


    write_bq2589x(0x04, 0x40);            //Max-Current 4096 mA
    //write_bq2589x(0x05, 0x22);            //pre-charge 128mA, Iterm 128mA
    //write_bq2589x(0x0D, 0x80);          //some how?? 0xFF


}

void register_reset() {
    unsigned char r=0;
    write_bq2589x(0x14, 0xB9);//start 1Hz ADC
    do {
        read_bq2589x(0x14, &r);
    } while(r&0x80);
}

unsigned char get_charge_stage(unsigned char regb) {
    switch (regb&0x18) {
        case 0x00:
            return 0;
        case 0x18:
            return 2;
        default:
            return 1;
    }
}

unsigned short get_max_batv(unsigned short batv, unsigned char clear) {
    static unsigned short maxv=0;
    if(clear || batv<=2304) {
        maxv=0;
        return maxv;
    }
    if(maxv<batv) {
        maxv = batv;
    }
    return maxv;
}

unsigned short batv=0;
unsigned char reg_B=0, reg_C=0, reg_E=0, reg_12;
unsigned char powergood, batgood, stage;
/*
 * P2.5        INT BQ
 * P1.6-P1.7   I2C
 * P2.0-P2.3   LEDs
 */
int main(void)
{


    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog
    /*if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
      {
      while(1);                               // do not load, trap CPU!!
      }*/
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;

    P2DIR |= 0x0F;                            // Set P2.0 to output direction
    P2OUT = (P2OUT&~0x0F);                    //clear all leds
    P2IE |= 0x20;                             // P2.5 interrupt enabled
    P2IES |= 0x20;                            // P2.5 Hi/lo edge
    P2IFG &= ~0x20;                           // P2.5 IFG cleared

    P1OUT = 0xC0;                             // P1.6 & P1.7 Pullups, others to 0
    P1REN |= 0xC0;                            // P1.6 & P1.7 Pullups


    register_reset();

    init_bq2589x();
    enable_timer();

    while(1) {
#ifdef LS_DEBUG
        //debug:dump all register
        unsigned char i;
        for(i=sizeof(regs); i>0; i--) {
            read_bq2589x(i-1, &regs[i-1]);
        }
#endif

        read_bq2589x(0x0B, &reg_B);
        read_bq2589x(0x0C, &reg_C);
        read_bq2589x(0x0E, &reg_E);
        read_bq2589x(0x12, &reg_12);

        //batv = (reg_E&0x7F)*20 + 2304;//-(reg_12*45/10));
        batv = ((reg_E&0x7F)*20 + 2304);
        batv = (batv - (reg_12<<2) - (reg_12>>1));

        powergood=((reg_B&0x04)==0) ? 0 : 1;
        batgood = batv==2304 ? 0 : 1;
        stage = get_charge_stage(reg_B);

        batv = get_max_batv(batv, reg_C&0x01);
        

        if(powergood && batgood) {
            if(stage==0) {
                set_leds_mode(batv, 0, 0);
            } else if(stage==2) {
                set_leds_mode(batv, 0xFF, 1);
            } else {
                set_leds_mode(batv, 0xFF, 0);
            }
        } else if (powergood && !batgood) {
            set_leds_mode(0x00, 0, 0);
        } else if ( !powergood && batgood) {
            set_leds_mode(batv, 0x01, 0);
        } else {
            //never get here
            set_leds_mode(0x00, 0, 0);
        }

        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM4 w/interrupt
    }
}
