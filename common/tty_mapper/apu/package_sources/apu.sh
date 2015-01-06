
buildProject()
{
  cd tty_mapper/ && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data && \
  mkdir -p apu/control && \
  cp -r tty_mapper apu/data/
}

projectName()
{
  echo "tty_mapper-src"
}

projectArchDepend()
{
  # no, no need of arch for building src apu 
  return 1
}

