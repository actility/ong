
where=`uname -n`
killall wmbus.x

if [ -f main.c ]
then
	./MAKE
fi

#
#	now read from spvconfig.xml
#	but can be overloaded here
#
if [ "$where" = "pca-ESPRIMO-P5730" ]
then
	option="-X -S -O -t3 -d1 -p0"
else
	option="-X -S -t3 -d1 -p0"
fi
option=""
echo $option


mv stdouterr.log stdouterr.old > /dev/null 2>&1
rm TRACE*.log core core.* > /dev/null 2>&1
./wmbus.x $option $* > stdouterr.log 2>&1 &
sleep 2
tail -n 50 -f ./TRACE.log
