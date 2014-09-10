
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp target-centos6-x86/target.cfg apu/data/
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "target-centos6-x86"
}

projectVersion()
{
  cat target-centos6-x86/Version
}

projectApuRev()
{
  cat target-centos6-x86/apu/revision
}

projectDescription()
{
  echo "Target for centos6-x86"
}

