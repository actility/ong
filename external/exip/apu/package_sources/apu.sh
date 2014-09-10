
buildProject()
{
  cd exip
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control

  cp -r exip apu/data
}

projectGroupId()
{
  echo "net.sourceforge"
}

projectName()
{
  echo "exip-src"
}

projectVersion()
{
  cat exip/Version
}

projectApuRev()
{
  cat exip/apu/revision
}

projectDescription()
{
  echo "Sources of exip - utility library to manipulate EXI documents"
}

