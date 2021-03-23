/*
 * -----------------------------------------------------
 *  P R O G R A M M A T I O N   S P I   E S I G E L E C
 *
 * Lauchpad v1.5 support
 *
 * M S P 4 3 0  G 2 5 5 3   -   SPI/MASTER 3 Wires
 *
 * (c)-Yann DUCHEMIN / ESIGELEC - r.III162018 for CCS
 * --------------------------------------------------------------
 *
 * La carte Launchpad est raccord�e en SPI via l'USI B0
 * SCLK : P1.5 / UCB0CLK, clock in/out
 * SIMO : P1.7 / UCB0SIMO, master OUT
 * MOSI : P1.6 / UCB0SOMI, master IN
 *
 * A la reception du caract�re 1 sur l'UART,
 *  le caract�re est transmis sur le bus SPI,
 *  et affich� en echo sur l'UART
 *
 * A la reception du caract�re 0 sur l'UART,
 *  le caract�re est transmis sur le bus SPI,
 *  et affich� en echo sur l'UART
 *
 */

#include <msp430.h> 
#include <string.h>
#include <UART.h>
#include <SPI.h>
#include <move.h>
#include <define.h>

/*
 * Variables globales
 */
// static const char spi_in = 0x37;
unsigned char cmd[CMDLEN];      // tableau de caracteres lie a la commande user
unsigned char car = 0x30;       // 0
unsigned int  nb_car = 0;
unsigned char intcmd = FALSE;   // call interpreteur()

/*
 * Prototypes
 */
void init_BOARD( void );
void init_UART( void );
void init_USCI( void );
void interpreteur( void );
void envoi_msg_UART(unsigned char * );
void Send_char_SPI( unsigned char );

void init_pwm(void);
void arretRobot(void);
void avancer(void);


/* ----------------------------------------------------------------------------
 * Fonction d'initialisation de la carte TI LauchPAD
 * Entrees: -
 * Sorties:  -
 */
void init_BOARD( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW | WDTHOLD;

    if( (CALBC1_1MHZ==0xFF) || (CALDCO_1MHZ==0xFF) )
    {
        __bis_SR_register(LPM4_bits);
    }
    else
    {
        // Factory Set.
        DCOCTL = 0;
        BCSCTL1 = CALBC1_1MHZ;
        DCOCTL = (0 | CALDCO_1MHZ);
    }

    /*/--------------- Secure mode
    P1SEL  = 0x00;        // GPIO
    P1SEL2 = 0x00;        // GPIO
    P2SEL  = 0x00;        // GPIO
    P2SEL2 = 0x00;        // GPIO
    P1DIR = 0x00;         // IN
    P2DIR = 0x00;         // IN
*/

    P1SEL  &= ~LED_R;
    P1SEL2 &= ~LED_R;
    P1DIR |= LED_R ;  // LED: output
    P1OUT &= ~LED_R ;
}


/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * main.c
 */
void main( void )
{
    init_BOARD();
    init_UART();
    init_USCI();

    envoi_msg_UART("\rReady !\r\n"); // user prompt
    envoi_msg_UART(PROMPT);        //---------------------------- command prompt

 while(1)
    {
        if( intcmd )
        {
            while ((UCB0STAT & UCBUSY));   // attend que USCI_SPI soit dispo.
            interpreteur();         // execute la commande utilisateur
            intcmd = FALSE;         // acquitte la commande en cours
        }
        else
        {
            __bis_SR_register(LPM4_bits | GIE); // general interrupts enable & Low Power Mode
        }
    }
}

// --------------------------- R O U T I N E S   D ' I N T E R R U P T I O N S

/* ************************************************************************* */
/* VECTEUR INTERRUPTION USCI RX                                              */
/* ************************************************************************* */
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR()
{
    //---------------- UART
    if (IFG2 & UCA0RXIFG)
    {
        while(!(IFG2 & UCA0RXIFG));
        cmd[nb_car]=UCA0RXBUF;         // lecture caract�re re�u

        while(!(IFG2 & UCA0TXIFG));    // attente de fin du dernier envoi (UCA0TXIFG � 1 quand UCA0TXBUF vide) / echo
        UCA0TXBUF = cmd[nb_car];

        if( cmd[nb_car] == ESC)
        {
            nb_car = 0;
            cmd[1] = 0x00;
            cmd[0] = CR;
        }

        if( (cmd[nb_car] == CR) || (cmd[nb_car] == LF))
        {
            cmd[nb_car] = 0x00;
            intcmd = TRUE;
            nb_car = 0;
            __bic_SR_register_on_exit(LPM4_bits);   // OP mode !
        }
        else if( (nb_car < CMDLEN) && !((cmd[nb_car] == BSPC) || (cmd[nb_car] == DEL)) )
        {
            nb_car++;
        }
        else
        {
            cmd[nb_car] = 0x00;
            nb_car--;
        }
    }

    //--------------- SPI
    else if (IFG2 & UCB0RXIFG)
    {
        while( (UCB0STAT & UCBUSY) && !(UCB0STAT & UCOE) );
        while(!(IFG2 & UCB0RXIFG));
        cmd[0] = UCB0RXBUF;
        cmd[1] = 0x00;
        P1OUT ^= LED_R;
    }
}
//------------------------------------------------------------------ End ISR
