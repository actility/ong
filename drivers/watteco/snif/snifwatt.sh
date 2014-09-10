
MYUID=`id -u`
if [ $MYUID != "0" ]
then
	echo "you must be root to execute snifwatt.sh"
	sudo -E $ROOTACT/watteco/snif/snifwatt.sh $*
	exit $?
fi
$ROOTACT/watteco/snif/snifwatt.x $*
