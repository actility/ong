
buildProject()
{
  cd storageReader
  make clean && \
  make

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

projectName()
{
  echo "storageReader"
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

