

.include "tn85def.inc";

;.def and .equ things

.def regA = r16;
.def isr_toggle_reg = r17;
.def temp_pwm_reg = r15;

.def tgl_io_regA = r21;
.def tgl_io_regB = r22;
.def tfr_regA = r21;
.def pwm_fade_reg = r19;
.def my_flags_reg = r20;
.def rgb_tgl_reg = r23;
.def flag_temp = r24;
.def tglstr_temp = r25;
.def rgbport_temp = r18;

.equ PWM_DIR_BIT = 3;

.equ RGB_DDRx	= DDRB;
.equ RGB_PORTx	= PORTB;
.equ RGB_PINx	= PINB;
.equ R_PORT_BIT	= 0;
.equ G_PORT_BIT = 1;
.equ B_PORT_BIT = 2;
.equ RGB_PORT_MASK = ((1<<R_PORT_BIT)|(1<<G_PORT_BIT)|(1<<B_PORT_BIT))

.equ R_FLAG = 0;
.equ G_FLAG = 1;
.equ B_FLAG = 2;
.equ RGB_FLAG_MASK = ((1<<R_FLAG)|(1<<G_FLAG)|(1<<B_FLAG));
.equ PWM_DIR_FLAG = 3;


;****MACROS****;
.macro tgl_io
        in tgl_io_regA, @0; input the io data in first arg
        ldi  tgl_io_regB, (1<<@1); input the 2nd arg bit to reg b
        eor tgl_io_regA, tgl_io_regB; toggle the bit in other reg
        out @0, tgl_io_regA; send it back to the io PORTx in 1st arg
.endmacro

.macro tgl_io_in_reg
	in tgl_io_regA, @0; input the io data in first arg
	eor @1, tgl_io_regA; xor the stuff to toggle
	;eor tgl_io_regA, @1;
	out @0, tgl_io_regA; put the new toggled stuff into the io reg
.endmacro

.macro tgl_tfr_reg
    ldi tfr_regA, (1<<@1);toggle a bit in a register using
    eor @0, tfr_regA;      a transfer register
.endmacro

.macro tfr_to_reg
    ldi tfr_regA,@1;put 2nd arg into transfer_regA
    mov @0,tfr_regA; move that into the desired register in arg 1
    clr tfr_regA; clear the transfer register after use
.endmacro

.macro init_RGB_stuff
	;set port bits to output
	in tgl_io_regB, RGB_DDRx;
	ori tgl_io_regB, RGB_PORT_MASK;
	out RGB_DDRx, tgl_io_regB;
	;set red pin on first
	sbi RGB_PORTx, R_PORT_BIT;
	;set R flag in my_flags_reg
	ori my_flags_reg, (1<<R_FLAG);
	;set the bits in the toggle reg
	ori rgb_tgl_reg, ((1<<R_PORT_BIT)|(1<<B_PORT_BIT));
.endmacro


;.db things

.org 0000	
	rjmp RESET;
.org OVF0addr	
	rjmp TIM0_OVF;
.org OC0Baddr 	
	rjmp TIM0_COMPB;


RESET:
	;stuff do do at reset
	
	;set clock prescaler to div 2
	ldi regA, (1<<CLKPCE);
	out CLKPR, regA;
	ldi regA, (1<<CLKPS0);
	out CLKPR, regA;

	;init_RGB_stuff; call the rgb init macro
	
	;set port bits to output
        in tgl_io_regB, RGB_DDRx;
        ori tgl_io_regB, RGB_PORT_MASK;
        out RGB_DDRx, tgl_io_regB;
        ;set red pin on first
        sbi RGB_PORTx, R_PORT_BIT;
        ;set R flag in my_flags_reg
        ori my_flags_reg, (1<<R_FLAG);
        ;set the bits in the toggle reg
        ori rgb_tgl_reg, ((1<<R_PORT_BIT)|(1<<B_PORT_BIT));


	;setup timer0 stuff
	;ldi regA, (1<<CS01)|(1<<CS00); set to div/64
	ldi regA, (1<<CS02); set to clk/256 prescaler
	out TCCR0B, regA;put into TCCR0B
	ldi regA, ((1<<TOIE0)|(1<<OCIE0B));enable timer0 overflow interrupt
						;and compare match B interrupt
	in tgl_io_regB, TIMSK;
	or tgl_io_regB, regA;
	out TIMSK, tgl_io_regB;put into TIMSK
	
	sei; enable interrupts
	rjmp MAIN; ;jump to main

MAIN:
	;do stuff	
	;nop;
	rjmp MAIN; loop back again.
;things

