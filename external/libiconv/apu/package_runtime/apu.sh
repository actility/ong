
buildProject()
{
  ROOTACT=`pwd` && \
  cd libiconv && \
  make TARGET=$1 && \
  make TARGET=$1 install

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  ICONV_VERSION=`projectVersion` \
  cp -r lib apu/data/
}

projectGroupId()
{
  echo "org.gnu"
}

projectName()
{
  echo "libiconv"
}

projectVersion()
{
  echo "1.11"
}

projectApuRev()
{
  cat libiconv/apu/revision
}

projectDescription()
{
  echo "GNU iconv library"
}

projectArchDepend()
{
  # yes, need arch for building runtime apu
  return 0
}


