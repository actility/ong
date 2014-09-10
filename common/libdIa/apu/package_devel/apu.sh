
buildProject()
{
  cd libdIa
  ./MAKE clean
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/libdIa/
  mkdir -p apu/control
  mkdir -p apu/data/shlib/

  DIA_VERSION=`cat libdIa/Version` 
 
  cp libdIa/*.h apu/data/libdIa/
  cp libdIa/*.a apu/data/libdIa/
  cp libdIa/*.so apu/data/libdIa/
  cp libdIa/libdIa.so apu/data/shlib/libdIa-$DIA_VERSION.so

  echo "cd \$ROOTACT/shlib" > apu/control/postinst
  echo "ln -f -s libdIa-$DIA_VERSION.so libdIa.so" >> apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "libdIa-devel"
}

projectVersion()
{
  cat libdIa/Version
}

projectApuRev()
{
  cat libdIa/apu/revision
}

projectDescription()
{
  echo "Headers and compiled binary for M2M dIa implementation"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

