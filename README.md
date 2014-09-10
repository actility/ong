This folder contains all the sources required to generate ONG software.
ONG is the Actility implementation of ETSI M2M GSCL function.
Please refer to http://cocoon.actility.com/

This way of presenting the ONG sources is a firt attempt to provide all the
sources at once, as well as to offer a manner to generate the whole ONG 
packages. This is not the preferrable way to work on ONG.
Once you've generate all the sub-modules once, you can re-work on the
closest sub-modules of the ONG you are interested in.


Installing the compilation environment:
=======================================
- Install "centos6.3 i386" or "centos6.3 x86_64" Linux distrib, with 
"Basic Server" installation profile
- Install "Development tools" group package
	$ yum groupinstall Development tools
- Install a recent JDK
	$ yum install java-1.7.0-openjdk-devel.i686
- Download Maven 3 from http://maven.apache.org/download.cgi, and follow 
the instructions for installation
- Download Actility boot-repository on http://cocoon.actility.com/download 
and unpack it in $HOME/.m2/repository

	$ mkdir -p $HOME/.m2/repository
	$ tar zxvf boot-repository-*.tgz -C $HOME/.m2/repository


Retrieve the ONG sources:
=========================
we assume that you clone the ong project into /usr/local/src/ directory:

	$ cd /usr/local/src/
	$ git clone https://github.com/actility/ong.git


Configuring APU environment:
============================
APU stands for Actility Package Unit. This is the elementary installable 
entity for the ONG. "apu" is the file format used for none-java modules.
The format of an "apu" file is close to the IPKG one 
(http://en.wikipedia.org/wiki/Ipkg).
"apu" file are generated with "apu-make" tool, which is written in Linux
shell.

It requires a few configuration in order to run:

Define APU building environment:
--------------------------------
  - Define APU_HOME environment variable (for instance in your .bashrc file)
	œexport APU_HOME=/usr/local/src/ong/dev-tools/apu-tools/
  - Add APU_HOME to your PATH
	export PATH=$PATH:$APU_HOME
  - Create a folder so called ".apu" in your homedir
	$ mkdir $HOME/.apu/
  - and add file $HOME/.apu/apu-tools.conf with following content:
    

	NEXUS_SRV_ADDR=<your-nexus-server-ip-address-or-blank>
	NEXUS_SRV_PORT=<your-nexus-server-port-or-blank>
	HOST_TARGET="centos6-x86"
	HOST_ARCH="x86"
	NEXUS_BASE=http://$NEXUS_SRV_ADDR:$NEXUS_SRV_PORT/nexus

  - Don't forget to source your .bashrc file if you've changed it.

Compiling for CentOs:
=====================
- Change to directory "ong" and run "./build_ong.sh centos6-x86"


Cross-compiling:
================

Before you can cross-compile, you'll have to compile the ONG for the HOST_TARGET (i.e. 
centos6-x86 or centos6-x86_64)

Cross-compiling for Raspberry Pi B (rpib):
------------------------------------------
Toolchain to use is provided with "poky-eglibc-i686-arm-toolchain-1.4.1.sh", 
for Raspberry Pi B
1. install the toolchain in $HOME/toolchains/poky/1.4.1/
2. Create the path $HOME/.apu/toolchains/org/yoctoproject/toolchain-poky-eglibc-arm/1.4.1/
and add file toolchain.cfg in it, with the following content:

	export TOOLCHAIN_HOME=$HOME/toolchains/poky/1.4.1/
	export PATH=$TOOLCHAIN_HOME/sysroots/i686-pokysdk-linux/usr/bin/armv6-vfp-poky-linux-gnueabi:$PATH

3. Go to the source directory:
	$ cd /usr/local/src/ong/targets/target-cross-rpib
4. Generate the APU file:
	$ apu-make install noarch
5. Install the APU file:
	$ apu-make install-target rpib
6. Then, you can build the ONG for rpbi
	$ cd /usr/local/src/ong/
	$ ./build_ong.sh rpib


Cross-compiling for "Connected Object" v2 (cov2):
-------------------------------------------------
Toolchain to use is provided with "gcc_4.5.1.tar.bz2", for ARM 920T
1. Install the toolchain in $HOME/toolchains/cov2/4.5.1/
2. Create the path
$HOME/.apu/toolchains/com/actility/m2m/toolchain-arm-none-linux-gnueabi/4.5.1/
and add file toolchain.cfg in it, with the following content:

	export TOOLCHAIN_HOME=$HOME/toolchains/cov2/4.5.1/
	export PATH=$TOOLCHAIN_HOME/bin:$PATH

3. Go to the source directory:
	$ cd /usr/local/src/ong/targets/target-cross-cov2
4. Generate the APU file:
	$ apu-make install noarch
5. Install the APU file:
	$ apu-make install-target cov2
6. Then, you can build the ONG for cov2
	$ cd /usr/local/src/ong/
	$ ./build_ong.sh cov2


Cross-compiling for centos6-x86 under centos6-x86_64:
-----------------------------------------------------
You may also want to install a 64bits OS, but need to cross-compile for 32bits target.
To do that, you'll need to generate and install the target-cross-centos6-x86 APU:

1. Go to the source directory:
	$ cd /usr/local/src/ong/targets/target-cross-centos6-x86
2. Generate the APU file:
	$ apu-make install noarch
3. Install the APU file:
	$ apu-make install-target cross-centos6-x86
4. Then, you can build the ONG for centos6-x86
	$ cd /usr/local/src/ong/
	$ ./build_ong.sh centos6-x86




