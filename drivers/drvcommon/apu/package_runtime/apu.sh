
buildProject()
{
  cd drvcommon && \
  make clean && \
  make

  return $?
}

buildApuImage()
{

  DRVCOMMON_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data/lib && \
  mkdir -p apu/control && \
  cp drvcommon/lib/lib/libdrvcommon.so apu/data/lib/libdrvcommon-$DRVCOMMON_VERSION.so && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -sf libdrvcommon-$DRVCOMMON_VERSION.so libdrvcommon.so" >> apu/control/postinst
}

projectName()
{
  echo "drvcommon"
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

