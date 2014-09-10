
Addr=192.168.1.11
Port=3398
ReqEntity="coap://plciecong.actility.com:5683/m2m/applications/SYSTEM"
#TargetID="coap://192.168.1.116:5683/m2m/applications/IEC_app1/containers/CONFIGURATION/contentInstances/latest/content"
TargetID="coap://192.168.1.116:5683/m2m/applications/IEC_app1/containers/CONFIGURATION/contentInstances"

$ROOTACT/libdIa/client/client.x -m retrieve -A $Addr -p $Port -v9 -R $ReqEntity $TargetID
