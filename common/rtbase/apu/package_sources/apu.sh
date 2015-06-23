
buildProject()
{
  cd rtbase && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r rtbase/ apu/data/
}

projectName()
{
  echo "rtbase-src"
}

projectDescription()
{
  echo "Source files from base library for Actility M2M Drivers"
}

projectArchDepend()
{
  # no, no need for arch for src apu
  return 1
}

