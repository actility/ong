#! /bin/bash

CURRENT=$(pwd)
APU_MAKE=apu-make
APU_INSTALL=apu-install-stdalone.sh
BUILD_INTALLER=1
PRGM_NAME=$(basename $0)

usage() {
  echo "usage: $PRGM_NAME <options> <target>"
  echo "where <target> can be {centos6-x86|centos6-x86_64|cov1|cov2|ntc6200|rpib}"
  echo "where <options> can be:"
  echo " -h|--help:         display this help message"
  echo " -n|--no-installer: skip the compilation for ong-installer project"
  echo 
}


GETOPTTEMP=`/usr/bin/getopt -o hn --long help,no-installer -- "$@"`
if [ $? != 0 ] ; then usage>&2 ; exit 1 ; fi
eval set -- "$GETOPTTEMP"

while true ; do
       case "$1" in
               -h|--help) usage; exit;;
               -n|--no-installer) BUILD_INTALLER=0 ; shift ;;
               --) shift ; break ;;
               *) echo "Internal error!" ; exit 1 ;;
        esac
done

if [ $# -ne 1 ]; then
  usage
  exit 1
fi

TARGET=$1

abortWithMsg() {
  echo "*************************************" >&2
  echo "*** "$1"; aborted" >&2
  cd $CURRENT
  exit 1
}

 
apuBuild() {
  SUB_MOD_TARGET=$1
  ROOTACT=$(cd .. && pwd) eval $APU_MAKE install $SUB_MOD_TARGET || \
  abortWithMsg "failed to build apu in $(pwd)"
}

apuInstall() {
  APU=$1
  TARGET_DIR=$2
  echo "Installing $APU in $TARGET_DIR"
  mkdir -p $TARGET_DIR || \
  abortWithMsg "Failed to create TARGET_DIR dir ($TARGET_DIR) while installing $APU"
  cd $TARGET_DIR
  ROOTACT=$(pwd) eval $APU_INSTALL -y $APU || \
  abortWithMsg "failed to install APU $APU"
  cd $CURRENT
}

apuInstallTarget() {
  SUB_MOD_TARGET=$1
  ROOTACT=$(cd .. && pwd) eval $APU_MAKE install-target $SUB_MOD_TARGET || \
  abortWithMsg "failed to install target $SUB_MOD_TARGET"
}

mavenBuild() {
  SUB_MOD_TARGET=$1
  OPTIONS="-Dmaven.test.skip=true"
  [ -n "$SUB_MOD_TARGET" ] && OPTIONS=$OPTIONS" -Dbuild.type=$SUB_MOD_TARGET -Dbuild.arch=$ARCH"
  
  echo mvn clean install $OPTIONS
  mvn clean install $OPTIONS || \
  abortWithMsg "failed to build jar"
}

build() {
  SUB_MOD_PATH=$1
  SUB_MOD_TARGET=$2
  SUB_MOD_SUPPORTED_TARGETS=$3

  echo "==============================================================================="
  echo
  echo "    Building sub-module $SUB_MOD_PATH for $TARGET"
  echo
  echo "==============================================================================="
  
  if [ -n "$SUB_MOD_SUPPORTED_TARGETS" ]; then
    SUPPORTED_TARGETS=$(echo $SUB_MOD_SUPPORTED_TARGETS | tr -s '{}' '|')
    if ! [[ "|$SUPPORTED_TARGETS|" =~ "|$TARGET|" ]]; then
      echo "This module is not required for this target; skipped."
      return
    fi
  fi

  cd $SUB_MOD_PATH || abortWithMsg "$SUB_MOD_PATH not found"
  if [ -d "apu" ]; then
    echo "Found apu folder; invoke apu-make"
    apuBuild $SUB_MOD_TARGET
  elif [ -f "pom.xml" ]; then
    echo "Found maven project; invoke maven"
    mavenBuild $SUB_MOD_TARGET
  else
    abortWithMsg "Unable to detect how to build in $SUB_MOD_PATH"
  fi
  cd $CURRENT
}

chmod +x dev-tools/apu-tools/$APU_MAKE
chmod +x dev-tools/apu-tools/$APU_INSTALL
export PATH=$(pwd)/dev-tools/apu-tools/:$PATH

[ -x $(which $APU_MAKE) ] || abortWithMsg "apu-make not found"
[ -z "$TARGET" ] && usage && abortWithMsg "<target> is missing"
[ -f $HOME/.apu/apu-tools.conf ] || abortWithMsg "APU environment not set; please refer to README file"
source $HOME/.apu/apu-tools.conf

case "$HOST_TARGET" in
	"centos6-x86")
		if [ -z "$($APU_MAKE ls-targets | grep ^centos6-x86$)" ]; then
			build targets/target-centos6-x86 noarch
			apuInstallTarget centos6-x86
		fi
		;;
	"centos6-x86_64")
		if [ -z "$($APU_MAKE ls-targets | grep ^centos6-x86_64$)" ]; then
			build targets/target-centos6-x86_64 noarch
			apuInstallTarget centos6-x86_64
		fi
		;;
	*)
		abortWithMsg "Unsupported HOST_TARGET in $HOME/.apu/apu-tools.conf ($HOST_TARGET)"
		;;
