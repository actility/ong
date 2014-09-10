#!/bin/bash

GW=""

usage ()
{
    echo "Usage : $0 [options] <ets4-project.knxproj>"
    echo "Where supported options are:"
    echo " -g <ip-addr>, --gateway-ip <ip-addr>: Provides the IP address of the gateway."
    echo " -h,           --help                : Print this help message and quit."
    echo
}

GETOPTTEMP=`/usr/bin/getopt -o hg: --long help,gateway-ip: -- "$@"`
if [ $? != 0 ] ; then usage>&2 ; exit 1 ; fi
eval set -- "$GETOPTTEMP"

while true ; do
  case "$1" in
    -h|--help) usage; exit;;
    -g|--gateway-ip) GW=$2 ; shift 2 ;;
    --) shift ; break ;;
    *) echo "Internal error!" ; exit 1 ;;
  esac
done

if [ $# -eq 1 ]; then
    PRJ_PATH=$1
else
    usage >&2
    exit 1
fi

TMP_FOLDER=`mktemp -d`
SCRIPT_FOLDER=`pwd`

cp $PRJ_PATH $TMP_FOLDER/prj.zip
cp ets4.xsl $TMP_FOLDER/ets4.xsl
cd $TMP_FOLDER

# Extract: knxproj == zip
unzip prj.zip

# Apply XSL
java -jar $SCRIPT_FOLDER/SaxonEE9-5-0-1J/saxon9ee.jar -xsl:ets4.xsl -s:$(ls -1 P-*/0.xml) -o:output.xml

if [ -z $GW ]; then
    grep 'gateway="0.0.0.0"' $TMP_FOLDER/output.xml
    if [ $? -eq 0 ]; then
        echo "*************** ERROR : NO GATEWAY **********************"
        rm -Rf $TMP_FOLDER
        exit    
    fi
else
   sed -i "s/gateway=\".*\"/gateway=\"$GW\"/" $TMP_FOLDER/output.xml
fi

echo "*************** SUCCESS *********************************"
echo "Converted project at : $TMP_FOLDER/output.xml"
echo "*********************************************************"

# cat $TMP_FOLDER/output.xml

