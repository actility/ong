
buildProject()
{
  cd xo && \
  make clean && \
  make

  return $?
}

buildApuImage()
{

  XO_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data/xo/include/ && \
  mkdir -p apu/data/xo/lib/ && \
  mkdir -p apu/data/shlib/ && \
  mkdir -p apu/control && \
  cp xo/include/*.h apu/data/xo/include/ && \
  cp xo/lib/lib/*.a apu/data/xo/lib/ && \
  cp xo/lib/lib/libxo.so apu/data/shlib/libxo-$XO_VERSION.so && \
  echo "cd \$ROOTACT/shlib" > apu/control/postinst && \
  echo "ln -s libxo-$XO_VERSION.so libxo.so" >> apu/control/postinst && \
  if [ -f xo/libexi/lib/libxoexi.so ]
  then
    cp xo/libexi/lib/libxoexi.so apu/data/shlib/libxoexi-$XO_VERSION.so && \
    echo "cd \$ROOTACT/shlib" >> apu/control/postinst && \
    echo "ln -sf libxoexi-$XO_VERSION.so libxoexi.so" >> apu/control/postinst
  fi
}

projectName()
{
  echo "xo-devel"
}

projectDescription()
{
  echo "Headers from utility library to manipulate M2M and oBIX documents"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

