
Actility Watteco supervisor (AWSPV) :
#####################################

Environment variables :
-----------------------

ROOTACT : this variable points to the installation directory.

	(ex : /home/actility)

AW_DATA_DIR : this variable points to the remanent and custom data for watteco driver.

	(ex : /home/actility/data/watteco)

Notes :
	- Set and export these variables in your .profile


Software compilation :
----------------------

If you have a source version :

cd $ROOTACT

gzip -d *.tar.gz

for t in *.tar
do
	tar xvf $t
done

for d in rtbase xo m2mxoref libcoap libdIa watteco
do
	cd $d
	./MAKE
	cd ..
done

Notes :
	- tested on UBUNTU 11.04 (32b) CENTOS 5.x (32b) 
	- choose x86 even on x64b systems
	- ignore warnings and errors about java jni in libdIa (if any)
	- run configure in libcoap if some errors occur
	- on ubuntu apt-get install :
		-- libxml2-devel
		-- flex
		-- bison

Directories description $(ROOTACT):
-----------------------------------

- rtbase/: tools like traces, poll(2) handler, thread messages exchange, timers.
- libcoap/: coap protocol library over UDP.
- libdIa/: interface DIA (ETSI M2M) library over COAP.
- xo/: library and tools for XML documents management.
- m2mxoref/: XO definitions for ETSI M2M documents.
- watteco/: adaptor watteco and tools.

Directories description $(ROOTACT)/watteco:
-------------------------------------------

- base/: default compilation options.
- config/: contains default file (spvconfig.xml) for supervisor configuration.
- diatemplates/ : templates xml for DIA requests.
- discover/: http client to get list of sensors (tests only).
- dongle/: source files delivered by watteco.
- include/:
- lib/: source files to encode/decode watteco protocol.
- package/: description for runtime or source packages.
- pseudoong/: ONG simulator.
- sensorconfig/: files to configure reporting of new sensors.
- simulsensors/: software (and simulated) sensors.
- supervisor/: the watteco adaptor.
- tunslip/: the bridge between USB tty interface and IPV6 stack.
- xoref/: specific xo definitions.
- zcllike:/ some tests for watteco protocol.

Directories description $(AW_DATA_DIR):
---------------------------------------

- config/: custom file (svpconfig.xml) for supervisor configuration.
- knownsensors/: contains one file per known sensors.
- discoverpan.txt,prefipv6.txt,routeraddr.txt:watteco network description.

System configuration :
----------------------

Add to /etc/hosts :

<<
::1		localhost
>>


Software configuration :
------------------------

Create file $AW_DATA_DIR/prefipv6.txt which contains the default ipv6 prefixe of the local area network of WATTECO sensors.

<<
aaaa
>>

Physical sensors declaration :
------------------------------

Create file $AW_DATA_DIR/discoverpan.txt with a list of ipv6 sensors address.

<<
aaaa::ff:ff00:2441
aaaa::ff:ff00:2458
aaaa::ff:ff00:240f
>>

Notes : 
	- '#' character at the beginning of a line to comment it.


Physical sensors reporting configuration :
------------------------------------------

For each cluster a configuration file is used from $ROOTACT/watteco/sensorconfig :
	0006-onoff.txt
	0052-simplemetering.txt
	0400-illuminance.txt
	0402-temperature.txt
	0405-humidity.txt

If you want to change configuration for a cluster, copy its default configuration file in $AW_DATA_DIR/sensorconfig and edit the copy. Do not modify the original file.

Notes :
	- awpsv will try to set the reporting configuration to each new sensor.


Already known sensors :
-----------------------

AWSPV keeps a trace of already known sensors in $AW_DATA_DIR/knownsensors. Each sensor has its own file ([num-softsensor@]ipv6_addr_sensor.xml) where :

	- <nicename> == friendly name associated to the device (optional)
	- <configured> == 1 means that the sensor is well configured for 
	reporting.
	- <clusterOut> is the list of clusters supported by the sensor.


Notes :
	- as some versions of watteco do not support "request description" the
	list <clusterOut> can be set manualy. (state WAITINGDESC).
	- to avoid the reporting configuration phase <configured> field can be 
	forced manualy. (state CONFIGURING).

Example :
<<
<?xml version="1.0" encoding="UTF-8"?>
<sensor xmlns="http://uri.actility.com/m2m/adaptor-watteco">
 <nicename>lampe</nicename>
 <configured>1</configured>
 <clusterOut>0006</clusterOut>
 <clusterOut>0052</clusterOut>
