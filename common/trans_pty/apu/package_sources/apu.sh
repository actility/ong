
buildProject()
{
  cd trans_pty/
  rm -rf .svn
  cd TransPtyExe
  make clean TARGET=$1
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control

  cp -r trans_pty apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "trans_pty-src"
}

projectVersion()
{
  cat trans_pty/Version
}

projectApuRev()
{
  cat trans_pty/apu/revision
}

projectDescription()
{
  echo "Source files of pseudo TTY"
}

projectArchDepend()
{
  # no, no need of arch for building src apu 
  return 1
}

