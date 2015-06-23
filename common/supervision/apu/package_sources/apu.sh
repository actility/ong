
buildProject()
{
  cd supervision
  rm -rf .svn
  return 0
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control

  cp -r supervision apu/data/
}

projectApuRev()
{
  cat supervision/apu/revision
}

projectNameSuffix()
{
  echo "src"
}

projectDescription()
{
  echo "Source files of supervision - Supervision script which monitors running services"
}

