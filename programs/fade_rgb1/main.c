/* fades the and RGB LED */

#include "main.h"
#include "probUSBis.h"

/* defines */

#define RGB_DDRx    DDRB
#define RGB_PORTx   PORTB
#define RGB_PINx    PINB
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

/* 0 is common-anode, 1 is common cathode */
#define RGB_LED_TYPE 0 

/* function prototypes */
static inline void init_tim0(void);
static inline void init_RGB_stuff(void);


/*globals*/
volatile uint8_t pwm_fade=0xFF;
volatile uint8_t my_flags;
//volatile uint8_t pwm_store;
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
    //RGB_PORTx |= (1<<G_PORT_BIT);
    /* set flag bit for R */
    my_flags |= (1<<R_FLAG);
    //my_flags |= (1<<G_FLAG);
    rgb_toggle_store |= (1<<R_PORT_BIT)|(1<<B_PORT_BIT);
    //rgb_toggle_store |= (1<<R_PORT_BIT)|(1<<G_PORT_BIT);
}

/* Interrupt Service Routines */

ISR(TIM0_OVF_vect){
/* Timer0 overflow interrupt service routine */
    //if(bit_is_set(my_flags, PWM_DIR_FLAG)){ 
    RGB_PORTx ^= rgb_toggle_store;
    //}
    //--ovf0_cnt;
    
    //if(!ovf0_cnt){ 
    //    ovf0_cnt = OVF0_CNT_TOP;   
   
        /* decrement the pwm value, this will go until zero, 
        then jump to 255, then keep decrementing */
        --pwm_fade; 
        uint8_t pwm_store;
        /*if the PWM direction flag is set, then output 255-pwm_value, else output pwm_value*/
        //pwm_store = (bit_is_set(my_flags, PWM_DIR_FLAG)) ? (255-pwm_fade) : pwm_fade; 
        pwm_store = pwm_fade;
        /* if the pwm_value has reached zero, then toggle the pwm direction flag */
        if(!pwm_fade){

            //pwm_store++;            

            my_flags ^= (1<<PWM_DIR_FLAG);
            

            RGB_PORTx ^= rgb_toggle_store;
            //RGB_PORTx &= ~RGB_PORT_MASK;

            //rotate selected color
            uint8_t flag_temp   =   my_flags;
            uint8_t tglstr_temp =   rgb_toggle_store;
            uint8_t rgbport_temp = RGB_PORTx;
            if(bit_is_set(my_flags,R_FLAG)){
                flag_temp &=    ~(1<<R_FLAG);
                flag_temp |=    (1<<G_FLAG);
                tglstr_temp &=  ~(1<<B_PORT_BIT);
                tglstr_temp |=  (1<<G_PORT_BIT);
                rgbport_temp &=    ~(1<<R_PORT_BIT);
                rgbport_temp |=    (1<<G_PORT_BIT);
            } else if(bit_is_set(my_flags,G_FLAG)){
                flag_temp &=    ~(1<<G_FLAG);
                flag_temp |=    (1<<B_FLAG);
                tglstr_temp &=  ~(1<<R_PORT_BIT);
                tglstr_temp |=  (1<<B_PORT_BIT);
                rgbport_temp &=    ~(1<<G_PORT_BIT);
                rgbport_temp |=    (1<<B_PORT_BIT);
            } else if(bit_is_set(my_flags,B_FLAG)){
                flag_temp &=    ~(1<<B_FLAG);
                flag_temp |=    (1<<R_FLAG);
                tglstr_temp &=  ~(1<<G_PORT_BIT);
                tglstr_temp |=  (1<<R_PORT_BIT);
                rgbport_temp &=    ~(1<<B_PORT_BIT);
                rgbport_temp |=    (1<<R_PORT_BIT);
            }
            
            my_flags = flag_temp;
            RGB_PORTx = rgbport_temp;
            rgb_toggle_store = tglstr_temp;
        }
        //#define RGB_LED_TYPE 1
       //if(bit_is_set(my_flags, PWM_DIR_FLAG)){
        #if RGB_LED_TYPE==1
        OCR0B = 255-pwm_store;
        #elif RGB_LED_TYPE==0
        OCR0B = pwm_store;
        #endif
        //} 
        /*else {
            RGB_PORTx ^= rgb_toggle_store;
            #if RGB_LED_TYPE==0
            OCR0B = 255-pwm_store;
            #elif RGB_LED_TYPE==1
            OCR0B = pwm_store;
            #endif
        }*/
    //}
    //TIFR |= (1<<OCF0B);  //clear the pending interrupt
    
    //RGB_PORTx ^= rgb_toggle_store;

}

ISR(TIM0_COMPB_vect){
/* Timer0 compare match B interrupt service routine */
    
        RGB_PORTx ^= rgb_toggle_store;
}
