
projectGroupId()
{
  echo "org.gnu"
}

projectVersion()
{
  echo "0.9.33"
}

projectApuRev()
{
  cat libmicrohttpd/apu/revision
}

projectArchDepend()
{
  # yes, need arch for building apu
  return 0
}


