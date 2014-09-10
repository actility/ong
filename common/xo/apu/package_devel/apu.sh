
buildProject()
{
  cd exip
  ./MAKE clean
  ./MAKE

  cd ../xo
  ./MAKE clean
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/xo/include/
  mkdir -p apu/data/xo/lib/
  mkdir -p apu/data/shlib/
  mkdir -p apu/control

  XO_VERSION=`projectVersion`

  cp xo/include/*.h apu/data/xo/include/
  cp xo/lib/*.a apu/data/xo/lib/
  cp xo/lib/libxo.so apu/data/shlib/libxo-$XO_VERSION.so
  echo "cd \$ROOTACT/shlib" > apu/control/postinst
  echo "ln -s libxo-$XO_VERSION.so libxo.so" >> apu/control/postinst

  if [ -f xo/libexi/libxoexi.so ]
  then
    cp xo/libexi/libxoexi.so apu/data/shlib/libxoexi-$XO_VERSION.so
    echo "cd \$ROOTACT/shlib" >> apu/control/postinst
    echo "ln -sf libxoexi-$XO_VERSION.so libxoexi.so" >> apu/control/postinst
  fi
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "xo-devel"
}

projectVersion()
{
  cat xo/Version
}

projectApuRev()
{
  cat xo/apu/revision
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

