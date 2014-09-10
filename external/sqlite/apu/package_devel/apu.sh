
buildProject()
{
  cd sqlite
  tar zxvf sqlite-autoconf-3080301.tar.gz
  cd sqlite-autoconf-3080301
  if [ ! -f Makefile ]; then
    export LDFLAGS=""
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin
  mkdir -p apu/data/lib
  mkdir -p apu/data/include
  mkdir -p apu/data/share
  mkdir -p apu/control
 
  cp -ar bin lib include share apu/data/
}

projectGroupId()
{
  echo "org.sqlite"
}

projectName()
{
  echo "sqlite-devel"
}

projectVersion()
{
  cat sqlite/Version
}

projectApuRev()
{
  cat sqlite/apu/revision
}

projectDescription()
{
  echo "Headers from sqlite"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

