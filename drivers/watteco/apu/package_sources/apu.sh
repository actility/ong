
buildProject()
{
  cd watteco && \
  make clean

  return $?
}

buildApuImage()
{

  WATTECO_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r watteco apu/data/
}

projectName()
{
  echo "watteco-src"
}

projectDescription()
{
  echo "Sources of Watteco M2M Driver"
}

projectArchDepend()
{
  # no, no need of arch for building src apu
  return 1
}

