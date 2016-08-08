; 1. this line is ok (DATA4 is declared later)
Start:  dec DATA4

; 2. missing operand
        sub #5

; 3. missing operand
        red 

; 4. this line is ok
       .extern DATA4

; 5. invalid target operand (immediate)
        add  #5,#6

; 6. this line is ok (immediate target operand allowed for cmp)
Next:   cmp  #5,#6

; 7. invalid target operand (dynamic immediate)
        add  r1,Start[3-5]

; 8. invalid operand (dynamic immediate)
        jmp  Next[4-9]

; 9. invalid operand (immediate)
        inc  #50

; 10. undefined instruction
        and  r1,r2

; 11. undefined instruction (case sensitivity)
        jSr  Start

; 12. this line is ok (r9 is a label declared later)
        add  r1,r9

; 13. invalid characters (,r2)
        cmp  r1,,r2

; 14. invalid characters (,r3)
        add  #5,r1,r3

; 15. invalid characters (blabla)
        prn r1 blabla

; 16. invalid characters (],r1)
        cmp Start[4],r1

; 17. invalid characters (+7])
        prn Next[4+7]

; 18. invalid characters (xyz])
        prn Start[4-xyz]

; 19. invalid characters (123)
        inc  123

; 20. invalid characters (xyz)
        cmp  #xyz,r1

; 21. label previously declared
Next:   clr  r2

; 22. label does not start in first column (the label declaration is ignored)
     label1:   sub r1,r7

; 23. invalid source operand (register)   
        lea  r3,r1

; 24. invalid characters (Start)
        stop Start

; 25. invalid characters (4000) 
        .data   200 4000

; 26. this line is ok (arbitrary spaces)
DATA1:  .data   3000,   4000 ,  5000, 6000

; 27. invalid characters (,3)
        .data   1, ,3

; 28. invalid character (,)
        .data   4,

; 29. invalid characters (#123)
        .data   #123

; 30. invalid characters (.4)
        .data   12.4

; 31. invalid characters (-5)
        .data   --5

; 32. this line is ok (case sensitive labels)
Data1:  .data   100, +200, -300

; 33. invalid label (cannot be an instruction)
mov:    .data   5

; 34. invalid label (cannot be a register)
r1:     .data   200,300

; 35. label previously declared
DATA1:  .data   300

; 36. invalid label(non-alphabetic first character)
1DATA:  .data   300

; 37. this line is ok (r9 is not the name of a register)
r9:     .data   200

; 38. this line is ok (label declaration X is ignored - warning to be issued)
X:      .entry  DATA1

; 39. this line is ok (it is ok to declare the same external more than once)
        .extern DATA4

; 40. local label cannot be declared as external
        .extern Start    
		
; 41. this line is ok (label declaration Y is ignored - warning to be issued)
Y:      .extern DATA8

; 42. this line is ok (STR1 is declared later)
        .entry STR1

; 43. label is already designated as external
        .entry  DATA4

; 44. undefined instruction (note: DATA2 is not a label declaration)
DATA2   .data   4

; 45. undefined directive (case sensitivity)
        .DATA   5

; 46. This line is ok (it is ok to designate the same entry more than once)
        .entry  STR1

; 47. invalid characters (blabla is not a string)
        .string blabla

; 48. invalid characters (blabla)
        .string "abcdefg" blabla

; 49. this line is ok (comma within string is not a separator)
STR1:   .string "abc, ,defg"

; 50. invalid label (too long)
SuperCalifragilisticExpiAlidocious: .data	4	
          
; 51. missing operands in directive
        .data

; 52. missing operand in directive
        .entry

; 53. undefined directive
        .invalid 85,90

; 54. this line is ok
        stop