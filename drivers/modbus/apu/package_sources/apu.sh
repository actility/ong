
buildProject()
{
  export PATH=$PATH:$(pwd)/bin
  cd modbus
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  \
  cp -r modbus/ apu/data/
}

projectName()
{
  echo "modbus-src"
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

