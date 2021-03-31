/* move.c
 *
 *  Created on: 20 mars 2021
 *  Author: Audrey Tuffier & Alexis Adde
 */

#include <msp430.h>
#include <define.h>
#include <move.h>
#include <ADC.h>

typedef unsigned int UINT_32;

UINT_32 vitesseGauche=0U;
UINT_32 vitesseDroite=0U;
UINT_32 seuil=400;
UINT_32 resultat=0;

void init_move (void) /*Initialiser le signal PWM pour controler la vitesse du robot*/
{
    TA1CTL = TASSEL_2 | MC_1; /*Horloge SMCLK, Mode UP, Initialisation de l'entrée TA1*/
    TA1CCTL1 |= OUTMOD_7; /*Correspond au registre TACCR1, Roue gauche*/
    TA1CCTL2 |= OUTMOD_7; /*Correspond au registre TACCR1, Roue droite*/
    TA1CCR0 = 2000; /*Ajustement de la periode du signal*/

    /*Init_rouegauche*/
    P2DIR |= BIT2; /*Initialisation roue droite*/
    P2SEL |= BIT2;
    P2SEL2 &=~ BIT2;
    P2DIR |= BIT5; /*Sens de la roue de base*/
    vitesseGauche = 500; /*Defini la vitesse de base de la roue*/

    /*Init_rouedroite*/
    P2DIR |= BIT4; /*Initialisation roue droite*/
    P2SEL |= BIT4;
    P2SEL2 &= ~BIT4;
    P2DIR &= ~BIT1; /*Sens de la roue de base*/
    vitesseDroite = 500; /*Défini la vitesse de base de la roue*/
}

void avancer(void) /* move forward */
{
    TA1CCR1 = 3000; /*Détermine le rapport cyclique du signal pour la roue gauche*/
    TA1CCR2 = 3000; /*Détermine le rapport cyclique du signal pour la roue droite*/
}

void arretRobot(void) /* stop */
{
    TA1CCR1 = 0; /*Vitesse Roue Gauche*/
    TA1CCR2 = 0; /*Vitesse Roue Droite*/
}

void tournerDroite(void) /* right */
{
    TA1CCR1 = 3000; /*Vitesse Roue Gauche*/
    TA1CCR2 = 0;	  /*Vitesse Roue Droite*/
}

void tournerGauche(void) /* left */
{
    TA1CCR1 = 0; /*Vitesse Roue Gauche*/
    TA1CCR2 = 3000; /*Vitesse Roue Droite*/
}

void demiTour(void) /* demitour */
{
    TA1CCR1 = 3000; /*Vitesse Roue Gauche*/
    TA1CCR2 = 0; /*Vitesse Roue Droite*/
    __delay_cycles(100000); /*pause*/
}

void moveAuto(void) /* auto mode */
{
    P1DIR &= ~BIT0; /*Detection d'obstacle*/

    /*Bibliothèque ADC pour la sensibilité du capteur IR*/
    ADC_Demarrer_conversion(0x00);
    resultat = ADC_Lire_resultat();

    avancer();
    /* si obstacle détecté */
    if(resultat > seuil)
    {
        tournerDroite(); /*right*/
    }
}
