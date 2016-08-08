
; 1. this line is ok
X:   cmp r1,r2

; 2. this line is ok (highest positive value that fits in 15 bits)
    .data  16383

; 3. data overflow (positive value too large to fit in 15 bits)
    .data  16384

; 4. this line is ok (lowest negative value that fits in 15 bits)
    .data  -16384

; 5. data overflow (negative value too large to fit in 15 bits)
    .data  -16385

; 6. this line is ok
     stop