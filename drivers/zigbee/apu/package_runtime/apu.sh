
projectName()
{
  echo "zigbee"
}

buildProject()
{
  cd zigbee && \
  ./MAKE clean && \
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/etc/zigbee/ && \
  mkdir -p apu/data/bin/ && \
  mkdir -p apu/data/etc/xo/ && \
  mkdir -p apu/data/etc/init.d/ && \
  mkdir -p apu/control && \
  cp zigbee/config/modelconfig.xml apu/data/etc/zigbee/ && \
  cp zigbee/config/spvconfig.xml apu/data/etc/zigbee/ && \
  cp -r zigbee/diatemplates apu/data/etc/zigbee/ && \
  cp zigbee/driver/azbspv.x apu/data/bin/ && \
  cp zigbee/xoref/zigbee.xns apu/data/etc/zigbee/ && \
  cp zigbee/xoref/*.xor apu/data/etc/zigbee/ && \
  cp zigbee/apu/$1/postinst apu/control/postinst && \
  cp zigbee/zigbee-init apu/data/etc/init.d/zigbee
}

projectDescription()
{
  echo "ZigBee M2M Driver"
}

projectArchDepend()
{
  # yes, need arch for building zigbee
  return 0
}

