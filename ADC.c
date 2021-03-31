
#include <msp430.h>

typedef unsigned char UCHAR;
typedef unsigned int UINT_32;

void ADC_init(void) /* initialisation UART */
  {
	  ADC10CTL0 = ADC10CTL1 = 0;

/* Choix de la référence de tension Vcc GND */
/* Référence interne active et générateur à 2,5 Volts  ADC10 actif */
/* Les autres bits sont supposés à 0 */

	  ADC10CTL0 =  SREF_0 + ADC10SHT_0  + REF2_5V + REFON + ADC10ON;  

/* Choix du diviseur par 1 pour l'horloge, démarrage conversion logiciel */
/* Horloge de conversion 1MHz, conversion monovoie-monocoup */	

	  ADC10CTL1 =  ADC10DIV_0 + ADC10SSEL_2 +  SHS_0 + CONSEQ_0 ;
  }



void ADC_Demarrer_conversion(UCHAR voie) /* démarrer conversion */
  {
     ADC10CTL1 = (voie * 0x1000U)+ ADC10DIV_0 + ADC10SSEL_2 +  SHS_0 + CONSEQ_0 ;
     ADC10CTL0 |= ENC + ADC10SC;     /* Sampling and conversion start */
  }  


UINT_32 ADC_Lire_resultat(void) /* lecture ADC */
  {
  	while (ADC10CTL1 & ADC10BUSY)	/* Tant que ADC occupé on attend */
	  {
	  }
	ADC10CTL0 &= ~ENC;		/* Conversion finie alors Disable ADC conversion */

    	return ADC10MEM;	        /* Return Conversion value */
  }
