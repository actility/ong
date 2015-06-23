
buildProject()
{
  cd exip

  mkdir -p $ROOTACT/lib

  ./MAKE clean
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/lib
  mkdir -p apu/control

  EXIP_VERSION=$(projectVersion)

  cp $ROOTACT/lib/libexip.so.$EXIP_VERSION apu/data/lib/
  
  echo "cd \$ROOTACT/lib" > apu/control/postinst
  echo "ln -sf libexip.so.$EXIP_VERSION libexip.so" >> apu/control/postinst
}

projectGroupId()
{
  echo "net.sourceforge"
}

projectName()
{
  echo "exip"
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

