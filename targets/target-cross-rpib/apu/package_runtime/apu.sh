
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp target-cross-rpib/target.cfg apu/data/
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "target-cross-rpib"
}

projectVersion()
{
  cat target-cross-rpib/Version
}

projectApuRev()
{
  cat target-cross-rpib/apu/revision
}

projectDescription()
{
  echo "Target for Raspberry Pi Rev B"
}

