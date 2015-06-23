
buildProject()
{
  cd stdin-logger/ && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data && \
  mkdir -p apu/control && \
  cp -r stdin-logger apu/data/
}

projectName()
{
  echo "stdin-logger-src"
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

