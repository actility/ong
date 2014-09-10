
buildProject()
{
  cd exip
  ./MAKE clean
  ./MAKE

  cd ../xo
  ./MAKE clean
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/xo/com/
  mkdir -p apu/control

  XO_VERSION=`projectVersion`

  cp xo/com/*.x apu/data/xo/com/
  cp xo/com/*.sh apu/data/xo/com/
  echo "chmod +x \$ROOTACT/xo/com/*.sh" >> apu/control/postinst
  echo "chmod +x \$ROOTACT/xo/com/*.x" >> apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "xo-build"
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
  echo "Utils for library to manipulate M2M and oBIX documents"
}

projectArchDepend()
{
  # yes, need arch for building build apu
  return 0
}

