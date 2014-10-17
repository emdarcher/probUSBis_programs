/* fades the and RGB LED */

#include "main.h"
#include "probUSBis.h"

/* defines */

#define RGB_DDRx    DDRB
#define RGB_PORTx   PORTB
#define R_PORT_BIT  0 
#define G_PORT_BIT  1
#define B_PORT_BIT  2
#define RGB_PORT_MASK ((1<<R_PORT_BIT)|(1<<G_PORT_BIT)|(1<<B_PORT_BIT))

#define R_FLAG 0
#define G_FLAG 1
#define B_FLAG 2
#define RGB_FLAG_MASK ((1<<R_FLAG)|(1<<G_FLAG)|(1<<B_FLAG))
#define PWM_DIR_FLAG 3

#define OVF0_CNT_TOP 8

/* function prototypes */
static inline void init_tim0(void);
//static inline void set_clk_div16(void);
static inline void init_RGB_stuff(void);


/*globals*/
volatile uint8_t pwm_fade=0xFF;
volatile uint8_t my_flags;
volatile uint8_t pwm_store;
volatile uint8_t ovf0_cnt = OVF0_CNT_TOP;
volatile uint8_t rgb_toggle_store;

void main(void){

    /* init stuff */
    //set_clk_div16();
    
    /* to divide 16.5MHz clk by 2*/
    clock_prescale_set(clock_div_2);
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
#if !defined(PROBUSBIS_H) 
static inline void set_clk_div16(void){
/* sets the cpu clk to be the system clk/16 */
    #if !defined(_AVR_POWER_H_)
    CLKPR = (1<<CLKPCE); //set CLK prescaler enable bit
    CLKPR = (1<<CLKPS2); // set the prescaler to clk/16
    #else
    clock_prescale_set(clock_div_16);
    #endif
}
#endif
static inline void init_tim0(void){
/* stuff to initialize timer0 for things */
//    TCCR0A |= (1<<WGM01)|(1<<WGM00); //set to fast PWM mode
//    TCCR0A |= (1<<COM0B1); //set clear on compare match for OC0B
   // TCCR0B |= (1<<CS01)|(1<<CS00); //set to clk/64 prescaler 
    TCCR0B |= (1<<CS02); //set to clk/256 prescaler
    TIMSK |= (1<<TOIE0); //enable timer0 overflow interrupt
    TIMSK |= (1<<OCIE0B); //enable timer0 compare match B interrupt
}

static inline void init_RGB_stuff(void){
/* stuff to initialize the pins and stuff for the RGB LED */
    RGB_DDRx |= (1<<R_PORT_BIT)|(1<<G_PORT_BIT)|(1<<B_PORT_BIT);
    /* set red to be on first ? */
    RGB_PORTx |= (1<<R_PORT_BIT);
    /* set flag bit for R */
    my_flags |= (1<<R_FLAG);
    rgb_toggle_store |= (1<<R_PORT_BIT)|(1<<B_PORT_BIT);
}

/* Interrupt Service Routines */

ISR(TIM0_OVF_vect){
/* Timer0 overflow interrupt service routine */

    --ovf0_cnt;
    
    if(!ovf0_cnt){ 
        ovf0_cnt = OVF0_CNT_TOP;   
   
        /* decrement the pwm value, this will go until zero, 
        then jump to 255, then keep decrementing */
        --pwm_fade; 
    
        /*if the PWM direction flag is set, then output 255-pwm_value, else output pwm_value*/
        //pwm_store = (bit_is_set(my_flags, PWM_DIR_FLAG)) ? (255-pwm_fade) : pwm_fade; 
        pwm_store = pwm_fade;
        /* if the pwm_value has reached zero, then toggle the pwm direction flag */
        if(!pwm_fade){
            my_flags ^= (1<<PWM_DIR_FLAG);
            //rotate selected color
            #if 0 
            if(bit_is_set(my_flags, B_FLAG)){
                my_flags &= ~(1<<B_FLAG);
                my_flags |= (1<<R_FLAG);
                rgb_toggle_store = (1<<R_PORT_BIT)|(1<<B_PORT_BIT); 
            } else {
                rgb_toggle_store = (bit_is_set(my_flags,R_FLAG)) ? 
                                        (rgb_toggle_store | (1<<G_PORT_BIT)): 
                                        (rgb_toggle_store << 1);
                my_flags = ((my_flags & RGB_FLAG_MASK)<<1) | (my_flags & ~RGB_FLAG_MASK);  
            }            
            PORTB = ((my_flags & RGB_FLAG_MASK)>>R_FLAG) | (PORTB & ~RGB_PORT_MASK);
            #endif
            
            if(bit_is_set(my_flags,R_FLAG)){
                my_flags = ((my_flags & RGB_FLAG_MASK)<<1) | (my_flags & ~RGB_FLAG_MASK);
                rgb_toggle_store |= (1<<G_PORT_BIT);
                PORTB &= (1<<R_PORT_BIT);
                PORTB |= (1<<G_PORT_BIT);
            } else if(bit_is_set(my_flags,G_FLAG)){
                my_flags = ((my_flags & RGB_FLAG_MASK)<<1) | (my_flags & ~RGB_FLAG_MASK);
                rgb_toggle_store &= (1<<R_PORT_BIT);
                rgb_toggle_store |= (1<<B_PORT_BIT);
                PORTB &= (1<<G_PORT_BIT);
                PORTB |= (1<<B_PORT_BIT);
            } else if(bit_is_set(my_flags,B_FLAG)){
                my_flags &= ~(1<<B_FLAG);
                my_flags |= (1<<R_FLAG);
                rgb_toggle_store &= (1<<G_PORT_BIT);
                rgb_toggle_store |= (1<<R_PORT_BIT);
                PORTB &= (1<<B_PORT_BIT);
                PORTB |= (1<<R_PORT_BIT);
            }
            
        } 
    }
}

ISR(TIM0_COMPB_vect){
/* Timer0 compare match B interrupt service routine */
    
        PORTB ^= rgb_toggle_store;
}
