
NAME=iec61131

buildProject()
{
  cd $NAME
  ./MAKE clean
  rm -rf .svn

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data
  mkdir -p apu/control/ && \
  cp -r $NAME/ apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo $NAME-src
}

projectVersion()
{
  cat $NAME/Version
}

projectApuRev()
{
  cat $NAME/apu/revision
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

