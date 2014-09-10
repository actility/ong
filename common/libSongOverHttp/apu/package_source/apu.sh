
projectName()
{
  echo "libSongOverHttp-src"
}

buildProject()
{
  cd libSongOverHttp && \
  make clean

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/ && \
  mkdir -p apu/control && \
  cp -r libSongOverHttp/ apu/data
}

projectArchDepend()
{
  # no, no need of arch for sources apu
  return 1
}

