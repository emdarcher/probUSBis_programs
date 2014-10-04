

.inc "tn85def.inc";

;.def and .equ things

;.db things



.org 0000
	rjmp RESET;


RESET:
	;stuff do do at reset

	rjmp MAIN; ;jump to main

MAIN:
	;do stuff

;things
