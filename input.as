LENGTH: .data 6, -7
.extern W
MAIN: mov K[2-4], W
	 add r2,MIAO
LOOP: jmp W
	prn #-7
	sub r1,r5
	inc K
	mov LOOP[1-13],r3
	bne L3
.entry LOOP
END: stop
MIAO: .string "nevergonnagiveyouup"
.entry LENGTH
.extern L3
K: .data 2

