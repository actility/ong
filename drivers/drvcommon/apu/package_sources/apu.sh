
buildProject()
{
  cd drvcommon
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control

  DRVCOMMON_VERSION=`projectVersion`

  cp -r drvcommon apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "drvcommon-src"
}

projectVersion()
{
  cat drvcommon/Version
}

projectApuRev()
{
  cat drvcommon/apu/revision
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

