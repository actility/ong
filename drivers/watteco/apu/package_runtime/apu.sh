
buildProject()
{
  cd watteco
  ./MAKE clean
  ./MAKE

  return $?
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/watteco/supervisor/
  mkdir -p apu/data/watteco/snif/
  mkdir -p apu/data/watteco/tunslip/
  #mkdir -p apu/data/watteco/config/
  #mkdir -p apu/data/watteco/xoref/
  #mkdir -p apu/data/etc/init.d/
  #mkdir -p apu/data/usr/data/watteco/
  mkdir -p apu/data/watteco/shells/

  mkdir -p apu/data/etc/watteco/
  mkdir -p apu/data/bin/
  mkdir -p apu/data/etc/init.d/
  mkdir -p apu/data/usr/data/watteco/

  mkdir -p apu/control

  WATTECO_VERSION=`projectVersion`

#  cp watteco/config/modelconfig.xml apu/data/watteco/config/
#  cp watteco/config/spvconfig.xml apu/data/watteco/config/
#  cp -r watteco/diatemplates apu/data/usr/data/watteco/
  cp -r watteco/sensorconfig apu/data/usr/data/watteco/
  cp watteco/supervisor/awspv.x apu/data/watteco/supervisor/
  cp watteco/supervisor/awspv.sh apu/data/watteco/supervisor/
#  cp watteco/supervisor/watteco.xns apu/data/watteco/supervisor/
  cp watteco/tunslip/tunslip6.sh apu/data/watteco/tunslip/
  cp watteco/tunslip/tunslip6.x apu/data/watteco/tunslip/
#  cp watteco/xoref/*.xor apu/data/watteco/xoref/
#  echo "aaaa" > apu/data/usr/data/watteco/prefipv6.txt
#  cp $CURRENT/watteco-control apu/control/control
#  cp $CURRENT/watteco-postinst apu/control/postinst
#  cp watteco/etc/init.d/awspv apu/data/etc/init.d/
#  cp $CURRENT/tunslip-init apu/data/etc/init.d/tunslip
  cp watteco/shells/clean_all_before_start.sh apu/data/watteco/shells/
  [ -f watteco/snif/snifwatt.x ] && cp watteco/snif/snifwatt.x apu/data/watteco/snif/

  cp watteco/config/modelconfig.xml apu/data/etc/watteco/
  cp watteco/config/spvconfig.xml apu/data/etc/watteco/
  cp -r watteco/diatemplates apu/data/etc/watteco/
#  cp -r watteco/sensorconfig apu/data/etc/watteco/
#  cp watteco/supervisor/awspv.x apu/data/bin/
#  cp watteco/supervisor/awspv.sh apu/data/etc/watteco/
  cp watteco/supervisor/watteco.xns apu/data/etc/watteco/
#  cp watteco/tunslip/tunslip6.sh apu/data/etc/watteco/
#  cp watteco/tunslip/tunslip6.x apu/data/bin/
  cp watteco/xoref/*.xor apu/data/etc/watteco/
  echo "aaaa" > apu/data/usr/data/watteco/prefipv6.txt
  cp watteco/apu/$1/preinst apu/control/preinst
  cp watteco/apu/$1/postinst apu/control/postinst
  cp watteco/etc/init.d/awspv apu/data/etc/init.d/
  cp watteco/apu/$1/tunslip-init apu/data/etc/init.d/tunslip
#  cp watteco/shells/clean_all_before_start.sh apu/data/etc/watteco/
#  cp watteco/discover/discover.x apu/data/bin/

}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "watteco"
}

projectVersion()
{
  cat watteco/Version
}

projectApuRev()
{
  cat watteco/apu/revision
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

