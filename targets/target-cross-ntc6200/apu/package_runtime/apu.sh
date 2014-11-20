
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp target-cross-ntc6200/target.cfg apu/data/
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "target-cross-ntc6200"
}

projectVersion()
{
  cat target-cross-ntc6200/Version
}

projectApuRev()
{
  cat target-cross-ntc6200/apu/revision
}

projectDescription()
{
  echo "Target for NTC-6200"
}

