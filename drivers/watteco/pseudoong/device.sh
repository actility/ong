TARGET=./m2m/applications/NW_*/containers/DESCRIPTOR/contentInstances/latest/create.xml

dev_filter()
{
	while read line
	do
		line=`expr "$line" : '.*/DEV_\([0-9a-zA-Z]*\).*'`
		echo $line
	done
}

./xoparse.sh -B $TARGET | grep "ref href" | grep DEV_ | dev_filter


