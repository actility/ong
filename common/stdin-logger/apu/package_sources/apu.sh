
buildProject()
{
  cd stdin-logger/
  rm -rf .svn
  make clean TARGET=$1
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control

  cp -r stdin-logger apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "stdin-logger-src"
}

projectVersion()
{
  cat stdin-logger/Version
}

projectApuRev()
{
  cat stdin-logger/apu/revision
}

projectDescription()
{
  echo "Source files of stdin-logger - Utility binary to redirect stdout outputs to rotative log files"
}

projectArchDepend()
{
  # no, no need of arch for building src apu 
  return 1
}

