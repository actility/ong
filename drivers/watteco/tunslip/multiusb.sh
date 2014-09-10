#!/bin/bash

declare	-a tbusbid
declare	-a tbpanid
declare	-a tbflags
declare	-a tbprefix
declare	-a tbip6addr
declare	-a tbtunnel
declare	-a tbtty
declare	-a tbpid

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
	echo "you must be root to execute NUSB.sh"
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

PREFIXSERIAL="/dev/serial/by-id/usb-FTDI_FT232R_USB_UART"
SUFFIXSERIAL="if00-port0"

CONFUSB=$AW_DATA_DIR/confusb.conf
PANADDR=$AW_DATA_DIR/routeraddr.txt
DISCOVER=$AW_DATA_DIR/discoverpan
EXE="$ROOTACT/watteco/tunslip/tunslip6.x"

Flags()
{
	ret=$(expr match "$1" ".*[,]*\(${2}\)[,]*")
	echo $ret
}

Error()
{
	echo "### ERR:$1" 1>&2
}

Trace()
{
	echo "### DBG:$1" 1>&2
}

GetTTY()
{
	USBID=$1
	FILEUSBID="${PREFIXSERIAL}_${USBID}-${SUFFIXSERIAL}"

	if [ ! -L $FILEUSBID ]
	then
		Error "can not find link on $USBID in /dev/serial/by-id"
		echo "-"
		return	1
	fi
	TTY=`readlink $FILEUSBID`
	TTY=`basename $TTY`
	TTY="/dev/${TTY}"
	if [ ! -c $TTY ]
	then
		Error "can not find char device $TTY"
		echo "-"
		return	1
	fi

	Trace	"$TTY found for $USBID"
	echo	$TTY
	return	0
}

Finish()
{
for	i in ${!tbusbid[*]}
do
	pid=${tbpid[i]}
	if [ $pid != "0" ]
	then
Trace "stop dongle $i $pid"
		kill $pid
		tbpid[i]="0"
	fi
done
exit 0
}

Dump()
{
for	i in ${!tbusbid[*]}
do
echo	"usb=${tbusbid[i]} panid=${tbpanid[i]} mac=${tbflags[i]} pref=${tbprefix[i]} ip6=${tbip6addr[i]} tun=${tbtunnel[i]} tty=${tbtty[i]} pid=${tbpid[i]}"
done
}

StartMainDongle()
{
$EXE -s$1 -t$2 -P$3 -@$PANADDR "$4::1/64" > /dev/null 2>&1 &
#$EXE -s$1 -t$2 -P$3 -@$PANADDR "$4::1/64" &
}

StartOtherDongle()
{
$EXE -s$1 -t$2 -P$3 "$4::1/64" > /dev/null 2>&1 &
}

StartDongle()
{
	usbid=${tbusbid[$1]}
	flags=${tbflags[$1]}
	panid=${tbpanid[$1]}
	prefix=${tbprefix[$1]}
	tunnel=${tbtunnel[$1]}
	tty=${tbtty[$1]}

	flags=$(Flags $flags "m")

	if [ $flags = "m" ]
	then
Trace "StartMainDongle $tty $tunnel $panid $prefix"
		StartMainDongle $tty $tunnel $panid $prefix
		return 0
	else
Trace "StartOtherDongle $tty $tunnel $panid $prefix"
		StartOtherDongle $tty $tunnel $panid $prefix
		return 0
	fi

	return 0
}

ParseHTTP()
{
grep -v via | grep -v body | sed -e 's/<h2>Neighbors<\/h2>//' -e 's/<br>//' -e"s/fe80/${1}/"
}

OnExit()
{
echo stopping
pgrep -P$$
pkill -TERM -P$$
sleep 3
pkill -KILL -P$$;
echo stopped;
exit 0
}

#trap Finish EXIT INT TERM
#trap 'kill $(jobs -p);exit 0' EXIT INT TERM
#trap 'echo stopping; pgrep -P$$; pkill -TERM -P$$; sleep 3; pkill -9 -P$$; echo stopped; exit 0' EXIT INT TERM

trap OnExit EXIT INT TERM

Trace "starting monitoring $$"
sleep 2

while	read aLine
do
	set $aLine
	if [ $# != 6 ]
	then
		continue
	fi
	case	$1 in
		\#)
			continue
		;;
		*)
		;;
	esac
	tbusbid[${#tbusbid[*]}]=$1
	tbpanid[${#tbpanid[*]}]=$2
	tbflags[${#tbflags[*]}]=$3
	tbprefix[${#tbprefix[*]}]=$4
	tbip6addr[${#tbip6addr[*]}]=$5
	tbtunnel[${#tbtunnel[*]}]=$6
	tbtty[${#tbtty[*]}]="-"
	tbpid[${#tbpid[*]}]="0"
done < $CONFUSB

#index=4
#for	((i=0;i<$index;i++))
#do
#	echo $i
#done

Dump
while	true
do
for	i in ${!tbusbid[*]}
do
	usbid=${tbusbid[i]}
	prefix=${tbprefix[i]}
	panid=${tbpanid[i]}
	ip6=${tbip6addr[i]}
	tty=${tbtty[i]}
	pid=${tbpid[i]}

#echo "$i $usbid $prefix $panid $tty $pid"

	if [ $tty = "-" ]
	then
		if [ $pid != "0" ]
		then
			Trace "$i process $pid exists and no tty found => kill"
			kill $pid > /dev/null 2>&1
			tbpid[i]="0"
		fi
		tty=`GetTTY $usbid`
		tbtty[i]=$tty
	fi

	if [ $tty != "-" ]
	then
		if [ $pid = "0" ]
		then
			StartDongle $i
			tbpid[i]=$!
			Trace	"$i start process ${tbpid[i]}"
		fi
		if [ $pid != "0" ]
		then
			kill -0 $pid > /dev/null 2>&1
			if [ $? != "0" ]
			then
				Trace "$i process $pid no more running"
				tbpid[i]="0"
			else
				Trace "$i process $pid still running"
			fi
		fi
		tty=`GetTTY $usbid`
		tbtty[i]=$tty
	fi

	if [ $tty != "-" -a $pid != "0" -a $ip6 != "-" ]
	then
		nbep="0"
		file=/tmp/http_${usbid}
		rm -f ${file}.txt > /dev/null 2>&1
		rm -f ${file}.tmp > /dev/null 2>&1
		wget -O ${file}.tmp "http://[${ip6}]" >/dev/null 2>&1
		if [ $? = "0" ]
		then
			cat ${file}.tmp | ParseHTTP $prefix > ${file}.txt
			nbep=`cat ${file}.txt | wc -l`
		fi
		rm -f ${file}.tmp > /dev/null 2>&1
Trace "HTTP $ip6 => $nbep"
	fi

done

#	Dump
	sleep	15

	if [ ! -f $DISCOVER.txt ]
	then
		touch $DISCOVER.txt
	fi

	cat $DISCOVER.txt > $DISCOVER.tmp
	
	for f in /tmp/http_*.txt
	do
		if [ -f $f ]
		then
			cat $f >> $DISCOVER.tmp
			rm -f ${f} > /dev/null 2>&1
		fi
	done
	sort -u $DISCOVER.tmp > $DISCOVER.tmp2
	mv $DISCOVER.tmp2 $DISCOVER.tmp
	diff $DISCOVER.txt $DISCOVER.tmp > /dev/null 2>&1
	if [ $? != "0" ]
	then
		Trace "Update $DISCOVER.tmp"
		mv $DISCOVER.tmp $DISCOVER.txt
	fi
done


exit 0
