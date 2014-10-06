

#include "main.h"

/* defines */

#define PWM_OCR0B_DIR_BIT 0 

/* function prototypes */
static inline void init_tim0(void);
static inline void set_clk_div16(void);

/*globals*/
volatile uint8_t pwm_fade_OCR0B=0xFF;
volatile uint8_t my_flags;

void main(void){

    /* init stuff */

    set_clk_div16();
    
    DDRB |= (1<<PB1);
    
    init_tim0();

    sei();     
    /* infinite loop */
    while(1){
        
    }
}

/* function bodies */

static inline void set_clk_div16(void){
    CLKPR = (1<<CLKPCE);
    CLKPR |= (1<<CLKPS2);
}

static inline void init_tim0(void){
    
    TCCR0A |= (1<<WGM01)|(1<<WGM00); //set to fast PWM mode
    TCCR0A |= (1<<COM0B1); //set clear on compare match for OC0B

    TCCR0B |= (1<<CS02)|(1<<CS00); //set to clk/1024 prescaler 
    
    TIMSK |= (1<<TOIE0); //enable timer0 overflow interrupt
    
}

/* Interrupt Service Routines */

ISR(TIM0_OVF_vect){
    
    --pwm_fade_OCR0B;
    OCR0B = (bit_is_set(my_flags, PWM_OCR0B_DIR_BIT)) ? (255-pwm_fade_OCR0B) : (pwm_fade_OCR0B) ; 
    if(!pwm_fade_OCR0B){
        my_flags ^= (1<<PWM_OCR0B_DIR_BIT);
    }
    
}


