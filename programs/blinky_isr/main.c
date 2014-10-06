

#include "main.h"

/* defines */

/* global variables */

/* function prototypes */
static inline void set_clk_div16(void);
static inline void init_tim0(void);

void main(void){

    /* init stuff */
    DDRB |= (1<<PB1);
    set_clk_div16();
    init_tim0();
    sei(); 
    /* infinite loop */
    while(1){
        
    }
}

/* function bodies */

static inline void set_clk_div16(void){
/* sets the cpu clk to be the system clk/16 */
    CLKPR = (1<<CLKPCE); //set CLK prescaler enable bit
    CLKPR = (1<<CLKPS2); // set the prescaler to clk/16
}

static inline void init_tim0(void){
/* stuff to initialize timer0 for things */
    TCCR0B |= (1<<CS02)|(1<<CS00); //set to clk/1024 prescaler 
    TIMSK |= (1<<TOIE0); //enable timer0 overflow interrupt
}

/* Interrupt Service Routines */

ISR(TIM0_OVF_vect){
    PINB |= (1<<PB1); //toggles PB1 on newer AVRs
}
