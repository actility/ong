
buildProject()
{
  cd tty_mapper/
  rm -rf .svn
  cd tty_mapper
  make clean
  make clean TARGET=$1

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control

  cp -r tty_mapper apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "tty_mapper-src"
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
  echo "Source files of TTY mapper"
}

projectArchDepend()
{
  # no, no need of arch for building src apu 
  return 1
}

