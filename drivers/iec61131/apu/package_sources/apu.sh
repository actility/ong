
NAME=iec61131

buildProject()
{
  cd $NAME && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data && \
  mkdir -p apu/control/ && \
  cp -r $NAME/ apu/data/
}

projectName()
{
  echo $NAME-src
}

projectDescription()
{
  echo "Sources of IEC6131 PLC Engine"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

