
; 1. this line is ok
X:   mov Y[0-12],r1

; 2. this line is ok
     cmp X[4-7],Y[2-14]

; 3. dynamic bit range invalid (range exceeds max value 13)
     mov Y[1-14],r1

; 4. dynamic bit range invalid (left boundary larger than right boundary)
     add X[8-4],Y

; 5. dynamic bit range invalid (right boundary exceeds max value 14)
     prn Y[11-15]

; 6. dynamic bit range invalid (left boundary exceeds max value 14)
     prn X[230-230]

; 7. this line is ok
Y:  .data 12345

; 8. this line is ok
     stop