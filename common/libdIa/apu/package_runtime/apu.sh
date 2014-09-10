
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
  mkdir -p apu/data/lib
  mkdir -p apu/control
  DIA_VERSION=`cat libdIa/Version`

  cp libdIa/libdIa.so apu/data/lib/libdIa-$DIA_VERSION.so
  echo "cd \$ROOTACT/lib" > apu/control/postinst
  echo "ln -f -s libdIa-$DIA_VERSION.so libdIa.so" >> apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "libdIa"
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
  echo "M2M dIa implementation"
}

projectArchDepend()
{
  # yes, need arch for building libdIa 
  return 0
}

