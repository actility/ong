
buildProject()
{
  cd knx
  rm -rf .svn
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r knx/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "knx-src"
}

projectVersion()
{
  cat knx/Version
}

projectApuRev()
{
  cat knx/apu/revision
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

