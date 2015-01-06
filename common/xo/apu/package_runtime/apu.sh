
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
  mkdir -p apu/data/lib && \
  mkdir -p apu/control && \
  cp xo/lib/lib/libxo.so apu/data/lib/libxo-$XO_VERSION.so && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -sf libxo-$XO_VERSION.so libxo.so" >> apu/control/postinst

  RET1=$?
  RET2=0
  if [ -f xo/libexi/lib/libxoexi.so ]
  then
	  cp xo/libexi/lib/libxoexi.so apu/data/lib/libxoexi-$XO_VERSION.so && \
	  echo "cd \$ROOTACT/lib" >> apu/control/postinst && \
	  echo "ln -sf libxoexi-$XO_VERSION.so libxoexi.so" >> apu/control/postinst

    RET2=$?
  fi
  return $RET1 && $RET2
}

projectName()
{
  echo "xo"
}

projectDescription()
{
  echo "Utility library to manipulate M2M and oBIX documents"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

