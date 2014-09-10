


case "$1" in
stop) 
	../init.d/awspv $*
	exit 0 
;;
start|restart)
	../init.d/awspv $*
	sleep 2
	tail -f ./TRACE.log
	exit 0
;;
status)
	../init.d/awspv $*
	exit 0
;;
statusext)
	../init.d/awspv $*
	exit 0
;;
*) 
	exit 1
	;;
esac

exit 1
