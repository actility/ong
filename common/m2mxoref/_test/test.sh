#file=$1
#LOADXOR=$ROOTACT/m2mxoref/xoref
PARSER="$ROOTACT/xo/com/xoxml.x -r ../xoref/m2m.xor"

for file in *.xml
do
	echo $file
	xmllint --format $file -o $file
	$PARSER $file | xmllint --format - | diff $file - | grep -v __skip__
done

for file in *.pxml
do
	echo $file
	xmllint --format $file -o $file
	$PARSER -Tm2m:application $file | xmllint --format - | diff $file - | grep -v __skip__
done

#$PARSER -P'm2m:aPoCPaths.m2m:aPoCPath[1].m2m:path' app.xml

for file in *.obx
do
	echo $file
	xmllint --format $file -o $file
	$PARSER -B $file | xmllint --format - | diff $file - | grep -v __skip__
done
