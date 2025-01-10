/* Name: main.c
 * Author: Vir Goyal
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h"




int main(){
   clear (DDRC,7); //making c7 input
   set(DDRB,3); // Data Direction Register for port B and setting it to PB5.
   m_usb_init();
 
 

while(1) {
m_usb_tx_uint(bit_is_set(PINC,7));
if(bit_is_set(PINC,7)>0){
   set(PORTB,3);
}
if(bit_is_set(PINC,7)==0){
   clear(PORTB,3);


}
   
   
   
}}