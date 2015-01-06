
buildProject()
{
  cd libdIa && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r libdIa/ apu/data/
}

projectName()
{
  echo "libdIa-src"
}

projectDescription()
{
  echo "Source files for M2M dIa implementation"
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

