
buildProject()
{
  cd rtbase
  
#  CHOICE=1
#  case "$1" in
#    centos6-x86) CHOICE=6;;
#    centos6-x86_64) CHOICE=7;;
#    cov1) CHOICE=3;;
#    cov2) CHOICE=8;;
#    lpv3) CHOICE=9;;
#    *) return 1;;
#  esac
#  ./MAKE clean
#  echo $CHOICE | ./MAKE

  ./MAKE clean
  echo $1 | ./MAKE
  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/rtbase/include/
  mkdir -p apu/data/rtbase/base
  mkdir -p apu/data/rtbase/shells
  mkdir -p apu/data/rtbase/lib
  mkdir -p apu/data/shlib
  mkdir -p apu/control
  
  RTBASE_VERSION=`cat rtbase/Version`

  cp rtbase/lib/librtbase.so apu/data/shlib/librtbase-$RTBASE_VERSION.so
  cp rtbase/include/* apu/data/rtbase/include/
  cp rtbase/base/* apu/data/rtbase/base/
  cp rtbase/shells/*.sh apu/data/rtbase/shells/
  cp rtbase/lib/*.a apu/data/rtbase/lib/

  echo "cd \$ROOTACT/shlib" > apu/control/postinst
  echo "ln -s librtbase-$RTBASE_VERSION.so librtbase.so" >> apu/control/postinst
  echo "chmod +x \$ROOTACT/rtbase/base/*.sh" >> apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "rtbase-devel"
}

projectVersion()
{
  cat rtbase/Version
}

projectApuRev()
{
  cat rtbase/apu/revision
}

projectDescription()
{
  echo "Headers from base library for Actility M2M Drivers"
}

projectArchDepend()
{
  # yes, need arch for this apu
  return 0
}

