/* fades the led on PB1 */

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

    sei();/* enable interrupts */     
    /* infinite loop */
    while(1){
       //do nothin' 
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
    TCCR0A |= (1<<WGM01)|(1<<WGM00); //set to fast PWM mode
    TCCR0A |= (1<<COM0B1); //set clear on compare match for OC0B
    TCCR0B |= (1<<CS01)|(1<<CS00); //set to clk/64 prescaler 
    TIMSK |= (1<<TOIE0); //enable timer0 overflow interrupt
}

/* Interrupt Service Routines */

ISR(TIM0_OVF_vect){
/* Timer0 overflow interrupt service routine */
    
    /* decrement the pwm value, this will go until zero, 
    then jump to 255, then keep decrementing */
    --pwm_fade_OCR0B; 
    /* if the PWM direction flag is set, then output 255-pwm_value, else output pwm_value */
    OCR0B = (bit_is_set(my_flags, PWM_OCR0B_DIR_BIT)) ? (255-pwm_fade_OCR0B) : pwm_fade_OCR0B; 
    /* if the pwm_value has reached zero, then toggle the pwm direction flag */
    if(!pwm_fade_OCR0B){
        my_flags ^= (1<<PWM_OCR0B_DIR_BIT);
    } 
}


