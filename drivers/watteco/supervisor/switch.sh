
(
	sleep 0.3
	echo 123
	sleep 0.3
#	echo method /6lowpan/APP_020000ffff002b0a.1/retargeting/0x0006.0x02.ope
	echo method /6lowpan/APP_020000ffff002d08.1/retargeting/0x0006.0x02.ope
	echo quit
) | telnet 0 2000 

