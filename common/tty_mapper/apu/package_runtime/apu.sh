
buildProject()
{
  cd tty_mapper/tty_mapper
  make clean TARGET=$1
  make TARGET=$1

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin
  mkdir -p apu/control

  cp tty_mapper/tty_mapper/bin/tty_mapper apu/data/bin/
  cp tty_mapper/apu/$1/postinst apu/control/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "tty_mapper"
}

projectVersion()
{
  cat tty_mapper/Version
}

projectApuRev()
{
  cat tty_mapper/apu/revision
}

projectDescription()
{
  echo "TTY mapper"
}

projectArchDepend()
{
  # yes, need arch for building tty_mapper 
  return 0
}

