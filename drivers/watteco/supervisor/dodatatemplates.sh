AddLine()
{
	while read line
	do
		echo "${line}\\"
	done
}

#nbvar="0"
#for f in ../diatemplates/*.xml
#do
#	varf=`basename $f`
#	varf=`expr "$varf" : '\(.*\).xml'`
#	echo	"static const char *${varf}=\"\\"
#	sed "s/\"/\'/g" $f | AddLine
#	echo	"\";"
#	tabvar[${nbvar}]=$varf
#	nbvar=$(($nbvar+1))
#done

#echo	"/* $nbvar templates */"
#index="0"
#while [ $index -lt $nbvar ]
#do
#	echo	"${tabvar[${index}]},"
#	index=$(($index+1))
#done
#echo	"};"

echo	"const char *TbTemplates[][2] = {"
nbvar="0"
for f in ../diatemplates/*.xml
do
	varf=`basename $f`
	varf=`expr "$varf" : '\(.*\).xml'`
	echo	"{ \"${varf}\" , \"\\"
	sed "s/\"/\'/g" $f | AddLine
	echo	"\"},"
	nbvar=$(($nbvar+1))
done
echo	"};"
