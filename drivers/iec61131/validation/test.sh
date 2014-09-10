
SUBSYSTEM=validation

./validation.x plc.xml 2>/dev/null
[ $? == 1 ] && echo "[31;7m[$SUBSYSTEM] Error Case 1 ![0m",exit 1 || true;

./validation.x plcerr.xml 2>/dev/null
[ $? == 0 ] && echo "[31;7m[$SUBSYSTEM] Error Case 2 ![0m",exit 1 || true;

exit 0
