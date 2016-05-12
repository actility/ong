
buildProject()
{
  cd stdin-logger/
  make clean TARGET=$1
  make TARGET=$1

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin
  mkdir -p apu/control

  cp stdin-logger/bin/stdin-logger apu/data/bin/
  cp stdin-logger/apu/$1/postinst apu/control/
}

projectDescription()
{
  echo "Utility binary to redirect stdout outputs to rotative log files"
}

projectArchDepend()
{
  # yes, need arch for building stdin-logger 
  return 0
}

