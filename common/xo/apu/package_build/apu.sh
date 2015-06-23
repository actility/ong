
buildProject()
{
  cd xo && \
  make clean && \
  make

  return $?
}

buildApuImage()
{
  XO_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data/xo/com/ && \
  mkdir -p apu/control && \
  cp xo/com/bin/*.x apu/data/xo/com/ && \
  cp xo/com/*.sh apu/data/xo/com/ && \
  echo "chmod +x \$ROOTACT/xo/com/*.sh" >> apu/control/postinst && \
  echo "chmod +x \$ROOTACT/xo/com/*.x" >> apu/control/postinst
}

projectName()
{
  echo "xo-build"
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

