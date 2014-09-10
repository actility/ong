
buildProject()
{
  cd xo
  ./MAKE clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control

  XO_VERSION=`projectVersion`

  cp -r xo apu/data
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "xo-src"
}

projectVersion()
{
  cat xo/Version
}

projectApuRev()
{
  cat xo/apu/revision
}

projectDescription()
{
  echo "Sources of xo - utility library to manipulate M2M and oBIX documents"
}

projectArchDepend()
{
  # no, no need of arch for building src apu
  return 1
}

