
buildProject()
{
  cd rtbase
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r rtbase/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "rtbase-src"
}

projectVersion()
{
  cat rtbase/Version
}

projectApuRev()
{
  cat rtbase/apu/revision
}

projectDescription()
{
  echo "Source files from base library for Actility M2M Drivers"
}

projectArchDepend()
{
  # no, no need for arch for src apu
  return 1
}

