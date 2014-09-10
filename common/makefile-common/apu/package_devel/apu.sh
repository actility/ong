
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp makefile-common/Makefile.common.* apu/data/
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "makefile-common-devel"
}

projectVersion()
{
  cat makefile-common/Version
}

projectApuRev()
{
  cat makefile-common/apu/revision
}

projectDescription()
{
  echo "Common makefiles to build native projects"
}

