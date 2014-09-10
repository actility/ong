
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
  mkdir -p apu/data/lib
  mkdir -p apu/control
  mkdir -p apu/data/etc/rtbase/base
  
  RTBASE_VERSION=`cat rtbase/Version`
 
  cp rtbase/base/TARGET apu/data/etc/rtbase/base
  cp rtbase/lib/librtbase.so apu/data/lib/librtbase-$RTBASE_VERSION.so
  echo "cd \$ROOTACT/lib" > apu/control/postinst
  echo "ln -sf librtbase-$RTBASE_VERSION.so librtbase.so" >> apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "rtbase"
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
  echo "Base library for Actility M2M Drivers"
}

projectArchDepend()
{
  # yes, need arch for this apu
  return 0
}

