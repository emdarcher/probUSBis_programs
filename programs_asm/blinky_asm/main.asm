

.include "tn85def.inc";

;.def and .equ things

.def regA = r16;
.def regB = r17;
.def regC = r18;

;.db things

.org 0000
	rjmp RESET;

.org OVF0addr
	rjmp TIM_OVF0;

RESET:
	;stuff do do at reset
	
	;ldi regA, (1<<PB1);
	sbi DDRB, PB1;
	ldi regA, (1<<CS02)|(1<<CS00);
	out TCCR0B, regA;
	ldi regA, (1<<TOIE0);
	out TIMSK, regA;
	
	;set clock prescaler to div 16
	;sbi CLKPR,CLKPCE;
	ldi regA, (1<<CLKPCE);
	out CLKPR, regA;
	ldi regA, (1<<CLKPS2);
	out CLKPR, regA;

	sei;
	rjmp MAIN; ;jump to main

MAIN:
	;do stuff
	
	nop;
	rjmp MAIN; loop back again.
;things

TIM_OVF0:
	sbi PINB, PB1; toggles PB1 on newer AVRs
	reti;return from interrupt
