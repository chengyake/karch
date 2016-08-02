
#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P2DIR |= 0x0F;                            // Set P1.0 to output direction
  P2REN |= 0x0F;

  P1IE |= 0x04;                             // P1.4 interrupt enabled
  P1IES |= 0x04;                            // P1.4 Hi/lo edge
  P1IFG &= ~0x04;                           // P1.4 IFG cleared

  P2IE |= 0x20;                             // P1.4 interrupt enabled
  P2IES |= 0x20;                            // P1.4 Hi/lo edge
  P2IFG &= ~0x20;                           // P1.4 IFG cleared


  __bis_SR_register(LPM4_bits + GIE);       // Enter LPM4 w/interrupt
}

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
  P2OUT ^= 0x0F;                            // P1.0 = toggle
  P1IFG &= ~0x04;                           // P1.4 IFG cleared
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
  P2OUT ^= 0x06;                            // P1.0 = toggle
  P2IFG &= ~0x20;                           // P1.4 IFG cleared
}



