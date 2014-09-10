
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
  mkdir -p apu/data/lib
  mkdir -p apu/control

  XO_VERSION=`projectVersion`

  cp xo/lib/libxo.so apu/data/lib/libxo-$XO_VERSION.so
  echo "cd \$ROOTACT/lib" > apu/control/postinst
  echo "ln -sf libxo-$XO_VERSION.so libxo.so" >> apu/control/postinst

  if [ -f xo/libexi/libxoexi.so ]
  then
	  cp xo/libexi/libxoexi.so apu/data/lib/libxoexi-$XO_VERSION.so
	  echo "cd \$ROOTACT/lib" >> apu/control/postinst
	  echo "ln -sf libxoexi-$XO_VERSION.so libxoexi.so" >> apu/control/postinst
  fi
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "xo"
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
  echo "Utility library to manipulate M2M and oBIX documents"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