esac

case "$TARGET" in

	"centos6-x86")
		if [ "$HOST_TARGET" = "centos6-x86_64" ] && [ -z "$($APU_MAKE ls-targets | grep ^cross-centos6-x86$)" ]; then
			build targets/target-cross-centos6-x86 noarch
			apuInstallTarget centos6-x86
		fi
		;;

	"centos6-x86_64")
		# nothing to do
		;;

	"cov1")
		if [ -z "$($APU_MAKE ls-targets | grep ^cross-cov1$)" ]; then
			build targets/target-cross-cov1 noarch
			apuInstallTarget cov1
		fi
		;;

	"cov2")
		if [ -z "$($APU_MAKE ls-targets | grep ^cross-cov2$)" ]; then
			build targets/target-cross-cov2 noarch
			apuInstallTarget cov2
		fi
		;;

	"rpib")
		if [ -z "$($APU_MAKE ls-targets | grep ^cross-rpib$)" ]; then
			build targets/target-cross-rpib noarch
			apuInstallTarget rpib
		fi
		;;

  "ntc6200")
    if [ -z "$($APU_MAKE ls-targets | grep ^cross-ntc6200$)" ]; then
      build targets/target-cross-ntc6200 noarch
      apuInstallTarget ntc6200
    fi
    ;;

	*)
		abortWithMsg "Unsupported TARGET ($TARGET)"
		;;

esac

ARCH=$($APU_MAKE env $TARGET | grep "export ARCH=" | sed -e "s|^export ARCH=\(.*\)$|\1|")

# now build all sub-modules in the right order
build common/makefile-common noarch
build common/rtbase $TARGET
[ "$HOST_TARGET" != "$TARGET" ] && build common/rtbase $HOST_TARGET

build external/argp-standalone $TARGET "{lpv3}"
build external/libiconv $TARGET
build external/pthsem $TARGET "{cov2|rpib|lpv3|ntc6200|centos6-x86|centos6-x86_64}"
build external/eibd $TARGET "{cov2|rpib|lpv3|ntc6200|centos6-x86|centos6-x86_64}"
build external/libxml2 $TARGET
[ "$HOST_TARGET" != "$TARGET" ] && build external/libxml2 $HOST_TARGET
build external/mxml $TARGET
build external/ntpclient $TARGET "{cov1|cov2|rpib}"
build external/libpcap $TARGET
build external/tcpdump $TARGET
build external/libmodbus $TARGET "{cov2|rpib|lpv3|ntc6200|centos6-x86|centos6-x86_64}"
build external/sqlite $TARGET
build external/curl $TARGET
build external/cproto $HOST_TARGET
build external/libmicrohttpd $TARGET
build external/jni $TARGET
build external/exip $TARGET "{centos6-x86}"
build external/phoneme-advanced-mr2 $TARGET "{cov1|cov2|rpib|lpv3|ntc6200|centos6-x86}"
PHONEME_VERSION=$(cat external/phoneme-advanced-mr2/Version)-$(cat external/phoneme-advanced-mr2/apu/revision)
export PHONEME_TARGET_DIR=$CURRENT/.build/$TARGET/phoneme
PHONEME_APU=$HOME/.m2/repository/com/sun/phoneme-advanced-mr2/$PHONEME_VERSION/phoneme-advanced-mr2-$PHONEME_VERSION-$TARGET.apu
apuInstall $PHONEME_APU $PHONEME_TARGET_DIR
export PHONEME_HOME=$PHONEME_TARGET_DIR/bin/phoneme


