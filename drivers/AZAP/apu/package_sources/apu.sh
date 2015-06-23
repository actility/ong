
buildProject()
{
  cd AZAP && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r AZAP/ apu/data/
}

projectName()
{
  echo "AZAP-src"
}

projectDescription()
{
  echo "Sources of AZAP - the Actility library to drive the ZigBee stack on TI CC2531 dongle stick"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