</sensor>
>>

AWPSV launch :
--------------

	1) in $ROOTACT/watteco/tunslip, run "tunslip6.sh &" as root user. It is
	not necessary to launch the ipv6/watteco tunneling if you only use 
	software sensors.

	sudo -s tunslip6.sh

	- ifconfig, netstat and awk commands must be in $PATH.

	2) in $ROOTACT/watteco/supervisor, run "awspv.sh" as normal user or use
	RUN.sh

	
	options of awspv.sh (and awspv.x) :

	--version : print software modules versions and exit.
	--help	  : print short help and exit.

	-t trace  : set trace level [0..10]
	-d debug  : set debug mode [0|1] just add __FILE__/__LINE__ to traces
	-D        : desactive DIA protocol
	-O        : use a pseudo ONG
	-S        : enable software sensors
	-X        : active full DIA/XML traces on stdout


AWPSV tunning :
--------------

	In place of lauching options it is recommended to use configuration file
	which is read from :
		1) $ROOTACT/watteco/config/spvconfig.xml
		2) $AW_DATA_DIR/config/spvconfig.xml

	In this last file (2) you can also change or add some variables.
<<
	<?xml version="1.0" encoding="UTF-8"?>
	<!--
	     Do not modify this file which is included in delivery.
	     Copy it in $AW_DATA_DIR and edit the copy.
	-->
	<obj xmlns="http://obix.org/ns/schema/1.1">

	<!--
	     parameters
	-->
	<list name="parameters">
		<int name="tracelevel" val="0"/>
		<int name="tracedebug" val="1"/>
		<int name="withdia" val="1"/>
		<int name="tracedia" val="0"/>
		<int name="pseudoong" val="0"/>
		<int name="softwaresensors" val="1"/>
	</list>

	<!--
	     variables
	-->
	<list name="variables">
		<str name="w_driverid" val="GIP"/>
		<str name="w_driverpath" val="6lowpan"/>
		<str name="w_ipuid" val="IPU_6LoWPAN"/>
		<str name="w_manufacturer" val="1234"/>
		<str name="w_panid" val="0000"/>

		<str name="w_domainname" val="actility"/>
		<str name="w_boxname" val="CYH-00FADAFADAFA"/>

		<str name="w_coapaddr_l" val="::1"/>
		<str name="w_coapport_l" val="5684"/>

		<str name="w_coapaddr_r" val="::1"/>
		<str name="w_coapport_r" val="5683"/>
	</list>
	</obj>
>>


AWPSV supervision :
-------------------

To supervise AWSPV connect to localhost(ipv4) port 2000 with telnet.
Password "123"

Software sensors (simulation) :
###############################


If you only use software sensors you have to create manually a fake watteco router configuration file in $AW_DATA_DIR named routeraddr.txt :

<<
@1=aaaa::da30:62ff:fe60
>>

Notes :
	- this file is normaly created by the tunslip6.x command when the
	watteco router is detected. 



Software sensors declaration :
------------------------------

Create file $AW_DATA_DIR/discoversoft.txt with a list of software sensors. Software sensors are hosted by localhost(6) at [::1].

<<
1-cpuload@::1
1-tempsoft@::1
2-humisoft@::1
3-temphumisoft@::1
4-allsoft@::1
5@::1
6@::1
>>

Notes : 
	- '#' character at the beginning of a line to comment it.
	- the string xxxx after num-xxxx@::1 has no meaning (just for help)
	- software sensors #0 and #17 are forbidden, limited to 255.


Software sensors simulator :
----------------------------


Two very (very ...) basic software/simulated sensors are available :
	- cpu.x mimics one temperature sensor and returns cpu usage instead.
	- simul.x is a N-sensors of type 0402, 0402&0405, 0402&0405&0400, but
	just responds to WATTECO configuration cluster (0050).

In $ROOTACT/watteco/simulsensors run cpu.x or (xor) simul.x

Notes :
	- Physical and software sensors can be used simultaneously.
	- AWSPV must be launched with -S option to authorize software sensors.


Pseudo ONGV2 (simulation) :
###########################

If ONGV2 is not available it is possible to use a pseudo ONG to simulate the DIA link. Resources are created as directories, associated XML content are stored in files.

Copy all files from $ROOTACT/pseudoong to $AW_DATA_DIR/pseudoong (example) and start pseudo_ong.sh. All SCL base will be created in AW_DATA_DIR/pseudoong/m2m.