build common/java.pom
build common/java.cdc.pom
build dev-tools/apu-tools noarch
build dev-tools/apu-maven-plugin
build dev-tools/cocoon-maven-plugin

build common/xo $TARGET
[ "$HOST_TARGET" != "$TARGET" ] && build common/xo $HOST_TARGET
build common/libcoap $TARGET
build common/libdIa $TARGET
build common/m2mxoref $HOST_TARGET
build common/stdin-logger $TARGET
build common/trans_pty $TARGET
build common/tty_mapper $TARGET
build common/libSongOverHttp $TARGET
build common/libHttpSubsMgmt $TARGET
build common/supervision noarch
build common/acy-init noarch
build drivers/AZAP $TARGET
build drivers/drvcommon $TARGET
build drivers/zigbee $TARGET
build drivers/watteco $TARGET
build drivers/wmbus $TARGET
build drivers/knx $TARGET "{cov2|rpib|lpv3|ntc6200|centos6-x86|centos6-x86_64}"
build drivers/iec61131 $TARGET "{cov1|cov2|rpib|lpv3|ntc6200|centos6-x86}"
build drivers/modbus $TARGET "{cov2|rpib|lpv3|ntc6200|centos6-x86|centos6-x86_64}"
build gscl/backend.api
build gscl/storage.api
build gscl/storage.driver.api
build gscl/jxo.api
build gscl/m2m.api
build gscl/song.api
build gscl/jxo.ni.api
build gscl/cm.api
build gscl/framework.resources.api
build gscl/log.api
build gscl/shutdown.hook.ni.api
build gscl/song.binding.coap.ni.api
build gscl/song.binding.http.api
build gscl/song.binding.http.ni.api
build gscl/storage.driver.sqlite.ni.api
build gscl/transport.logger.api
build gscl/org.apache.felix.shell.acy.api

build gscl/util
build gscl/backend
build gscl/org.apache.felix.shell.acy
build gscl/org.apache.felix.shell.tui.acy
build gscl/felix.shell.telnet
build gscl/song
build gscl/backend.command.shell
build gscl/song.command.shell
build gscl/song.trace.command.shell
build gscl/cm.command.shell
build gscl/log.command.shell
build gscl/inspector.command.shell
build gscl/phoneme.command.shell

build gscl/m2m
build gscl/storage.me
build gscl/system.version
build gscl/shutdown.hook
build gscl/storage.driver.sqlite $TARGET
build gscl/jxo
build gscl/song.binding.http
build gscl/song.binding.coap
build gscl/scl.app.me
build gscl/cm
build gscl/framework.command.shell.me
build gscl/framework.command.shell.se
build gscl/song.binding.http.command.shell
build gscl/jxo.jni $TARGET
build gscl/framework.resources.osgi
build gscl/log
build gscl/shutdown.hook.jni $TARGET
build gscl/song.binding.coap.jni $TARGET
build gscl/song.binding.http.jni $TARGET
build gscl/storage.driver.sqlite.jni $TARGET
build gscl/transport.logger.log
build gscl/gscl $TARGET

if [ $BUILD_INTALLER -eq 1 ]; then
  build installer/ong-installer
fi

echo "done"
 
