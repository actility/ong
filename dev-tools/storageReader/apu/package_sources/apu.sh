
buildProject()
{
  cd storageReader
  rm -rf .svn
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r storageReader/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "storageReader-src"
}

projectVersion()
{
  cat storageReader/Version
}

projectApuRev()
{
  cat storageReader/apu/revision
}

projectDescription()
{
  echo "Sources of the tool storageReader"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