Some shells can be used to validate (very partialy) the pseudo SCL.

Notes :
	- AWSPV must be launched with -O option.



Mapping of the WATTECO area network to the ETSI M2M resources :
###############################################################

A important part of this mapping is configurable via XML templates files located in $ROOTACT/watteco/diatemplates.


X : [1..1]
Y : [0..10]
Z : [1..1]
W : [1..10]
N : [1..15]		// 3 attributs * 5 members
M : [1..5]

Interworking Proxy Unit (*1) :
------------------------------
ipu_elem.xml
	ipu_cont.xml
		ipu_inst.xml

Network (*X) :
--------------
net_elem.xml
	net_cont.xml
		net_inst.xml

Devices (*Y) :
--------------
dev_elem.xml
	dev_cont.xml
		dev_inst.xml

Applications (*Z) :
-------------------
app_elem.xml
	app_cont.xml
		app_inst.xml + itf_inst_<cluster>_<server|client>.xml (*W) 
	dat_cont.xml
		dat_inst_<cluster>_<attribut>_<member>.xml (*N)
	met_cont.xml
		met_inst_<cluster>_<command>.xml (*M)


Notes :
	- met_cont.xml is not implemented
	- met_inst_<cluster>_<command>.xml is not implemented


Watteco attributs :
-------------------

As Watteco attributs can have multiple values of different types a member number is added.


Mapping of clusters/attributs :
-------------------------------

Cluster name				Cluster		Attribut	Member
	Attribut name

OnOffServerCluster.Srv			0x0006

	zclOnOff			0x0006		0x0000		0

SimpleMeteringServerCluster.Srv		0x0052

	zclCurrentSummationDelivered	0x0052		0x0000		0
	zclInstantaneousDemand		0x0052		0x0000		3


IlluminanceMeasurement.Srv		0x0400

	zclMeasuredValue		0x0400		0x0000		0

TemperatureMeasurement.Srv		0x0402

	zclMeasuredValue		0x0402		0x0000		0

RelativeHumidityMeasurement.Srv		0x0405

	zclMeasuredValue		0x0405		0x0000		0



Mapping of clusters/points :
----------------------------

Cluster name				Cluster		Attribut	Member
	Point name

OnOffServerCluster.Srv			0x0006

	m2mOnOff			0x0006		0x0000		0

SimpleMeteringServerCluster.Srv		0x0052

	m2mCurrentSummationDelivered	0x0052		0x0000		0
	m2mInstantaneousDemand		0x0052		0x0000		3

IlluminanceMeasurement.Srv		0x0400

	m2mMeasuredValue		0x0400		0x0000		0

TemperatureMeasurement.Srv		0x0402

	m2mMeasuredValue		0x0402		0x0000		0

RelativeHumidityMeasurement.Srv		0x0405

	m2mMeasuredValue		0x0405		0x0000		0



Mapping of clusters/commands :
------------------------------

Cluster name				Cluster		Command
	Command name

OnOffServerCluster.Srv			0x0006

	zclOff				0x0006		0x00
	zclOn				0x0006		0x01
	zclToggle			0x0006		0x02

SimpleMeteringServerCluster.Srv		0x0052

	zclRest				0x0052		0x00

IlluminanceMeasurement.Srv		0x0400

TemperatureMeasurement.Srv		0x0402

RelativeHumidityMeasurement.Srv		0x0405


Start writting a new adaptor :
##############################

All tools from the framework (rtbase,coap,xo,dia,m2mxoref) do not depend on sensor type. All of them are mandatory to write a new adaptor.

Unuseful directories in watteco/ :
----------------------------------

- discover/
- dongle/
- sensorconfig/ (perhaps useful if a file is required to configure each cluster)
- simulsensors/
- tunslip/
- zcllike/


Useful files in watteco/lib/ :
------------------------------

- ctools.c
- sensor.c if the list of (new/deleted) sensors can be managed with basic files.

All other files in watteco/lib/ are watteco specific.




Files in watteco/supervisor/ dependences on watteco (no/low/strong/specific) :
-----------------------------------------------------------------------------

- admtcp.c : no
- dia*.[ch] : no
- config.c : no (perhaps useful if a file is required to configure each cluster)
- main.c : low
- var.c : no
- xmltools.c : no
- http.c : specific
- sensor.c : specific
- admcli.c : specific
- zcludp.c : specific




