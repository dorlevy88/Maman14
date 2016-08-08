
; 1. this line is ok (label X ignored, warning should be issued in first pass)
X:     .extern DATA1

; 2. this line is ok (label Y ignored, warning should be issued in first pass)
Y:     .entry START

; 3. this line is ok (DATA1 is an external label)
START:  add  DATA1,r1

; 4. undefined label as first operand
        add  Q,r2

; 5. undefined label as second operand
        add  r3,Q

; 6. undefined label used in dynamic immediate operand
        add  Q[1-2],r1

; 7. undefined label as first operand (label X is ignored)
        add  X,r4

; 8. undefined label as second operand (label Y is ignored)
        add  r5,Y

; 9. undefined label
       .entry Q

; 10. undefined label (label X is ignored)
       .entry X

; 11. undefined label (label Y is ignored)
       .entry Y

; 12. this line is ok
        stop