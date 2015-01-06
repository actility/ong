
buildProject()
{
  cd wmbus && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  \
  cp -r wmbus/ apu/data/
}

projectName()
{
  echo "wmbus-src"
}

projectDescription()
{
  echo "Sources of the Wireless MBus M2M Driver"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

