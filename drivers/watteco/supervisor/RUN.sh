
where=`uname -n`
killall awspv.x

sleep 3

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
	option="--dev"
else
	option=""
fi
echo $option


mv stdouterr.log stdouterr.old.log > /dev/null 2>&1
rm TRACE*.log core core.* > /dev/null 2>&1

valgrindoptions="--leak-check=full --show-reachable=yes"

#valgrind $valgrindoptions ./awspv.x --dev $option $* > stdouterr.log 2>&1 &

./awspv.x $option $* > stdouterr.log 2>&1 &




sleep 2
tail -f ./TRACE.log
