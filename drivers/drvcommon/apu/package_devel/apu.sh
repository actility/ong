
buildProject()
{
  cd drvcommon && \
  make clean && \
  make

  return $?
}

buildApuImage()
{

  SELF_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data/drvcommon/lib/ && \
  mkdir -p apu/data/lib/ && \
  mkdir -p apu/control && \
  cp drvcommon/lib/*.h apu/data/drvcommon/lib/ && \
  cp drvcommon/lib/lib/*.a apu/data/drvcommon/lib/ && \
  cp drvcommon/lib/lib/libdrvcommon.so apu/data/lib/libdrvcommon-$SELF_VERSION.so && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -s libdrvcommon-$SELF_VERSION.so libdrvcommon.so" >> apu/control/postinst
}

projectName()
{
  echo "drvcommon-devel"
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

