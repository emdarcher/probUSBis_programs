/* fades the and RGB LED */

#include "main.h"
#include "probUSBis.h"

/* defines */

#define RGB_DDRx    DDRB
#define RGB_PORTx   PORTB
#define R_PORT_BIT  0 
#define G_PORT_BIT  1
#define B_PORT_BIT  2

#define R_FLAG 0
#define G_FLAG 1
#define B_FLAG 2
#define PWM_DIR_FLAG 3

/* function prototypes */
static inline void init_tim0(void);
static inline void set_clk_div16(void);
static inline void init_RGB_stuff(void);


/*globals*/
volatile uint8_t pwm_fade=0xFF;
volatile uint8_t my_flags;
volatile uint8_t pwm_store;

void main(void){

    /* init stuff */
    set_clk_div16();
//    DDRB |= (1<<PB1);
    init_RGB_stuff();
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
    #if !defined(_AVR_POWER_H_)
    CLKPR = (1<<CLKPCE); //set CLK prescaler enable bit
    CLKPR = (1<<CLKPS2); // set the prescaler to clk/16
    #else
    clock_prescale_set(clock_div_16);
    #endif
}

static inline void init_tim0(void){
/* stuff to initialize timer0 for things */
//    TCCR0A |= (1<<WGM01)|(1<<WGM00); //set to fast PWM mode
//    TCCR0A |= (1<<COM0B1); //set clear on compare match for OC0B
    TCCR0B |= (1<<CS01)|(1<<CS00); //set to clk/64 prescaler 
    TIMSK |= (1<<TOIE0); //enable timer0 overflow interrupt
    TIMSK |= (1<<OCIE0B); //enable timer0 compare match B interrupt
}

static inline void init_RGB_stuff(void){
/* stuff to initialize the pins and stuff for the RGB LED */
    RGB_DDRx |= (1<<R_PORT_BIT)|(1<<G_PORT_BIT)|(1<<B_PORT_BIT);
    /* set red to be on first ? */
    RGB_PORTx |= (1<<R_PORT_BIT);
}

/* Interrupt Service Routines */

ISR(TIM0_OVF_vect){
/* Timer0 overflow interrupt service routine */
    
    /* decrement the pwm value, this will go until zero, 
    then jump to 255, then keep decrementing */
    --pwm_fade; 
    /* if the PWM direction flag is set, then output 255-pwm_value, else output pwm_value */
    pwm_store = (bit_is_set(my_flags, PWM_OCR0B_DIR_BIT)) ? (255-pwm_fade_OCR0B) : pwm_fade_OCR0B; 
    /* if the pwm_value has reached zero, then toggle the pwm direction flag */
    if(!pwm_fade){
        my_flags ^= (1<<PWM_DIR_FLAG);
    } 
}

ISR(TIM0_COMPB_vect){
/* Timer0 compare match B interrupt service routine */
    
    
}
