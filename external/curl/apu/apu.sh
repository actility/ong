
projectGroupId()
{
  echo "se.haxx"
}

projectVersion()
{
  echo "7.36.0"
}

projectApuRev()
{
  cat curl/apu/revision
}

projectArchDepend()
{
  # yes, need arch for building apu
  return 0
}


