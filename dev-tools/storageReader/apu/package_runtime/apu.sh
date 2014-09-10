
buildProject()
{
  cd storageReader
  make clean all

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin/
  mkdir -p apu/control
  
  cp storageReader/sqliteReader/sqliteReader.x apu/data/bin/

  cp storageReader/apu/$1/postinst apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "storageReader"
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
  echo "storageReader"
}

projectArchDepend()
{
  # yes, need arch for building zigbee
  return 0
}

