
T1 := LocalDT();
x := sec(T1);
x := min(T1);
x := hour(T1);
x := mday(T1);
x := mon(T1);
x := year(T1);
x := wday(T1);
x := yday(T1);
x := isdst(T1);
x := gmtoff(T1);

y := tmzone(T1);
d := isDay(T1);
HC := sunsetTime();
HL := sunriseTime();

T2 := DT_TO_TOD(T1);
T3 := T1 + T#1s;
T4 := T1 + T#-1s;
T5 := T1 + T#1ms;
T6 := T1 + T#-1ms;

S := DT_TO_STRING(T1);
S := TOD_TO_STRING(T2);
S := DT_TO_STRING(T3);
S := DT_TO_STRING(T4);
S := DT_TO_STRING(T5);
S := DT_TO_STRING(T6);
