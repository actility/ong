
projectGroupId()
{
  echo "org.xmlsoft"
}

projectName()
{
  echo "libxml2"
}

projectVersion()
{
  echo "2.7.8"
}

projectApuRev()
{
  cat libxml2/apu/revision
}

projectApuMinVersion()
{
  echo "2.0.0"
}

projectArchDepend()
{
  # yes, need arch for building apu
  return 0
}

