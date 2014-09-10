
#
#	the USB router must be plugged before tunslip6 start
#

PRODUCT="FT232R USB UART"
TTY_PREFIX="ttyUSB"
TTY_NAME="/dev/ttyUSB0"

if [ -f /etc/profile ]
then
	. /etc/profile
fi

if [ -z "$ROOTACT" ]
then
	echo "ROOTACT not set"
	exit 1
fi

if [ -z "$AW_DATA_DIR" ]
then
	AW_DATA_DIR=$ROOTACT/usr/data/watteco
	export AW_DATA_DIR
fi

MYUID=`id -u`
if [ $MYUID != "0" ]
then
	echo "you must be root to execute tunslip6.sh"
	exit 1
#	sudo -E $ROOTACT/watteco/tunslip/tunslip6.sh $*
#	exit $?
fi

#if [ -f $AW_DATA_DIR/confusb.conf -a -x $ROOTACT/watteco/tunslip/multiusb.sh ]
#then
#	exit 1
#fi

TTY_MAPPER="$ROOTACT/bin/tty_mapper"
if [ -x "$TTY_MAPPER" ]
then
	tmp=`$TTY_MAPPER "$TTY_PREFIX" "$PRODUCT"`
	if [ $tmp != '<unknown>' ]
	then
		echo "discovered (and forced) tty for watteco tunneling : $tmp"
		TTY_NAME=$tmp
        else
                echo "no watteco dongle"
                sleep 15
                exit 1
	fi
fi

if [ ! -c "$TTY_NAME" ]
then
	echo "$TTY_NAME not found"
	exit 1
fi

if [ ! -d /dev/net ]
then
	mkdir /dev/net
fi

if [ ! -c /dev/net/tun ]
then
	mknod /dev/net/tun c 10 200
fi

if [ ! -c /dev/net/tun ]
then
	echo "/dev/net/tun not found"
	exit 1
fi

echo 1 > /proc/sys/net/ipv6/conf/all/forwarding

if [ -f /proc/cpu/alignment ]
then
	echo 2 > /proc/cpu/alignment
fi

trap '' 1 2 3

mkdir -p $AW_DATA_DIR > /dev/null 2>&1
chmod a+rwx $AW_DATA_DIR

PREFIPV6=aaaa
if [ -f $AW_DATA_DIR/prefipv6.txt ]
then
	PREFIPV6=`cat $AW_DATA_DIR/prefipv6.txt`
fi

PANID=""
if [ -f $AW_DATA_DIR/panid.txt ]
then
#	43981 == 0xabcd
	PANID=`cat $AW_DATA_DIR/panid.txt`
fi

PANADDR=$AW_DATA_DIR/routeraddr.txt
rm $PANADDR > /dev/null 2>&1
touch $PANADDR
chmod a+rw $PANADDR

PANIDOUT=$AW_DATA_DIR/panid_out.txt
rm $PANIDOUT > /dev/null 2>&1

TRACE=/dev/null
#TRACE=$AW_DATA_DIR/tunslip6.log
if [ "$TRACE" != "/dev/null" ]
then
	rm $TRACE > /dev/null 2>&1
	touch $TRACE
	chmod a+rw $TRACE
fi

#EXE="$ROOTACT/watteco/tunslip/tunslip6.x -v"
EXE="$ROOTACT/watteco/tunslip/tunslip6.x"

#echo "./tunslip6.x -s $TTY_NAME -@$PANADDR $PREFIPV6::1/64"
#exit 0

#exec $EXE -s "$TTY_NAME" -@$PANADDR "$PREFIPV6::1/64" >> $TRACE
if [ "$PANID" != "" ]
then
exec $EXE -s "$TTY_NAME" -P$PANID -R$PANIDOUT -@$PANADDR "$PREFIPV6::1/64" >> $TRACE 2>&1
else
exec $EXE -s "$TTY_NAME" -R$PANIDOUT -@$PANADDR "$PREFIPV6::1/64" >> $TRACE 2>&1
fi



