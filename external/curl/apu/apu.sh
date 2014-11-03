
projectGroupId()
{
  echo "se.haxx"
}

projectVersion()
{
  cat curl/Version
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


