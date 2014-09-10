
buildProject()
{
  cd sqlite
  tar zxvf sqlite-autoconf-3080301.tar.gz
  cd sqlite-autoconf-3080301
  if [ ! -f Makefile ]; then
    LDFLAGS=""
    ./configure $CONFIGURE_FLAGS --prefix $ROOTACT
  fi
  make
  make install

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/lib
  mkdir -p apu/control

  cp lib/libsqlite3.so.0.8.6 apu/data/lib/
  echo "cd \$ROOTACT/lib" > apu/control/postinst
  echo "ln -s libsqlite3.so.0.8.6 libsqlite3.so" >> apu/control/postinst
  echo "ln -s libsqlite3.so.0.8.6 libsqlite3.so.0" >> apu/control/postinst
}

projectGroupId()
{
  echo "org.sqlite"
}

projectName()
{
  echo "sqlite"
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
  echo "SQL database with small footprint"
}

projectArchDepend()
{
  # yes, need arch for building devel apu
  return 0
}

