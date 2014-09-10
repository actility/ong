
applyPatch()
{
  echo -n "Apply $1..."
  patch -p1 -i $1 || (cd - && echo "FAILED to apply patch" && exit 1)
  echo "OK"
}

buildProject()
{
  ROOTACT=`pwd`
  FLAGS="-fPIC"
  case "$1" in 
    centos6-x86)
      CROSS_COMPILATION=0
      ;;
    cov1)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-linux
      ;;
    cov2)
      CROSS_COMPILATION=1
      FLAGS=-mcpu=arm920t
      PREFIX=arm-none-linux-gnueabi
      ;;
    lpv3)
      CROSS_COMPILATION=1
      PREFIX=mips-openwrt-linux-uclibc
      ;;
    *)
      ;;
  esac

  cd argp-standalone
  tar zxvf argp-standalone-1.3.tar.gz
  cd argp-standalone-1.3/

  applyPatch $ROOTACT/argp-standalone/patches/001-throw-in-funcdef.patch

  if [ ! -f Makefile ]; then
    if [ $CROSS_COMPILATION = 1 ]; then
        if [ "x" != "x$FLAGS" ]; then
            CFLAGS="$FLAGS" LDFLAGS="$FLAGS" ./configure --host $PREFIX --prefix $ROOTACT
        else
            ./configure --host $PREFIX --prefix $ROOTACT
        fi
    else
        ./configure --prefix $ROOTACT
    fi
  fi
  make

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/include/
  mkdir -p apu/data/lib/
  mkdir -p apu/control
 
  PCAP_VERSION=`projectVersion`

  cp argp-standalone/argp-standalone-1.3/argp.h apu/data/include
  cp argp-standalone/argp-standalone-1.3/libargp.a apu/data/lib
}

projectGroupId()
{
  echo "org.FreeBSD"
}

projectName()
{
  echo "argp-standalone-devel"
}

projectVersion()
{
  echo "1.3"
}

projectApuRev()
{
  cat argp-standalone/apu/revision
}

projectDescription()
{
  echo "GNU libc hierarchial argument parsing library broken out from glibc."
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}


