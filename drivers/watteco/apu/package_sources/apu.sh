
buildProject()
{
  cd watteco
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control

  WATTECO_VERSION=`projectVersion`

  cp -r watteco apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "watteco-src"
}

projectVersion()
{
  cat watteco/Version
}

projectApuRev()
{
  cat watteco/apu/revision
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

