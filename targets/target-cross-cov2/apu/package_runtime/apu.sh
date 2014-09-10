
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp target-cross-cov2/target.cfg apu/data/
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "target-cross-cov2"
}

projectVersion()
{
  cat target-cross-cov2/Version
}

projectApuRev()
{
  cat target-cross-cov2/apu/revision
}

projectDescription()
{
  echo "Target for Connected Object V2"
}

