
buildProject()
{
  cd watteco && \
  make clean && \
  make

  return $?
}

buildApuImage()
{

  WATTECO_VERSION=`projectVersion`

  rm -rf apu && \
  mkdir -p apu/data/watteco/supervisor/ && \
  mkdir -p apu/data/watteco/snif/ && \
  mkdir -p apu/data/watteco/tunslip/ && \
  mkdir -p apu/data/watteco/shells/ && \
  mkdir -p apu/data/etc/watteco/ && \
  mkdir -p apu/data/bin/ && \
  mkdir -p apu/data/etc/init.d/ && \
  mkdir -p apu/data/usr/data/watteco/ && \
  mkdir -p apu/control && \
  cp -r watteco/sensorconfig apu/data/usr/data/watteco/ && \
  cp watteco/supervisor/bin/awspv.x apu/data/watteco/supervisor/ && \
  cp watteco/supervisor/awspv.sh apu/data/watteco/supervisor/ && \
  cp watteco/tunslip/tunslip6.sh apu/data/watteco/tunslip/ && \
  cp watteco/tunslip/bin/tunslip6.x apu/data/watteco/tunslip/ && \
  cp watteco/shells/clean_all_before_start.sh apu/data/watteco/shells/ && \
  ( ! [ -f watteco/snif/lib/snifwatt.x ] || \
    cp watteco/snif/lib/snifwatt.x apu/data/watteco/snif/ ) && \
  cp watteco/config/modelconfig.xml apu/data/etc/watteco/ && \
  cp watteco/config/spvconfig.xml apu/data/etc/watteco/ && \
  cp -r watteco/diatemplates apu/data/etc/watteco/ && \
  cp watteco/supervisor/watteco.xns apu/data/etc/watteco/ && \
  cp watteco/xoref/*.xor apu/data/etc/watteco/ && \
  echo "aaaa" > apu/data/usr/data/watteco/prefipv6.txt && \
  cp watteco/apu/$1/preinst apu/control/preinst && \
  cp watteco/apu/$1/postinst apu/control/postinst && \
  cp watteco/etc/init.d/awspv apu/data/etc/init.d/ && \
  cp watteco/apu/$1/tunslip-init apu/data/etc/init.d/tunslip

}

projectName()
{
  echo "watteco"
}

projectDescription()
{
  echo "Watteco M2M Driver"
}

projectArchDepend()
{
  # yes, need arch for building zigbee
  return 0
}

