
projectGroupId()
{
  echo "net.sourceforge"
}

projectVersion()
{
  echo "4.6"
}

projectApuRev()
{
  cat cproto/apu/revision
}

projectArchDepend()
{
  # yes, need arch for building apu
  return 0
}


