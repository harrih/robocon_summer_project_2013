//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, 9600 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1MHz = 1MHz/9600 = ~104.2
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 9600 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include  "msp430g2452.h"
#include <legacymsp430.h>
#include <inttypes.h>

// P2.0 - Phase X Left
// P2.1 + P2.2 - Phase Y Left
// P2.6 - Phase X Right
// P2.0 - Phase X Left

# define LEDL BIT0
# define LEDR BIT6

uint16_t encl = 0, encr = 0, pencl = 0, pencr = 0;
uint16_t encl_flg = 2, encr_flg = 0, encl_val = 0, encr_val = 0;
int i = 0;
int lstate = 0, rstate = 0;

/* Pin config:
 * Left encoder:
 * 	Phase A up -> P2.0
 * 	Phase B up -> P2.1
 * 	Phase B down -> P2.2
 * Right Encoder:
 * 	Phase A up -> P2.3
 * 	Phase B up -> P2.4
 * 	Phase B down -> P2.5
 */
int main(void)
{
  // Stop WDT
  WDTCTL = WDTPW + WDTHOLD;
  // Init UART
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 8;                            // 1MHz 9600
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS2 + UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

  // Initialize pins foe LED
  P1DIR |= LEDL;
  P1OUT |= LEDL;
  //P2DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5); // get input from encoders (0,6 for phase X and 1,5 for phase Y)

  P2DIR |= BIT1;
  P2OUT |= BIT0;	//enable pull-up resistor
  P2REN |= BIT0;	//enable pull-up resistor
  P2IFG &= ~BIT0; // clear int flags for Encoders
  P2IE |= BIT0; // Init Interrupts

  //P2IES &= ~BIT0;

 // Enter LPM0 w/ interrupts enabled
  __bis_SR_register(GIE);
  while(1);
}

//  Echo back RXed character, confirm TX buffer is ready first
// #pragma vector=USCIAB0RX_VECTOR
interrupt (USCIAB0RX_VECTOR ) USCI0RX_ISR (void)
{
  char val = UCA0RXBUF;
  if (val == 'S')
  {
    pencl = ~encl;
    encl = 0;
    pencr = ~encr;
    encr = 0;
    i = 1;
    IE2 |= UCA0TXIE;
    UCA0TXBUF = pencl;
  } else if (val == 'L')
  {
    i = 4;
    IE2 |= UCA0TXIE;
    UCA0TXBUF = pencl;
  } else if (val == 'R')
  {
    i = 3;
    IE2 |= UCA0TXIE;
    UCA0TXBUF = pencr;
  }
}

interrupt (USCIAB0TX_VECTOR ) USCI0TX_ISR (void)
{
  if(i == 1){ // send L and cont to R
    UCA0TXBUF = pencl>>8;
    i = 2;
  } else if(i == 2){
    UCA0TXBUF = pencr;
    i = 3;
  } else if(i == 3){ // send R and end
    UCA0TXBUF = pencr>>8;
    IE2 &= ~UCA0TXIE;
    i = 0;
  } else if(i == 4){ // send L and end
    UCA0TXBUF = pencl>>8;
    IE2 &= ~UCA0TXIE;
    i = 0;
  }
}

interrupt (PORT2_VECTOR ) Port_2 (void)
{

  if (P2IN & BIT1)
    encl++;
  else
    encl--;
  encr++;
  P2IFG &= ~BIT0; // remove interrupt flag
  P1OUT ^= LEDL; // toggle LED

}
