#include <msp430.h>

//#define ls_debug

//SLAVE_ADDR	(0x6A)
#define SLAVE_ADDR	(0xD4)


volatile unsigned int i,j;                  // Use volatile to prevent removal
unsigned char reg, value_w, *value_r;      // Variable for transmitted data
unsigned char I2C_State, Success, Transmit;     // State variable
unsigned char button=0, display=0, timer_count=0, batfet=0, en_fet=0;;
unsigned char reg_stat, reg_fault, reg_batv;

#ifdef ls_debug
//dump debug
unsigned char regs[21]={0};
#endif

void get_batv();
void disable_batfet();
void enable_batfet();


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
	button = 1;
	P1IFG &= ~0x04;                           // P1.2 IFG cleared
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
	if(timer_count < 21) {
		CCR0 += 50000;                            // Add Offset to CCR0
		timer_count++;
	} else {
		timer_count=0;
		CCTL0 &= ~CCIE;                             // CCR0 interrupt enabled
		LPM0_EXIT;										//double check
	}

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
            Success++;
            USISRL = 0x00;                // Generate Start Condition...
            USICTL0 |= USIGE+USIOE;
            USICTL0 &= ~USIGE;

            USISRL = SLAVE_ADDR;


            I2C_State = 1;                // next state: rcv address (N)Ack
            USICNT = (USICNT & 0xE0) + 0x08; // Bit counter = 8, TX Address
            break;

        case 1: // Receive Address Ack/Nack bit
        	Success++;
            I2C_State = 2;                // Go to next state: check (N)Ack
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 2: // Process Address Ack/Nack & handle reg TX
        	Success++;
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
            } else { // Ack received, TX data to slave...
                USISRL = reg;          		// Load data byte
                I2C_State = 3;
                USICNT |=  0x08;              // Bit counter = 8, start TX
            }
            break;

        case 3: //Get reg Ack/Nack bit
        	Success++;
            if(Transmit == 1) {
                I2C_State = 4;               // Go to next state: check (N)Ack
            } else {
                I2C_State = 8;               // Go to next state: check (N)Ack
            }
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 4: // Process Address Ack/Nack & handle data TX
        	Success++;
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;               // Go to next state: check (N)Ack
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
            } else { 	// Ack received, TX data to slave...
                USISRL = value_w;            // Load data byte
                I2C_State = 5;               // Go to next state: check (N)Ack
                USICNT |=  0x08;              // Bit counter = 8, start TX
            }
            break;


        case 5: // Get Ack/Nack bit
        	Success++;
            I2C_State = 6;                // Go to next state: check (N)Ack
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 6: // Check ACK and Prep Stop Condition
        	Success++;
            USICTL0 |= USIOE;             // SDA = output
            USISRL = 0x00;
            USICNT |=  0x01;              // Bit counter= 1, SCL high, SDA low
            I2C_State = 7;               // Go to next state: generate Stop
            break;


        case 7:// Generate Stop Condition
        	Success++;
            USISRL = 0x0FF;               // USISRL = 1 to release SDA
            USICTL0 |= USIGE;             // Transparent latch enabled
            USICTL0 &= ~(USIGE+USIOE);    // Latch/SDA output disabled
            I2C_State = 0;                // Reset state machine for next xmt
            LPM0_EXIT;                    // Exit active for next transfer
            break;


        case 8:
        	Success++;
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;               // Go to next state: check (N)Ack
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
                break;
            }

            USICTL0 |= USIOE;             // SDA = output
            USISRL = 0xFF;              // Send NAck
            I2C_State = 9;              // stop condition
            USICNT |= 0x01;               // Bit counter = 1, send (N)Ack bit
            break;


        case 9: // Generate Start Condition & send address to slave
        	Success++;
            USISRL = 0x00;                // Generate Start Condition...
            USICTL0 |= USIGE+USIOE;
            USICTL0 &= ~USIGE;

            USISRL = SLAVE_ADDR+1;

            I2C_State = 10;                // next state: rcv address (N)Ack
            USICNT = (USICNT & 0xE0) + 0x08; // Bit counter = 8, TX Address
            break;


        case 10: // Receive Address Ack/Nack bit
        	Success++;
            I2C_State = 11;                // Go to next state: check (N)Ack
            USICTL0 &= ~USIOE;            // SDA = input
            USICNT |= 0x01;               // Bit counter=1, receive (N)Ack bit
            break;

        case 11: // Process Address Ack/Nack & handle data Rx
        	Success++;
            USICTL0 |= USIOE;             // SDA = output
            if (USISRL & 0x01) {            // If Nack received...  Send stop...
                USISRL = 0x00;
                I2C_State = 7;               // Go to next state: check (N)Ack
                USICNT |=  0x01;            // Bit counter=1, SCL high, SDA low
            } else { 	// Ack received, RX data from slave...
                USICTL0 &= ~USIOE;                  // SDA = input --> redundant
                I2C_State = 12;                      // Next state: Test data and (N)Ack
                USICNT |=  0x08;                    // Bit counter = 8, RX data
            }
            break;


        case 12: // Send Data Ack/Nack bit
        	Success++;
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

    reg = r;
    value_w = v;
    Success = 0;
    Setup_USI_Master_TX();
    USICTL1 |= USIIFG;                      // Set flag and start communication
    LPM0;                                   // CPU off, await USI interrupt
    if(Success == 8)
    	return 0;
    return 1;
}

