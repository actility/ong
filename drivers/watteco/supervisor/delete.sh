USER="http%3a//nsc1.actility.com%3a8088/m2m/applications/SYSTEM"
TYPE="Content-Type: application/xml"
for dev in 803b 2b0a 2d08 1605
do
RES="http://192.168.1.98:8080/m2m/applications/DEV_020000ffff00${dev}/retargeting1/0xFFFF.0x00.ope"
#curl -v -X GET -u "$USER" $RES
curl -v -H "$TYPE" -X POST --data "@./params.xml" -u "$USER" $RES
done

