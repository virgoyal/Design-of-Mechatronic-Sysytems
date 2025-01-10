/* Name: 3.1.2.c
 * Author: Vir Goyal
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h" //  library used for printing

void setupADC(int ADCchnl){
  set(ADMUX,REFS0);//vcc
  clear(ADMUX,REFS1);
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
  
}
  int ADCreadv(){ 

    int tadc; // temporarily stores ADC value
    set(ADCSRA,ADEN); // enabling the ADC subsystem
    set(ADCSRA,ADSC);// start conversion
    
    while(!bit_is_set(ADCSRA,ADIF)); // wait for bit to be set
    tadc=ADC;
    return tadc;
    set(ADCSRA,ADIF); // clear the conversion flag
    set(ADCSRA,ADSC); //start converting again        

 }

  

int main (void){
 
m_usb_init();


int door;
int lock;
  for (;;){
    setupADC(); // initialise ADC
    door=ADCreadv();
    m_usb_tx_string( "door = ");
    m_usb_tx_uint(door);
    m_usb_tx_string( "\t ");
    setupADC(5);
    lock =ADCreadv();  // initialise ADC
    m_usb_tx_string( "lock = ");
    m_usb_tx_uint(lock);
    m_usb_tx_string( "\n ");

    set(DDRC,6);  
    set(PORTC,6);

    set(DDRB,5);
    set(PORTB,5);

    _clockdivide(0);

  // set timer on OCR3A mode 7
    set(TCCR3B,WGM32); 
    set(TCCR3A,WGM30);
    clear(TCCR3B,WGM33);
    set(TCCR3A,WGM31);

    set(TCCR3B, CS30);
    clear(TCCR3B, CS31);
    set(TCCR3B, CS32); // set prescaler to /1024


  //clear at OCR3A, set at rollover
  set (TCCR3A,COM3A1); 
  clear (TCCR3A,COM3A0);


  // set timer on OCR1B mode 7
  set(TCCR1B,WGM12); 
  set(TCCR1A,WGM10);
  clear(TCCR1B,WGM13);
  set(TCCR1A,WGM11);

  set (TCCR1B,CS10); //Setting prescaler to 1/1024
  set (TCCR1B,CS12);
  clear (TCCR1B,CS11);

  //clear at OCR1A, set at rollover
  set (TCCR1A,COM1A1); 
  clear (TCCR1A,COM1A0);

  float compareval_door = 0.055*door;
  OCR1A = compareval_door;

  float compareval_lock = 0.055*lock;
  OCR3A = compareval_lock;

  }
  return 0; // never reached
}
