
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp target-cross-centos6-x86/target.cfg apu/data/
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "target-cross-centos6-x86"
}

projectVersion()
{
  cat target-cross-centos6-x86/Version
}

projectApuRev()
{
  cat target-cross-centos6-x86/apu/revision
}

projectDescription()
{
  echo "Target for centos6-x86 on centos6-x86_64"
}

