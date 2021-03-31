/*
 * UART.c
 *
 *  Created on: 20 mars 2021
 *      Author: Audrey Tuffier & Alexis Adde
 */


/* ----------------------------------------------------------------------------
 * Fonction d'initialisation de l'UART
 * Entree : -
 * Sorties: -
 */

#include <msp430.h>
#include <string.h>
#include <define.h>
#include <UART.h>
#include <move.h>
#include <SPI.h>

typedef char CHAR;
typedef unsigned char UCHAR;
typedef unsigned int UINT_32;

/*static const char spi_in = 0x37*/
UCHAR cmd[CMDLEN]; /*tableau de caracteres lie a la commande user*/

void init_UART( void )		/* Initialize UART function */
  {
    P1SEL  |= (BIT1 | BIT2); /*P1.1 = RXD, P1.2=TXD*/
    P1SEL2 |= (BIT1 | BIT2); /*P1.1 = RXD, P1.2=TXD*/
    UCA0CTL1 |= UCSWRST; /*SOFTWARE RESET*/
    UCA0CTL1 |= UCSSEL_2; /*SMCLK (2 - 3)*/
    UCA0BR0 = 104; /*104 1MHz, OSC16, 9600 (8Mhz : 52) : 8 115k - 226/12Mhz*/
    UCA0BR1 = 0; /*1MHz, OSC16, 9600 - 4/12Mhz*/
    UCA0MCTL = 10;
    UCA0CTL0 &= ~(UCPEN  | UCMSB | UCDORM);
    UCA0CTL0 &= ~(UC7BIT | UCSPB  | UCMODE_3 | UCSYNC); /*dta:8 stop:1 usci_mode3uartmode*/
    UCA0CTL1 &= ~UCSWRST; /***Initialize USCI state machine***/
    /* Enable USCI_A0 RX interrupt */
    IE2 |= UCA0RXIE;
  }


/* ----------------------------------------------------------------------------
 *
 * Fonction d'emission d'une chaine de caracteres
 * Entree : pointeur sur chaine de caracteres
 * Sorties:  -
 *
 * ----------------------------------------------------------------------------
 */
void envoi_msg_UART(CHAR *msg)	/* Function to send a message via UART */
  {
  UINT_32 i = 0U;
  for(i=0U ; msg[i] != 0x00 ; i++)
      {
      while(!(IFG2 & UCA0TXIFG)) /*attente de fin du dernier envoi (UCA0TXIFG à 1 quand UCA0TXBUF vide)*/
        {
	  }
	UCA0TXBUF=msg[i];
      }
  }


/* ----------------------------------------------------------------------------
 * 
 * Fonction d'interpretation des commandes utilisateur
 * Entrees: -
 * Sorties:  -
 *
 * ----------------------------------------------------------------------------
 */
void interpreteur( void ) /*function to interpret command */
  {
  if(strcmp((const char *)cmd, "h") == 0) /*help*/
    {
        envoi_msg_UART("\r\nCommandes :");
        envoi_msg_UART("\r\n'ver' : Version");
        envoi_msg_UART("\r\n'h' : Help");
        envoi_msg_UART("\r\n'0' : LED off");
        envoi_msg_UART("\r\n'1' : LED on");
        envoi_msg_UART("\r\n'3' : Auto mode");
        envoi_msg_UART("\r\n'4' : Servomotor on");
        envoi_msg_UART("\r\n'5' : Servomotor off");
        envoi_msg_UART("\r\n'a' : Forward");
        envoi_msg_UART("\r\n's' : Stop");
        envoi_msg_UART("\r\n'q' : Left");
        envoi_msg_UART("\r\n'd' : Right");
    }
  else if (strcmp((const char *)cmd, "0") == 0) /* turn off led */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x30); /*Send '0' over SPI to Slave*/
        P1OUT &= ~BIT0; /*turn off LED*/
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "1") == 0) /* turn on led */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x31); /*Send '1' over SPI to Slave*/
        P1OUT |= BIT0; /*turn on LED*/
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "ver") == 0) /* version */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART(RELEASE);
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "a") == 0) /* move forward */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        avancer();
        envoi_msg_UART(" SamBot move forward\r\n");
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "s") == 0) /* stop */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        arretRobot();
        envoi_msg_UART(" SamBot stop\r\n");
        envoi_msg_UART("\r\n");
    } 
  else if (strcmp((const char *)cmd, "d") == 0) /* right */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        tournerDroite();
        envoi_msg_UART(" Turn right\r\n");
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "q") == 0) /* left */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        tournerGauche();
        envoi_msg_UART(" Turn Left\r\n");
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "3") == 0) /* auto mode */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        moveAuto();
        Send_char_SPI(0x0A); /*Send 10 to SPI : move servo*/
        envoi_msg_UART(" SamBot Auto\r\n");
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "4") == 0) /*servo on */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x0A); /*Send 10 to SPI : move servo*/
        envoi_msg_UART(" Servomotor on\r\n");
        envoi_msg_UART("\r\n");
    }
  else if (strcmp((const char *)cmd, "5") == 0) /* servo off */
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((UCHAR *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x0B); /*Send 11 to SPI : stop servo*/
        envoi_msg_UART(" Servomotor off\r\n");
        envoi_msg_UART("\r\n");
    }
  else /*default choice*/
    {
        envoi_msg_UART("\r\n ?"); /* repond un ? */
        envoi_msg_UART((UCHAR *)cmd);
    }
  envoi_msg_UART(PROMPT); /*command prompt*/
}
