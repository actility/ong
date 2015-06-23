
buildProject()
{
  cd trans_pty/TransPtyExe
  make clean TARGET=$1
  make TARGET=$1

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/bin
  mkdir -p apu/control

  cp trans_pty/TransPtyExe/bin/trans_pty apu/data/bin/
  cp trans_pty/apu/$1/postinst apu/control/
}

projectName()
{
  echo "trans_pty"
}

projectDescription()
{
  echo "Pseudo TTY"
}

projectArchDepend()
{
  # yes, need arch for building trans_pty 
  return 0
}

