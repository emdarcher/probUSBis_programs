

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
	sei;
	rjmp MAIN; ;jump to main

MAIN:
	;do stuff
	
	nop;
	rjmp MAIN;
;things

TIM_OVF0:
	sbi PINB, PB1;
	reti;
