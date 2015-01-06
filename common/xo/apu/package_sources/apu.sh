
buildProject()
{
  cd xo && \
  make clean

  return $?
}

buildApuImage()
{

  XO_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data && \
  mkdir -p apu/control && \
  cp -r xo apu/data
}

projectName()
{
  echo "xo-src"
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

