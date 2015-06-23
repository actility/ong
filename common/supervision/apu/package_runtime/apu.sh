
buildProject()
{
  return 0
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data && \
  mkdir -p apu/control && \
  cp -r supervision/apu/control/* apu/control/ && \
  cp -r supervision/apu/$1/preinst apu/control/preinst && \
  cp -r supervision/apu/$1/postinst apu/control/postinst && \
  cp -r supervision/etc apu/data/
}

projectDescription()
{
  echo "Supervision script which monitors running services"
}

