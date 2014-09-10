USER="http%3a//nsc1.actility.com%3a8088/m2m/applications/SYSTEM"
RES="http://192.168.1.98:8080/m2m/applications/APP_020000ffff002b0a.1/retargeting2/0x0006.0x02.ope"
TYPE="Content-Type: application/xml"
#curl -v -X GET -u "$USER" $RES
curl -v -H "$TYPE" -X POST --data "@./params.xml" -u "$USER" $RES

