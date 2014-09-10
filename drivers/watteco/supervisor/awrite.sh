USER="http%3a//nsc1.actility.com%3a8088/m2m/applications/SYSTEM"
RES="http://192.168.1.98:8080/m2m/applications/APP_020000ffff002290.1/retargeting1/0x000f.0x0054.0.attr"
TYPE="Content-Type: application/xml"
curl -v -H "$TYPE" -X POST --data "@./params.xml" -u "$USER" $RES
#curl -v -X GET -u "$USER" $RES

