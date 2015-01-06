
buildProject()
{
  cd exip && \
  mkdir -p $ROOTACT/lib && \
  ./MAKE clean && \
  ./MAKE

  return $?
}

buildApuImage()
{
  EXIP_VERSION=$(projectVersion)

  rm -rf apu && \
  mkdir -p apu/data/lib && \
  mkdir -p apu/data/exip/src/grammar/include && \
  mkdir -p apu/data/exip/include && \
  mkdir -p apu/data/exip/build/gcc/pc && \
  mkdir -p apu/data/exip/src/stringTables/include && \
  mkdir -p apu/control && \
  cp $ROOTACT/exip/src/grammar/include/*.h apu/data/exip/src/grammar/include && \
  cp $ROOTACT/exip/include/*.h apu/data/exip/include && \
  cp $ROOTACT/exip/build/gcc/pc/*.h apu/data/exip/build/gcc/pc && \
  cp $ROOTACT/exip/src/stringTables/include/*.h apu/data/exip/src/stringTables/include && \
  cp $ROOTACT/lib/libexip.so.$EXIP_VERSION apu/data/lib/ && \
  echo "cd \$ROOTACT/lib" > apu/control/postinst && \
  echo "ln -sf libexip.so.$EXIP_VERSION libexip.so" >> apu/control/postinst
}

projectGroupId()
{
  echo "net.sourceforge"
}

projectName()
{
  echo "exip-devel"
}

projectVersion()
{
  cat exip/Version
}

projectApuRev()
{
  cat exip/apu/revision
}

projectDescription()
{
  echo "Utility library to manipulate EXI documents"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

