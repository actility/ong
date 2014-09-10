
filterwatt()
{
	while read line
	do
		echo $line
	done
}

#tcpdump -l -itun0 -s1024 -x port 47002 or port 47004 | grep -v '0x0000:'
tcpdump -lnOpq -ieth0 -s0 -X port 5683 or port 5684 | filterwatt
