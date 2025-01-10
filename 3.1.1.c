/* Name: 3.1.2.c
 * Author: Vir Goyal
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h" //  library used for printing

void setupADC(int ADCchnl){
  set(ADMUX,REFS0);//vcc
  set(ADCSRA,ADPS0);//1/128
  set(ADCSRA,ADPS1);
  set(ADCSRA,ADPS2);

  if(ADCchnl==0){
    set(DIDR0,ADC0D);//ADC0 disabling digital input
    clear (ADMUX,MUX0);// selecting single ended channel
    clear(ADMUX,MUX1);
    clear(ADMUX,MUX2);
    clear(ADCSRB,MUX5);
  }
  if(ADCchnl==1){
    set(DIDR0,ADC1D);//ADC1 disabling digital input
    // selecting single ended channel
    set (ADMUX,MUX0);
    clear(ADMUX,MUX1);
    clear(ADMUX,MUX2);
    clear(ADCSRB,MUX5);
  }
   if(ADCchnl==4){
    set(DIDR0,ADC4D);//ADC4 disabling digital input
    clear (ADMUX,MUX0);// selecting single ended channel
    clear(ADMUX,MUX1);
    set(ADMUX,MUX2);
    clear(ADCSRB,MUX5);
  }
   if(ADCchnl==5){
    set(DIDR0,ADC5D);//ADC5 disabling digital input
    set (ADMUX,MUX0);// selecting single ended channel
    clear(ADMUX,MUX1);
    set(ADMUX,MUX2);
    clear(ADCSRB,MUX5);
  }
   if(ADCchnl==6){
    set(DIDR0,ADC6D);//ADC6 disabling digital input
    clear (ADMUX,MUX0);// selecting single ended channel
    set(ADMUX,MUX1);
    set(ADMUX,MUX2);
    clear(ADCSRB,MUX5);
  }

  if(ADCchnl==7){
    set(DIDR0,ADC6D);//ADC7 disabling digital input
    set (ADMUX,MUX0);// selecting single ended channel
    set(ADMUX,MUX1);
    set(ADMUX,MUX2);
    clear(ADCSRB,MUX5);
  }
  if(ADCchnl==8){
    set(DIDR2,ADC8D);//ADC8 disabling digital input
    set(ADCSRB,MUX5);// selecting single ended channel
    clear (ADMUX,MUX0);
    clear(ADMUX,MUX1);
    clear(ADMUX,MUX2);
  }
  if(ADCchnl==9){
    set(DIDR2,ADC9D);//ADC9 disabling digital input
    set(ADCSRB,MUX5);// selecting single ended channel
    set (ADMUX,MUX0);
    clear(ADMUX,MUX1);
    clear(ADMUX,MUX2);
  }
   if(ADCchnl==10){
    set(DIDR2,ADC10D);//ADC10 disabling digital input
    clear (ADMUX,MUX0);// selecting single ended channel
    set(ADMUX,MUX1);
    clear(ADMUX,MUX2);
    set(ADCSRB,MUX5);
  }
   if(ADCchnl==11){
    set(DIDR2,ADC11D);//ADC11 disabling digital input
    set (ADMUX,MUX0);// selecting single ended channel
    set(ADMUX,MUX1);
    clear(ADMUX,MUX2);
    set(ADCSRB,MUX5);
  }
  if(ADCchnl==12){
    set(DIDR2,ADC12D);//ADC9 disabling digital input
    clear (ADMUX,MUX0);// selecting single ended channel
    clear(ADMUX,MUX1);
    set(ADMUX,MUX2);
    set(ADCSRB,MUX5);
  }

  if(ADCchnl==13){
    set(DIDR2,ADC13D);//ADC9 disabling digital input
    set (ADMUX,MUX0);// selecting single ended channel
    clear(ADMUX,MUX1);
    set(ADMUX,MUX2);
    set(ADCSRB,MUX5);

  }
  
  set(ADCSRA,ADEN); // enabling the ADC subsystem
  set(ADCSRA,ADSC);// start conversion
  set(ADCSRA,ADIF); // reading the result
}

 int ADCread(int ADCchnl){ 
    int tadc; // temporarily stores ADC value
    setupADC(ADCchnl);
    while(!bit_is_set(ADCSRA,ADIF)); // wait for bit to be set
    set(ADCSRA,ADIF);
    m_usb_tx_string( "potentiometer");m_usb_tx_uint(ADCchnl); m_usb_tx_string( "value = ");
    m_usb_tx_uint(ADC);
    m_usb_tx_string( "\n ");
    tadc=ADC;
    set(ADCSRA,ADSC);
    return tadc;

 }

int main (void){
  m_usb_init();
  for (;;){
    ADCread(5);
  }
  return 0; // never reached
}
