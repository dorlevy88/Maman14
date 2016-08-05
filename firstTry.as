ABD: mov #22 , r3
lab: not r4
.data 5,5,6,4,7,8
mov r2,r3
.extern AR 
datlabe: .data 6,+7,8,9,-12, +45, -452
ASD: .string "aaaAA"
datlab: mov r1,datlabe
.entry ABD
.entry datlab
prn datlabe[5-12]
mov AR,r2
