
buildProject()
{
  export PATH=$PATH:$(pwd)/bin
  cd modbus
  rm -rf .svn
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r modbus/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "modbus-src"
}

projectVersion()
{
  cat modbus/Version
}

projectApuRev()
{
  cat modbus/apu/revision
}

projectDescription()
{
  echo "Sources of the ModBus M2M Driver"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

