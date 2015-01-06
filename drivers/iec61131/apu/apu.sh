
NAME=iec61131

projectGroupId()
{
  echo "com.actility.m2m"
}

projectVersion()
{
  cat $NAME/Version
}

projectApuRev()
{
  cat $NAME/apu/revision
}

