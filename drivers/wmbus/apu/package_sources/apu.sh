
buildProject()
{
  cd wmbus
  rm -rf .svn
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r wmbus/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "wmbus-src"
}

projectVersion()
{
  cat wmbus/Version
}

projectApuRev()
{
  cat wmbus/apu/revision
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