TIM0_COMPB:

        ;tgl_io_in_reg RGB_PORTx, rgb_tgl_reg;toggle the pins 
        in  isr_toggle_reg, RGB_PORTx;
        ;eor isr_toggle_reg, rgb_tgl_reg;
	eor rgb_tgl_reg, isr_toggle_reg;
	;
        ;out RGB_PORTx, isr_toggle_reg;
        out RGB_PORTx, rgb_tgl_reg;
        reti;return from interrupt



TIM0_OVF:

	;tgl_io_in_reg RGB_PORTx, rgb_tgl_reg;toggle the pins	
	;in  isr_toggle_reg, RGB_PORTx;
        ;eor isr_toggle_reg, rgb_tgl_reg;
        ;out RGB_PORTx, isr_toggle_reg;


	dec pwm_fade_reg; dec the val
	;subi	pwm_fade_reg,1;
	mov temp_pwm_reg, pwm_fade_reg;copy to temp_pwm_reg
	;sbrc my_flags_reg, PWM_DIR_BIT;skip if pwm dir flag is set
	;	com temp_pwm_reg;do 1's complement, makes it 255-value.
	;out OCR0B, temp_pwm_reg; put the value into OCR0B

	tst pwm_fade_reg,pwm_fade_reg; test if zero or minus
	brne PWM_NOT_0;branch if not equal to zero, skip over things
	
	;these are executed if no branch occurs,
	;so if pwm_fade_reg = 0, do these things.
	;ldi regA, (1<<PWM_DIR_FLAG);put the dir flag bit in regA
	;eor my_flags_reg, regA; toggle dir flag bit in my_flags_reg
	
	inc temp_pwm_reg;
	;out OCR0B, temp_pwm_reg; put the value into OCR0B

	;tgl_io_in_reg RGB_PORTx, rgb_tgl_reg;toggle the pins  
	in  isr_toggle_reg, RGB_PORTx;
        eor isr_toggle_reg, rgb_tgl_reg;
        out RGB_PORTx, isr_toggle_reg;

	
	;make temp storage
	mov flag_temp, my_flags_reg;
	mov tglstr_temp, rgb_tgl_reg;
	in rgbport_temp, RGB_PORTx;

	sbrs my_flags_reg, R_FLAG;
		rjmp G_FLAG_SET;

R_FLAG_SET:
	andi flag_temp, ~(1<<R_FLAG);
	ori  flag_temp, (1<<G_FLAG);
	andi tglstr_temp, ~(1<<B_PORT_BIT);
	ori  tglstr_temp, (1<<G_PORT_BIT);
	andi rgbport_temp, ~(1<<R_PORT_BIT);
	ori  rgbport_temp, (1<<G_PORT_BIT);
		rjmp END_JUMP_TREE;	

G_FLAG_SET:
	sbrs my_flags_reg, G_FLAG;
		rjmp B_FLAG_SET;
	andi flag_temp, ~(1<<G_FLAG);
	ori  flag_temp,	(1<<B_FLAG);
	andi tglstr_temp, ~(1<<R_PORT_BIT);
	ori  tglstr_temp, (1<<B_PORT_BIT);
	andi rgbport_temp, ~(1<<G_PORT_BIT);
	ori  rgbport_temp, (1<<B_PORT_BIT);
		rjmp END_JUMP_TREE;

B_FLAG_SET:
	sbrs my_flags_reg, B_FLAG;
		rjmp END_JUMP_TREE;
	andi flag_temp,  ~(1<<B_FLAG);
	ori  flag_temp, (1<<R_FLAG);
	andi tglstr_temp, ~(1<<G_PORT_BIT);
	ori  tglstr_temp, (1<<R_PORT_BIT);
	andi rgbport_temp, ~(1<<B_PORT_BIT);
	ori  rgbport_temp, (1<<R_PORT_BIT);
		rjmp END_JUMP_TREE;	

END_JUMP_TREE:
	mov my_flags_reg, flag_temp;
	out RGB_PORTx, rgbport_temp;
	mov rgb_tgl_reg, tglstr_temp;

	out OCR0B, temp_pwm_reg;	
	;out OCR0B, pwm_fade_reg;
	;in  isr_toggle_reg, RGB_PORTx;
        ;eor isr_toggle_reg, rgb_tgl_reg;
        ;out RGB_PORTx, isr_toggle_reg;


PWM_NOT_0: ; the branch skips to here 
	nop; a nop to do something before returning
	reti;return from interrupt

;TIM0_COMPB:
;	
;	;tgl_io_in_reg RGB_PORTx, rgb_tgl_reg;toggle the pins 
;	in  isr_toggle_reg, RGB_PORTx;
;	eor isr_toggle_reg, rgb_tgl_reg;
;	out RGB_PORTx, isr_toggle_reg;
;	;out RGB_PINx, rgb_tgl_reg;
;	reti;return from interrupt
