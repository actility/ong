
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
  mkdir -p apu/data/rtbase/include/ && \
  mkdir -p apu/data/rtbase/base && \
  mkdir -p apu/data/rtbase/shells && \
  mkdir -p apu/data/rtbase/lib && \
  mkdir -p apu/data/shlib && \
  mkdir -p apu/control && \
  cp rtbase/lib/lib/librtbase.so apu/data/shlib/librtbase-$RTBASE_VERSION.so && \
  cp rtbase/include/* apu/data/rtbase/include/ && \
  cp rtbase/base/* apu/data/rtbase/base/ && \
  cp rtbase/shells/*.sh apu/data/rtbase/shells/ && \
  cp rtbase/lib/lib/*.a apu/data/rtbase/lib/ && \
  echo "cd \$ROOTACT/shlib" > apu/control/postinst && \
  echo "ln -s librtbase-$RTBASE_VERSION.so librtbase.so" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/rtbase/base/*.sh" >> apu/control/postinst
}

projectName()
{
  echo "rtbase-devel"
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

