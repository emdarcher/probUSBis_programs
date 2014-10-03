/* code blinks the onboard led on PB1 */

#include "main.h"

/* defines */

/* function prototypes */

void main(void){

    /* init stuff */
    DDRB |= (1<<PB1);    
    /* infinite loop */
    while(1){
        PORTB ^= (1<<PB1);
        _delay_ms(500);
    }
}

/* function bodies */



/* Interrupt Service Routines */



