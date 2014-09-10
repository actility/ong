
buildProject()
{
  cd drvcommon
  ./MAKE clean
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/lib
  mkdir -p apu/control

  DRVCOMMON_VERSION=`projectVersion`

  cp drvcommon/lib/libdrvcommon.so apu/data/lib/libdrvcommon-$DRVCOMMON_VERSION.so
  echo "cd \$ROOTACT/lib" > apu/control/postinst
  echo "ln -sf libdrvcommon-$DRVCOMMON_VERSION.so libdrvcommon.so" >> apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "drvcommon"
}

projectVersion()
{
  cat drvcommon/Version
}

projectApuRev()
{
  cat drvcommon/apu/revision
}

projectDescription()
{
  echo "library for M2M drivers"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

