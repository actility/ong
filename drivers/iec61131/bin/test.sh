
./plc.x -f $ROOTACT/iec61131-reference-projects/Thermostat_Simple -t0 2>/dev/null &

sleep 1

python test.py
rc=$?

lpid=`pidof plc.x`
if [ "x$lpid" != "x" ]
then
	for pid in $lpid
	do
		kill -9 $pid
		#echo "Killing "$pid
	done
fi

exit $rc
