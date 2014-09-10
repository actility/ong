
buildProject()
{
  cd AZAP
  rm -rf .svn
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r AZAP/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "AZAP-src"
}

projectVersion()
{
  cat AZAP/Version
}

projectApuRev()
{
  cat AZAP/apu/revision
}

projectDescription()
{
  echo "Sources of AZAP - the Actility library to drive the ZigBee stack on TI CC2531 dongle stick"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

