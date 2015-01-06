
buildProject()
{
  cd rtbase && \
  make clean && \
  make
  return $?
}

buildApuImage()
{
  RTBASE_VERSION=`cat rtbase/Version`
  rm -rf apu && \
  mkdir -p apu/data/lib && \
  mkdir -p apu/control && \
  mkdir -p apu/data/etc/rtbase/base && \
  cp rtbase/base/TARGET apu/data/etc/rtbase/base && \
  cp rtbase/lib/lib/librtbase.so apu/data/lib/librtbase-$RTBASE_VERSION.so && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -sf librtbase-$RTBASE_VERSION.so librtbase.so" >> apu/control/postinst
}

projectName()
{
  echo "rtbase"
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

