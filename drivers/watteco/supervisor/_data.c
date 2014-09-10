const char *TbTemplates[][2] __attribute__ ((section(".text#"))) = {
{ "acc_elem_AR2" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<m2m:accessRight m2m:id='APP_${d_ieee}.${a_num}_AR2'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:permissions>\
<m2m:permission m2m:id='crwd'>\
<m2m:permissionFlags>\
<m2m:flag>CREATE</m2m:flag><m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag><m2m:flag>DELETE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SYSTEM</m2m:holderRef>\
<m2m:holderRef>coap://${w_boxname}.${w_domainname}${s_rootapp}/${w_driverid}</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
<m2m:permission m2m:id='rw'>\
<m2m:permissionFlags>\
<m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SUPPORT</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
<m2m:permission m2m:id='r'>\
<m2m:permissionFlags>\
<m2m:flag>READ</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders/>\
</m2m:permission>\
</m2m:permissions>\
<m2m:selfPermissions>\
<m2m:permission m2m:id='crwd'>\
<m2m:permissionFlags>\
<m2m:flag>CREATE</m2m:flag><m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag><m2m:flag>DELETE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SYSTEM</m2m:holderRef>\
<m2m:holderRef>coap://${w_boxname}.${w_domainname}${s_rootapp}/${w_driverid}</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
<m2m:permission m2m:id='r'>\
<m2m:permissionFlags>\
<m2m:flag>READ</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SUPPORT</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
</m2m:selfPermissions>\
</m2m:accessRight>\
"},
{ "acc_elem_AR3" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<m2m:accessRight m2m:id='APP_${d_ieee}.${a_num}_AR3'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:permissions>\
<m2m:permission m2m:id='crwd'>\
<m2m:permissionFlags>\
<m2m:flag>CREATE</m2m:flag><m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag><m2m:flag>DELETE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SYSTEM</m2m:holderRef>\
<m2m:holderRef>coap://${w_boxname}.${w_domainname}${s_rootapp}/${w_driverid}</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
<m2m:permission m2m:id='rw'>\
<m2m:permissionFlags>\
<m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SUPPORT</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
</m2m:permissions>\
<m2m:selfPermissions>\
<m2m:permission m2m:id='crwd'>\
<m2m:permissionFlags>\
<m2m:flag>CREATE</m2m:flag><m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag><m2m:flag>DELETE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SYSTEM</m2m:holderRef>\
<m2m:holderRef>coap://${w_boxname}.${w_domainname}${s_rootapp}/${w_driverid}</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
<m2m:permission m2m:id='r'>\
<m2m:permissionFlags>\
<m2m:flag>READ</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SUPPORT</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
</m2m:selfPermissions>\
</m2m:accessRight>\
"},
{ "acc_elem_AR" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<m2m:accessRight m2m:id='APP_${d_ieee}.${a_num}_AR'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:permissions>\
<m2m:permission m2m:id='crwd'>\
<m2m:permissionFlags>\
<m2m:flag>CREATE</m2m:flag><m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag><m2m:flag>DELETE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SYSTEM</m2m:holderRef>\
<m2m:holderRef>coap://${w_boxname}.${w_domainname}${s_rootapp}/${w_driverid}</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
<m2m:permission m2m:id='r'>\
<m2m:permissionFlags>\
<m2m:flag>READ</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SUPPORT</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
</m2m:permissions>\
<m2m:selfPermissions>\
<m2m:permission m2m:id='crwd'>\
<m2m:permissionFlags>\
<m2m:flag>CREATE</m2m:flag><m2m:flag>READ</m2m:flag><m2m:flag>WRITE</m2m:flag><m2m:flag>DELETE</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SYSTEM</m2m:holderRef>\
<m2m:holderRef>coap://${w_boxname}.${w_domainname}${s_rootapp}/${w_driverid}</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
<m2m:permission m2m:id='r'>\
<m2m:permissionFlags>\
<m2m:flag>READ</m2m:flag>\
</m2m:permissionFlags>\
<m2m:permissionHolders>\
<m2m:holderRefs>\
<m2m:holderRef>http://thingpark.${w_domainname}${s_rootapp}/SUPPORT</m2m:holderRef>\
</m2m:holderRefs>\
</m2m:permissionHolders>\
</m2m:permission>\
</m2m:selfPermissions>\
</m2m:accessRight>\
"},
{ "app_cont" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
-->\
<m2m:container m2m:id='DESCRIPTOR'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectSemantic/OASIS.OBIX_1_1</m2m:searchString>\
</m2m:searchStrings>\
<m2m:maxNrOfInstances>1</m2m:maxNrOfInstances>\
</m2m:container>\
"},
{ "app_elem" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
## w_netid\
## w_ipuid\
## d_ieee\
## a_num\
## w_drvcontact\
## w_driverpath\
-->\
<m2m:application appId='APP_${d_ieee}.${a_num}'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectType/ETSI.AN_APP</m2m:searchString>\
<m2m:searchString>ETSI.ObjectTechnology/ZigBee.ZigBee_1_0</m2m:searchString>\
<m2m:searchString>ETSI.InterworkingProxyID/${w_ipuid}</m2m:searchString>\
<m2m:searchString>ETSI.NetworkID/NW_${w_netid}</m2m:searchString>\
<m2m:searchString>ETSI.NodeID/DEV_${d_ieee}</m2m:searchString>\
<!--\
Not defined for watteco\
\
<m2m:searchString>ZigBee.ApplicationID/0x0104</m2m:searchString>\
<m2m:searchString>ZigBee.DeviceID/0x0302</m2m:searchString>\
-->\
<!--\
See sensor.c:iSensorSetSearchStringInObj()\
<m2m:searchString>ETSI.InterfaceID/TemperatureMeasurement.Srv</m2m:searchString>\
<m2m:searchString>ETSI.InterfaceID/RelativeHumidityMeasurement.Srv</m2m:searchString>\
...\
\
<m2m:searchString>ZigBee.ClusterID/server/0x0402</m2m:searchString>\
<m2m:searchString>ZigBee.ClusterID/server/0x0405</m2m:searchString>\
...\
-->\
</m2m:searchStrings>\
<m2m:aPoC>coap://${w_drvcontact}/${w_driverpath}/APP_${d_ieee}.${a_num}</m2m:aPoC>\
<m2m:aPoCPaths>\
<m2m:aPoCPath>\
<m2m:path>retargeting1</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
<m2m:aPoCPath>\
<m2m:path>retargeting2</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
<m2m:aPoCPath>\
<m2m:path>retargeting3</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
</m2m:aPoCPaths>\
</m2m:application>\
"},
{ "app_inst" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='applicationID' val='APP_${d_ieee}.${a_num}'/>\
<!--\
<str name='applicationProfileID' val='0x104'/>\
<str name='applicationDeviceID' val='0x0103'/>\
-->\
<int name='applicationDeviceVersion' val='1'/>\
<int name='endpoint' val='1'/>\
<list name='interfaces'>\
<!--\
\
See sensor.c:iSensorSetInterfaceListInObj() and itf_inst_%04x_server.xml\
\
<obj>\
<str name='interfaceID' val='TemperatureMeasurement.Srv'/>\
<str name='clusterID' val='0x0402'/>\
<enum name='clusterType' val='server'/>\
\
<int name='zbMeasuredValue' href='${s_rootapp}/APP_0b435eff56a3bbbb.1/retargeting2/0x0402/0x0000'/>\
<int name='zbMinMeasuredValue' val='-4000'/>\
<int name='zbMaxMeasuredValue' val='12000'/>\
\
\
<real name='m2mMeasuredValue' unit='obix:units/celsius' href='${s_rootapp}/APP_0b435eff56a3bbbb.1/containers/0x0402/contentInstances/latest/content'/>\
</obj>\
<obj>\
<str name='interfaceID' val='RelativeHumidityMeasurement.Srv'/>\
<str name='clusterID' val='0x0405'/>\
<enum name='clusterType' val='server'/>\
\
<int name='zbMeasuredValue' href='${s_rootapp}/APP_0b435eff56a3bbbb.1/retargeting2/0x0405/0x0000'/>\
<int name='zbMinMeasuredValue' val='500'/>\
<int name='zbMaxMeasuredValue' val='9500'/>\
\
\
<real name='m2mMeasuredValue' unit='obix:units/percent_relative_humidity' href='${s_rootapp}/APP_0b435eff56a3bbbb.1/containers/0x0405/contentInstances/latest/content'/>\
</obj>\
-->\
</list>\
</obj>\
"},
{ "dat_cont" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<m2m:container m2m:id='0x${c_num}'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
<!--\
<m2m:accessRightID>${s_rootacc}/APP_${d_ieee}.${a_num}_AR</m2m:accessRightID>\
-->\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectSemantic/OASIS.OBIX_1_1</m2m:searchString>\
</m2m:searchStrings>\
\
<!--\
<m2m:maxNrOfInstances>10</m2m:maxNrOfInstances>\
<m2m:maxInstanceAge>11</m2m:maxInstanceAge>\
-->\
\
<!-- added if necessary\
xmlns:acy=http://uri.actility.com/m2m\
-->\
<!--\
<acy:storageConfiguration>\
<acy:param name='store:driver' val='ram'/>\
</acy:storageConfiguration>\
-->\
\
\
</m2m:container>\
"},
{ "dat_inst_0006_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_000c_0055_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_000f_0055_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0052_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0052_0000_1" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0052_0000_3" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0052_0000_4" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0053_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
this diatemplates is quiet special because r_value can be 400 bytes long\
because of ascii hexa tic data\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
-->\
<str val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}' x-unit='ERDF-NOI-CPT_02E'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0053_0001_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
this diatemplates is quiet special because r_value can be 400 bytes long\
because of ascii hexa tic data\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
-->\
<str val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}' x-unit='ERDF-NOI-CPT_02E'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0053_0002_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
this diatemplates is quiet special because r_value can be 400 bytes long\
because of ascii hexa tic data\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
-->\
<str val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}' x-unit='ERDF-NOI-CPT_02E'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0053_0010_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<int val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0054_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
this diatemplates is quiet special because r_value can be 400 bytes long\
because of ascii hexa tic data\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
-->\
<str val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}' x-unit='ERDF-NOI-CPT_02E'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0054_0010_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<int val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0055_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
this diatemplates is quiet special because r_value can be 400 bytes long\
because of ascii hexa tic data\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
do not suppress this comment which is here to increase the file size\
-->\
<str val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}' x-unit='ERDF-NOI-CPT_02E'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0055_0010_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<int val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0400_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0402_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0405_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dat_inst_0406_0000_0" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<real val='${r_value}'\
x-template='${w_template}'\
x-date='${w_date} ${w_time}'\
xmlns='http://obix.org/ns/schema/1.1'/>\
"},
{ "dev_cont" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
#\
#\
#\
-->\
<m2m:container m2m:id='DESCRIPTOR'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectSemantic/OASIS.OBIX_1_1</m2m:searchString>\
</m2m:searchStrings>\
<m2m:maxNrOfInstances>1</m2m:maxNrOfInstances>\
</m2m:container>\
"},
{ "dev_elem" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
## w_netid\
## w_ipuid\
## d_ieee\
## w_drvcontact\
## w_driverpath\
<m2m:searchString>ETSI.ObjectType/ETSI.AN_NODE</m2m:searchString>\
<m2m:searchString>ETSI.ObjectType/ETSI.AN_DEV</m2m:searchString>\
-->\
<m2m:application appId='DEV_${d_ieee}'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectType/ETSI.AN_NODE</m2m:searchString>\
<m2m:searchString>ETSI.ObjectTechnology/ZigBee.ZigBee_1_0</m2m:searchString>\
<m2m:searchString>ETSI.InterworkingProxyID/${w_ipuid}</m2m:searchString>\
<m2m:searchString>ETSI.NetworkID/NW_${w_netid}</m2m:searchString>\
<m2m:searchString>ETSI.NodeID/DEV_${d_ieee}</m2m:searchString>\
</m2m:searchStrings>\
<m2m:aPoC>coap://${w_drvcontact}/${w_driverpath}/DEV_${d_ieee}</m2m:aPoC>\
<m2m:aPoCPaths>\
<m2m:aPoCPath>\
<m2m:path>retargeting1</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
<m2m:aPoCPath>\
<m2m:path>retargeting2</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
</m2m:aPoCPaths>\
</m2m:application>\
"},
{ "dev_inst" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='nodeID' val='DEV_${d_ieee}'/>\
<str name='ieeeAddress' val='0x${d_ieee}'/>\
<!--\
<str name='networkAddress' val='0x1234'/>\
-->\
<enum name='nodeType' val='endDevice'/>\
<str name='manufacturerCode' val='0x${w_manufacturer}'/>\
<!--\
<enum name='powerMode' val='rxidle'/>\
<enum name='powerSource' val='rechar'/>\
<enum name='powerLevel' val='l66'/>\
-->\
\
<list name='applications'>\
<!--\
See sensor.c:iSensorSetApplicationListInObj()\
\
<ref href='${s_rootapp}/APP_0b435eff56a3bbbb.1/containers/DESCRIPTOR/contentInstances/latest/content/'/>\
-->\
</list>\
<!--\
<op name='zdoMgmtBind' href='${s_rootapp}/DEV_0b435eff56a3bbbb/retargeting2/zmgmtbind'/>\
<op name='zdoMgmtLeave' href='${s_rootapp}/DEV_0b435eff56a3bbbb/retargeting2/zmgmtleave'/>\
-->\
</obj>\
"},
{ "ipu_cont" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
#\
#\
#\
-->\
<m2m:container m2m:id='DESCRIPTOR'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectSemantic/OASIS.OBIX_1_1</m2m:searchString>\
</m2m:searchStrings>\
<m2m:maxNrOfInstances>1</m2m:maxNrOfInstances>\
</m2m:container>\
"},
{ "ipu_elem" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
#\
#	w_ipuid\
#	w_drvcontact\
#	w_driverpath\
#\
-->\
<m2m:application appId='${w_ipuid}'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectType/ETSI.IP</m2m:searchString>\
<m2m:searchString>ETSI.ObjectTechnology/ZigBee.ZigBee_1_0</m2m:searchString>\
<m2m:searchString>ETSI.InterworkingProxyID/${w_ipuid}</m2m:searchString>\
</m2m:searchStrings>\
<m2m:aPoC>coap://${w_drvcontact}/${w_driverpath}/IPU_${w_ipuid}</m2m:aPoC>\
<m2m:aPoCPaths>\
<m2m:aPoCPath>\
<m2m:path>retargeting1</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
<m2m:aPoCPath>\
<m2m:path>retargeting2</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
</m2m:aPoCPaths>\
</m2m:application>\
\
"},
{ "ipu_inst" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
#\
#	w_ipuid\
#	w_netid\
#\
-->\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interworkingProxyID' val='${w_ipuid}'/>\
<list name='supportedTechnologies'>\
<obj>\
<str name='anPhysical' val='IEEE_802_15_4_2003_2_4GHz'/>\
<str name='anStandard' val='ZigBee_1_0'/>\
<str name='anProfile' val='ZigBee_HA'/>\
</obj>\
</list>\
<list name='networks'>\
<ref href='${s_rootapp}/NW_${w_netid}/containers/DESCRIPTOR/contentInstances/latest/content/'/>\
</list>\
</obj>\
"},
{ "itf_inst_0006_client" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='OnOff.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0006' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<bool name='zbOnOff' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x0000.0.attr'/>\
\
<!-- ZigBee commands -->\
\
<op name='zbOff'\
href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x00.ope'/>\
\
<op name='zbOn'\
href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x01.ope'/>\
\
<op name='zbToggle'\
href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x02.ope'/>\
\
<!-- M2M POINTS-->\
<bool name='m2mOnOff' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0006.0x0000.0.m2m/contentInstances/latest/content'/>\
\
</obj>\
"},
{ "itf_inst_0006_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='OnOff.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0006' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<bool name='zbOnOff' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x0000.0.attr'/>\
\
<!-- ZigBee commands -->\
\
<op name='zbOff'\
href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x00.ope'/>\
\
<op name='zbOn'\
href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x01.ope'/>\
\
<op name='zbToggle'\
href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0006.0x02.ope'/>\
\
<!-- M2M POINTS-->\
<bool name='m2mOnOff' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0006.0x0000.0.m2m/contentInstances/latest/content'/>\
\
</obj>\
"},
{ "itf_inst_000c_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='AnalogicInputServerCluster.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x000c' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- M2M POINTS-->\
<real name='m2mMeasuredValue' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x000c.0x0055.0.m2m/contentInstances/latest/content'/>\
\
</obj>\
"},
{ "itf_inst_000f_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='BinaryInputServerCluster.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x000f' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- M2M POINTS-->\
<real name='m2mMeasuredValue' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x000f.0x0055.0.m2m/contentInstances/latest/content'/>\
\
</obj>\
"},
{ "itf_inst_0052_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='SimpleMetering.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0052' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<!--\
<int name='zbCurrentSummationDelivered' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0052.0x0000.0.attr'/>\
<int name='zbInstantaneousDemand' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0052.0x0000.3.attr'/>\
-->\
\
<!-- ZigBee commands -->\
<!-- Watteco commands -->\
<op name='extReset'\
href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0052.0x00.ope'/>\
\
<!-- M2M POINTS-->\
<real name='m2mCurrentSummationDelivered' unit='obix:units/watt_hour' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0052.0x0000.0.m2m/contentInstances/latest/content'/>\
<real name='m2mInstantaneousDemand' unit='obix:units/watt' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0052.0x0000.3.m2m/contentInstances/latest/content'/>\
\
<!-- M2M POINTS watteco reactive power-->\
<real name='extCurrentSummationDeliveredR' unit='obix:units/volt_ampere_reactive_hour' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0052.0x0000.1.m2m/contentInstances/latest/content'/>\
<real name='extInstantaneousDemandR' unit='obix:units/volt_ampere_reactive' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0052.0x0000.4.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "itf_inst_0053_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='TicMetering.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0053' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<!-- ZigBee commands -->\
<!-- Watteco commands -->\
<!-- M2M POINTS-->\
<!-- M2M POINTS watteco -->\
<str name='extTicIceData' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0053.0x0000.0.m2m/contentInstances/latest/content'/>\
<str name='extTicIcePeriode0' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0053.0x0001.0.m2m/contentInstances/latest/content'/>\
<str name='extTicIcePeriode1' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0053.0x0002.0.m2m/contentInstances/latest/content'/>\
\
<int name='extTicIceType' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0053.0x0010.0.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "itf_inst_0054_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='TicMetering.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0054' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<!-- ZigBee commands -->\
<!-- Watteco commands -->\
<!-- M2M POINTS-->\
<!-- M2M POINTS watteco -->\
<str name='extTicCbeData' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0054.0x0000.0.m2m/contentInstances/latest/content'/>\
\
<int name='extTicCbeType' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0054.0x0010.0.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "itf_inst_0055_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='TicMetering.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0055' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<!-- ZigBee commands -->\
<!-- Watteco commands -->\
<!-- M2M POINTS-->\
<!-- M2M POINTS watteco -->\
<str name='extTicCjeData' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0055.0x0000.0.m2m/contentInstances/latest/content'/>\
\
<int name='extTicCjeType' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0055.0x0010.0.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "itf_inst_0400_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='IlluminanceMeasurement.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0400' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes\
<int name='zbMeasuredValue' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0400.0x0000.0.attr'/>\
<int name='zbMinMeasuredValue' val='0'/>\
<int name='zbMaxMeasuredValue' val='120000'/>\
-->\
\
<!-- ZigBee commands -->\
\
<!-- M2M POINTS-->\
<real name='m2mMeasuredValue' unit='obix:units/lux' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0400.0x0000.0.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "itf_inst_0402_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='TemperatureMeasurement.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0402' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<!--\
<int name='zbMeasuredValue' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0402.0x0000.0.attr'/>\
<int name='zbMinMeasuredValue' val='-4000'/>\
<int name='zbMaxMeasuredValue' val='12000'/>\
-->\
\
<!-- ZigBee commands -->\
\
<!-- M2M POINTS-->\
<real name='m2mMeasuredValue' unit='obix:units/celsius' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0402.0x0000.0.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "itf_inst_0405_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='RelativeHumidityMeasurement.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0405' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<!--\
<int name='zbMeasuredValue' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0405.0x0000.0.attr'/>\
<int name='zbMinMeasuredValue' val='500'/>\
<int name='zbMaxMeasuredValue' val='9500'/>\
-->\
\
<!-- ZigBee commands -->\
\
<!-- M2M POINTS  -->\
<real name='m2mMeasuredValue' unit='obix:units/percent_relative_humidity' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0405.0x0000.0.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "itf_inst_0406_server" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='interfaceID' val='OccupancyMeasurement.Srv' status='cfg-mandatory'/>\
<str name='clusterID' val='0x0406' status='cfg-mandatory'/>\
<enum name='clusterType' val='server' status='cfg-mandatory'/>\
\
<!-- ZigBee attributes -->\
<!--\
<int name='zbMeasuredValue' href='${s_rootapp}/APP_${d_ieee}.${a_num}/retargeting2/0x0406.0x0000.0.attr'/>\
-->\
\
\
<!-- ZigBee commands -->\
\
<!-- M2M POINTS  -->\
<real name='m2mMeasuredValue' unit='' href='${s_rootapp}/APP_${d_ieee}.${a_num}/containers/0x0406.0x0000.0.m2m/contentInstances/latest/content'/>\
</obj>\
"},
{ "met_cont" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<m2m:container m2m:id='0x${c_num}'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<!--\
<m2m:accessRightID>${s_rootacc}/APP_${d_ieee}.${a_num}_AR</m2m:accessRightID>\
-->\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectSemantic/OASIS.OBIX_1_1</m2m:searchString>\
</m2m:searchStrings>\
</m2m:container>\
"},
{ "net_cont" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
#\
#\
#\
-->\
<m2m:container m2m:id='DESCRIPTOR'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectSemantic/OASIS.OBIX_1_1</m2m:searchString>\
</m2m:searchStrings>\
<m2m:maxNrOfInstances>1</m2m:maxNrOfInstances>\
</m2m:container>\
"},
{ "net_elem" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
## w_netid\
## w_ipuid\
## w_drvcontact\
## w_driverpath\
-->\
<m2m:application appId='NW_${w_netid}'\
xmlns:m2m='http://uri.etsi.org/m2m'>\
\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
<m2m:searchStrings>\
<m2m:searchString>ETSI.ObjectType/ETSI.AN_NWK</m2m:searchString>\
<m2m:searchString>ETSI.ObjectTechnology/ZigBee.ZigBee_1_0</m2m:searchString>\
<m2m:searchString>ETSI.InterworkingProxyID/${w_ipuid}</m2m:searchString>\
<m2m:searchString>ETSI.NetworkID/NW_${w_netid}</m2m:searchString>\
</m2m:searchStrings>\
<m2m:aPoC>coap://${w_drvcontact}/${w_driverpath}/${w_netid}</m2m:aPoC>\
<m2m:aPoCPaths>\
<m2m:aPoCPath>\
<m2m:path>retargeting1</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
<m2m:aPoCPath>\
<m2m:path>retargeting2</m2m:path>\
<m2m:accessRightID>${s_rootacc}/Locadmin_AR</m2m:accessRightID>\
</m2m:aPoCPath>\
</m2m:aPoCPaths>\
</m2m:application>\
"},
{ "net_inst" , "\
<?xml version='1.0' encoding='UTF-8'?>\
<!--\
#\
#	w_ipuid\
#	w_netid\
#	w_panid\
#\
-->\
<obj\
x-template='${w_template}'\
xmlns='http://obix.org/ns/schema/1.1'>\
<str name='networkID' val='NW_${w_netid}'/>\
<str name='extendedPanID' val='0x${w_netid}'/>\
<str name='panID' val='0x${w_panid}'/>\
<list name='nodes'>\
<!--\
See sensor.c:SensorSetDeviceListInObj()\
\
<ref href='${s_rootapp}/DEV_0b435eff56a3bbbb/containers/DESCRIPTOR/contentInstances/latest/content'/>\
-->\
</list>\
</obj>\
"},
};
