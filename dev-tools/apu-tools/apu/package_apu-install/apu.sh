
buildProject()
{
  echo "Nothing to do"
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/bin && \
  mkdir -p apu/control && \
  cp apu-tools/apu-install-stdalone.sh apu/data/bin/apu-install && \
  cp apu-tools/apu/$1/postinst apu/control/postinst
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "apu-install"
}

projectVersion()
{
  grep "^__VERSION" apu-tools/apu-install-stdalone.sh | cut -d= -f2
}

projectApuRev()
{
  cat apu-tools/apu/package_apu-install/revision
}

projectDescription()
{
  echo "Tool for installing Actility Package Unit (APU) files"
}

projectArchDepend()
{
  # no, not for this project.
  return 1
}

