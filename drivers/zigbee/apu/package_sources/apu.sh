
projectName()
{
  echo "zigbee-src"
}

buildProject()
{
  cd zigbee && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r zigbee/ apu/data/
}

projectDescription()
{
  echo "Sources of the ZigBee M2M Driver"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

