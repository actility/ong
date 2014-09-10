
projectName()
{
  echo "libHttpSubsMgmt-src"
}

buildProject()
{
  cd libHttpSubsMgmt && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data && \
  mkdir -p apu/control && \
  cp -r libHttpSubsMgmt/ apu/data
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

