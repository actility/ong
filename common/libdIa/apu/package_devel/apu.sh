
buildProject()
{
  cd libdIa && \
  make clean && \
  make

  return $?
}

buildApuImage()
{
  DIA_VERSION=`cat libdIa/Version` 
  rm -rf apu && \
  mkdir -p apu/data/libdIa/ && \
  mkdir -p apu/control && \
  mkdir -p apu/data/lib/ && \
  cp libdIa/*.h apu/data/libdIa/ && \
  cp libdIa/lib/*.a apu/data/libdIa/ && \
  cp libdIa/lib/libdIa.so apu/data/lib/libdIa-$DIA_VERSION.so && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -f -s libdIa-$DIA_VERSION.so libdIa.so" >> apu/control/postinst
}

projectName()
{
  echo "libdIa-devel"
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

