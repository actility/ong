A := TRUE;
B := TRUE;
A := NOT A;
C := (A AND B) XOR (B OR A);


QX1 := TRUE;
QX2 := FALSE;
QX3 := TRUE;
RET := NOT ( QX1 OR QX2 OR QX3 )