unsigned char read_bq2589x(unsigned char r, unsigned char *v) {
    reg = r;
    value_r = v;
    Success = 0;
    Setup_USI_Master_RX();
    USICTL1 |= USIIFG;                        // Set flag and start communication
    LPM0;                                     // CPU off, await USI interrupt
    if(Success == 11)
    	return 0;
    return 1;
}



void get_batv() {
	//start adc
	read_bq2589x(0x02, &reg_batv);
	write_bq2589x(0x02, reg_batv|0x80);


    for(j=1000; j>0; j--) {
    	__delay_cycles(10000);
    	read_bq2589x(0x02, &reg_batv);
    	if((reg_batv&0x80) == 0) {
    		read_bq2589x(0x0E, &reg_batv);
    		break;
    	}
    }

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

void init_bq2589x() {

	unsigned char v;


	//get_batv();
	//disable_batfet();

	//disable wdog and stat pin
	read_bq2589x(0x07, &v);
	write_bq2589x(0x07, (v|0x40)&(~0x30));

	//min sys 3.6v
	read_bq2589x(0x03, &v);
	write_bq2589x(0x03, (v&(~0x0E))|0x0C);

	//thermal threshold 60`;
	read_bq2589x(0x08, &v);
	write_bq2589x(0x08, v&(~0x03));

}

void delay_cycles(unsigned char t) {

	for(i=t; i>0; i--) {
    	__delay_cycles(10000);
	}


}


int main(void)
{

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

    batfet = 0;
    button = 0;
    display = 0;
    timer_count=0;

    init_bq2589x();

    __no_operation();

#ifdef ls_debug
        //debug:dump all register
        for(i=sizeof(regs); i>0; i--) {
        	read_bq2589x(i-1, &regs[i-1]);
        }
#endif

    while(1) {

    	if(batfet == 0) enable_batfet();

    	delay_cycles(10);

        read_bq2589x(0x0C, &reg_fault);
        read_bq2589x(0x0B, &reg_stat);


        if(reg_fault != 0x00) {
        	read_bq2589x(0x0C, &reg_fault);
        }


        if((reg_stat&0x04) == 0x04) { 				//power good
        	display = 2;
        	if((reg_stat&0xE0) == 0x20) { 		//usb pc
        		if(button == 1) {
        			en_fet^=0x01;
        		}
        	} else {
        		en_fet = 0;
        	}
        } else {									//no power
        	en_fet = 0;
        	if(button == 1 || timer_count > 0) {
        		display = 1;
        	} else {
        		display = 0;
        	}
        }

    	//display power percent
    	if(display) {
    		  unsigned int percent;
    		  get_batv();
    		  read_bq2589x(0x12, &reg_fault);
    		  percent = (reg_batv&0x7F)*20 + 2304 - reg_fault*10;

    		  //8 1 2 4
    		  P2OUT &= ~0x0F;
    		  if(percent <= 3600) {
    			  P2OUT |= 0x04;                            //1 led
    		  } else if (percent < 3900) {
    			  P2OUT |= 0x06;                            //2 led
    		  } else if (percent < 4100) {
    			  P2OUT |= 0x07;                            //3 led
    		  } else {
    			  P2OUT |= 0x0F;							//4 led
    		  }
    		  if(timer_count == 0) {
        		  timer_count=1;
        		  CCR0 = 50000;
        		  CCTL0 |= CCIE;                             // CCR0 interrupt enabled
        		  TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode
    		  }
    	} else {
    		P2OUT &= ~0x0F;
    	}


        if(en_fet == 0) {
        	P1OUT &= (~0x08);
        	if(display != 2) {
        		disable_batfet();
        	}
        } else {
        	P1OUT |= 0x08;
        }


    	button = 0;
    	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM4 w/interrupt
    }
}




