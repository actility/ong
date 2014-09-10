
(
	sleep 0.3
	echo "123"
	sleep 0.3
	echo "sendsensor aaaa::ff:ff00:240f 1150000602"
	echo "quit"
) | telnet 0 2000 

