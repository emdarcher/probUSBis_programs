

.include "tn85def.inc";

;.def and .equ things

.def regA = r16;
.def regB = r17;
.def temp_pwm_reg = r15;

.def pwm_fade_OCR0B_reg = r19;
.def my_flags_reg = r20;
.equ PWM_OCR0B_DIR_BIT = 0;

;.db things

.org 0000
	rjmp RESET;
.org OVF0addr
	rjmp TIM_OVF0;

RESET:
	;stuff do do at reset
	
	;set clock prescaler to div 16
	ldi regA, (1<<CLKPCE);
	out CLKPR, regA;
	ldi regA, (1<<CLKPS2);
	out CLKPR, regA;
	sbi DDRB, PB1;set PB1 to output

	;setup timer0 stuff
	ldi regA, (1<<WGM01)|(1<<WGM00)|(1<<COM0B1);set to fast PWM mode
	out TCCR0A, regA;
	ldi regA, (1<<CS01)|(1<<CS00); set to div/64
	out TCCR0B, regA;
	ldi regA, (1<<TOIE0);
	out TIMSK, regA;
	
	sei; enable interrupts
	rjmp MAIN; ;jump to main

MAIN:
	;do stuff	
	nop;
	rjmp MAIN; loop back again.
;things

TIM_OVF0:
	dec pwm_fade_OCR0B_reg; dec the val
	mov temp_pwm_reg, pwm_fade_OCR0B_reg;copy to temp_pwm_reg
	sbrc my_flags_reg, PWM_OCR0B_DIR_BIT;skip if pwm dir flag is set
		com temp_pwm_reg;do 1's complement, makes it 255-value.
	out OCR0B, temp_pwm_reg; put the value into OCR0B

	tst pwm_fade_OCR0B_reg,pwm_fade_OCR0B_reg; test if zero or minus
	brne PWM_NOT_0;branch if not equal to zero, skip over things
	
	;these are executed if no branch occurs,
	;so if pwm_fade_OCR0B_reg = 0, do these things.
	ldi regA, (1<<PWM_OCR0B_DIR_BIT);put the dir flag bit in regA
	eor my_flags_reg, regA; toggle dir flag bit in my_flags_reg

PWM_NOT_0: ; the branch skips to here 
	nop; a nop to do something before returning
	reti;return from interrupt

