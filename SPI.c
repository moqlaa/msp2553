/*
 * SPI.c
 *
 *  Created on: 20 mars 2021
 *      Author: Audrey Tuffier & Alexis Adde
 */


/* ----------------------------------------------------------------------------
 * Fonction d'initialisation de l'USCI POUR SPI SUR UCB0
 * Entree : -
 * Sorties: -
 */

#include <msp430.h>
#include <define.h>
#include <SPI.h>
#include <UART.h>

typedef unsigned char UCHAR;

/*static const char spi_in = 0x37*/
UCHAR cmd[CMDLEN]; /*tableau de caracteres lie a la commande user*/

void init_USCI( void )
  {
    /*Waste Time, waiting Slave SYNC*/
    __delay_cycles(250);

    /*SOFTWARE RESET - mode configuration*/
    UCB0CTL0 = 0;
    UCB0CTL1 = (0 + (UCSWRST*1) );

    /*clearing IFg /16.4.9/p447/SLAU144j
     *set by setting UCSWRST just before*/
    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);

    
    UCB0CTL0 |= ( UCMST | UCMODE_0 | UCSYNC );
    UCB0CTL0 &= ~( UCCKPH | UCCKPL | UCMSB | UC7BIT );
    UCB0CTL1 |= UCSSEL_2;

    UCB0BR0 = 0x0A; /*divide SMCLK by 10*/
    UCB0BR1 = 0x00;

    /*SPI : Fonctions secondaires
     *MISO-1.6 MOSI-1.7 et CLK-1.5
     *Ref. SLAS735G p48,49*/
    P1SEL  |= ( SCK | DATA_OUT | DATA_IN);
    P1SEL2 |= ( SCK | DATA_OUT | DATA_IN);

    UCB0CTL1 &= ~UCSWRST; /*activation USCI*/
  }


/* ----------------------------------------------------------------------------
 * Fonction d'envoie d'un caractère sur USCI en SPI 3 fils MASTER Mode
 * Entree : Caractère à envoyer
 * Sorties: /
 */
void Send_char_SPI(UCHAR carac) /* function to send char */
  {
    while ((UCB0STAT & UCBUSY)) /*attend que USCI_SPI soit dispo*/
      {
      }
    while(!(IFG2 & UCB0TXIFG)) /*p442*/
      {
      }
    UCB0TXBUF = carac; /*Put character in transmit buffer*/
    envoi_msg_UART((UCHAR *)cmd); /*slave echo*/
  }

