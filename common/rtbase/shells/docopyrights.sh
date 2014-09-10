
ADDFILE=$ROOTACT/rtbase/copyrights.txt

DoJob()
{
	while read file
	do
		mdt=`stat -c %y $file`
		sed -i '1i\\n' $file
		sed -i "1r$ADDFILE" $file
		touch -m -d"$mdt" $file
		echo $file $mdt
	done
}



find . -type f -name '*.[ch]' -print | DoJob
