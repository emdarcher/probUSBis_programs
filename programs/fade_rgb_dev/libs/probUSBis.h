
#include <avr/io.h>

#ifndef PROBUSBIS_H
#define PROBUSBIS_H

/* things for pins and ports */

#define PROB_LED PB1

#define PROB_DDRx DDRB
#define PROB_PORTx PORTB

#define PROB_P0 PB0
#define PROB_P1 PB1
#define PROB_P2 PB2
#define PROB_P3 PB3
#define PROB_P4 PB4
#define PROB_P5 PB5

static inline void set_clk_div16(void);// __attribute__((__always_inline__));

static inline void set_clk_div16(void){// __attribute__((__always_inline__));
/* sets the cpu clk to be the system clk/16 */
    #if !defined(_AVR_POWER_H_)
    CLKPR = (1<<CLKPCE); //set CLK prescaler enable bit
    CLKPR = (1<<CLKPS2); // set the prescaler to clk/16
    #else
    clock_prescale_set(clock_div_16);
    #endif
}


#endif
