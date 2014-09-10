
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp target-cross-cov1/target.cfg apu/data/
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "target-cross-cov1"
}

projectVersion()
{
  cat target-cross-cov1/Version
}

projectApuRev()
{
  cat target-cross-cov1/apu/revision
}

projectDescription()
{
  echo "Target for Connected Object V1"
}

