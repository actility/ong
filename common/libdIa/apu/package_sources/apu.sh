
buildProject()
{
  cd libdIa
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control
  
  cp -r libdIa/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "libdIa-src"
}

projectVersion()
{
  cat libdIa/Version
}

projectApuRev()
{
  cat libdIa/apu/revision
}

projectDescription()
{
  echo "Source files for M2M dIa implementation"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

