
buildProject()
{
  cd tty_mapper/tty_mapper && \
  make clean && \
  make

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/bin && \
  mkdir -p apu/control && \
  cp tty_mapper/tty_mapper/bin/tty_mapper apu/data/bin/ && \
  cp tty_mapper/apu/$1/postinst apu/control/
}

projectName()
{
  echo "tty_mapper"
}

projectArchDepend()
{
  # yes, need arch for building tty_mapper 
  return 0
}

