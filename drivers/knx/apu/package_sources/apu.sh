
buildProject()
{
  cd knx && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  \
  cp -r knx/ apu/data/
}

projectName()
{
  echo "knx-src"
}

projectDescription()
{
  echo "Sources of the KNX M2M Driver"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

