
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
  mkdir -p apu/data/drvcommon/lib/
  mkdir -p apu/data/shlib/
  mkdir -p apu/control

  SELF_VERSION=`projectVersion`

  cp drvcommon/lib/*.h apu/data/drvcommon/lib/
  cp drvcommon/lib/*.a apu/data/drvcommon/lib/
  cp drvcommon/lib/libdrvcommon.so apu/data/shlib/libdrvcommon-$SELF_VERSION.so
  echo "cd \$ROOTACT/shlib" > apu/control/postinst
  echo "ln -s libdrvcommon-$SELF_VERSION.so libdrvcommon.so" >> apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "drvcommon-devel"
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

