
SUBSYSTEM=interv2
for i in tests/*; do echo $i >&2; ./parser.x -f $i -t9 ; done 2>ST.out
grep ERROR ST.out
[ $? == 0 ] && echo "[31;7m[$SUBSYSTEM] Error 1![0m",exit 1 || true;

./parser.x -f error.st -t9 2>/dev/null
[ $? == 0 ] && echo "[31;7m[$SUBSYSTEM] Error 2![0m",exit 1 || true;

./VG 2>&1 | grep "definitely lost: 0 bytes" >/dev/null
[ $? == 1 ] && echo "[31;7m[$SUBSYSTEM] Error 3![0m",exit 1 || true;

exit 0
