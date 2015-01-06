
buildProject()
{
  cd drvcommon && \
  make clean

  return $?
}

buildApuImage()
{

  DRVCOMMON_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r drvcommon apu/data/
}

projectName()
{
  echo "drvcommon-src"
}

projectDescription()
{
  echo "Sources of drvcommon M2M Driver"
}

projectArchDepend()
{
  # no, no need of arch for building src apu
  return 1
}

