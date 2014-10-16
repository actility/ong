
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/etc && \
  mkdir -p apu/control && \
  cp acy-init/etc/acy apu/data/etc/ && \
  cp acy-init/etc/acy.cfg apu/data/etc/ && \
  cp acy-init/etc/pids apu/data/etc/ && \
  cp acy-init/etc/functions apu/data/etc/ && \
  cp acy-init/apu/$1/postinst apu/control/postinst
  
}

projectGroupId()
{
  echo "com.actility.m2m"
}

projectName()
{
  echo "acy-init"
}

projectVersion()
{
  cat acy-init/Version
}

projectApuRev()
{
  cat acy-init/apu/revision
}

projectDescription()
{
  echo "Actility's generic init system projects"
}

