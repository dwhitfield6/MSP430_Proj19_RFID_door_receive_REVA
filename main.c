#include <msp430.h>
#include "msprf24.h"
#include "nrf_userconfig.h"
#include "stdint.h"

#define LED0 BIT3  //1.5 is SPI clock
#define LED1 BIT6  //1.5 is SPI clock
#define motorF BIT5  //1.5 is SPI clock
#define motorR BIT7  //1.5 is SPI clock
#define vibrate BIT4  //1.5 is SPI clock


volatile unsigned int user;
unsigned char doorvalue =0;
unsigned long motorvalue =100000;
int ledvalue =1000;
int dooropen=1;
unsigned long motordelay = 100000;
unsigned int vibratecount = 1000;

unsigned long count =0;

int main()
{
  uint8_t addr[5];

  WDTCTL = WDTHOLD | WDTPW;
  DCOCTL = CALDCO_16MHZ;
  BCSCTL1 = CALBC1_16MHZ;
  BCSCTL2 = DIVS_1;  // SMCLK = DCOCLK/2
  // SPI (USCI) uses SMCLK, prefer SMCLK < 10MHz (SPI speed limit for nRF24 = 10MHz)

  P1DIR |= (LED1 + motorR);
  P2DIR |= (LED0 + motorF + vibrate);
  P1OUT &= ~(LED1 + motorR);
  P2OUT &= ~(LED0 + motorF);
  P2OUT |=  + vibrate;

  /* Initial values for nRF24L01+ library config variables */
  // rf_crc = RF24_EN_CRC | RF24_CRCO; // CRC enabled, 16-bit
  rf_addr_width      = 5;
  rf_speed_power     = RF24_SPEED_2MBPS | RF24_POWER_0DBM;
  rf_channel         = 124;

  msprf24_init();
  msprf24_set_pipe_packetsize(0, 1);
  msprf24_open_pipe(0, 0);  // Open pipe#0 with Enhanced ShockBurst

  // Set our RX address
  addr[0] = 0xDE;
  addr[1] = 0xAD;
  addr[2] = 0xBE;
  addr[3] = 0xEF;
  addr[4] = 0x00;
  w_rx_addr(0, addr);

  // Receive mode
  if (!(RF24_QUEUE_RXEMPTY & msprf24_queue_state())) {
    flush_rx();
  }
  msprf24_activate_rx();


  while (1) {
     if (rf_irq & RF24_IRQ_FLAGGED) {
       rf_irq &= ~RF24_IRQ_FLAGGED;
     msprf24_get_irq_reason();
      }
    if (rf_irq & RF24_IRQ_RX || msprf24_rx_pending())
    {
      r_rx_payload(1, &doorvalue);
if(motorvalue >(motordelay-20))
{
	if(dooropen ==0)
	{
      if(doorvalue ==2)
      {
    	  P1OUT |= motorR;
    	  P2OUT &= ~motorF;
    	  P2OUT &= ~(vibrate);
    	  dooropen = 1;
    	  motorvalue =0;
    	  vibratecount =0;

      }
	}
	else
	{
      if(doorvalue ==1) //close
      {
    	  P1OUT &= ~motorR;
    	  P2OUT |= motorF;
    	  P2OUT &= ~(vibrate);
    	  dooropen =0;
    	  motorvalue =0;
    	  vibratecount =0;
      }
	}


}
if(doorvalue != 0)
{
	ledvalue =0;
	P1OUT |= (LED1);
	P2OUT &= ~(LED0);
}
doorvalue =0;

    }

    __delay_cycles(2000);

    if(motorvalue == (motordelay -20))
    {
    	P2OUT &= ~motorF;
    	P1OUT &= ~motorR;
    }

    if(vibratecount == 800)
    {
    	P2OUT |= (vibrate);
    }
    vibratecount++;
    if(vibratecount >2000)
    {
    	vibratecount =2000;
    }

    if(ledvalue == 990)
    {
        P1OUT &= ~(LED1);
        P2OUT |= (LED0);
    }
    ledvalue++;
    if(ledvalue >1000)
    {
    	ledvalue =1000;
    }
    motorvalue++;
    if(motorvalue >motordelay)
    {
    	motorvalue =motordelay;
    }

  }
}

