
buildProject()
{
  ROOTACT=`pwd`

  case "$1" in 
    cov1)
        CC="arm-linux-gcc -mcpu=arm920t"
        ;;
    cov2)
        CC="arm-none-linux-gnueabi-gcc -mcpu=arm920t"
        ;;
    rpib)
        source $TOOLCHAIN_HOME/environment-setup-armv6-vfp-poky-linux-gnueabi
        PREFIX=arm-poky-linux-gnueabi
        ;;
    *)
        echo "Invalid arch or no arch specified..."
        exit 1
        ;;
  esac

  cd ntpclient
  rm -rf build
  mkdir build
  tar zxvf ntpclient-2010.365.tgz -C build
  cd build
  make CC="$CC" ARCH=$1 || (echo "FAILED to compile" && exit 1)

  return 0
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin
  mkdir -p apu/data/etc/init.d
  mkdir -p apu/data/etc/ntp
  mkdir -p apu/control
 
  cp -r ntpclient/build/ntpclient apu/data/bin

  cp ntpclient/apu/$1/ntp-init apu/data/etc/init.d/ntp 
  cp ntpclient/apu/$1/ntp.cfg apu/data/etc/ntp/ntp.cfg
  cp ntpclient/apu/$1/postinst apu/control/postinst 
}

projectGroupId()
{
  echo "com.icarus"
}

projectName()
{
  echo "ntpclient"
}

projectVersion()
{
  cat ntpclient/Version
}

projectApuRev()
{
  cat ntpclient/apu/revision
}

projectDescription()
{
  echo "ntpclient is a light library that synchronizes machine time with an NTP server"
}

projectArchDepend()
{
  # yes, need arch for building this apu
  return 0
}


