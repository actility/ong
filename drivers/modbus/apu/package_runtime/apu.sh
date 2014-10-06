
buildProject()
{
  export PATH=$PATH:$(pwd)/bin
  cd modbus && \
  make clean && \
  make && \
  if [[ $1 == centos6-x86 ]] 
  then
    cd tests && \
    make run
  fi

  return $?
}

copyTemplate()
{
  _SRC_DIR=$1
  _TARGET_DIR=$2
  pushd $(pwd)
  cd $_SRC_DIR
  echo "changing to folder $(pwd)"
  for file in $(ls -1 *.xml)
  do
    cat $file | \
      sed -e "s| is=\".*\"||" | \
      sed -e "s| in=\".*\"||" | \
      sed -e "s| out=\".*\"||" > $_TARGET_DIR/$file
  done
  popd
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/etc/modbus/diatemplates && \
  mkdir -p apu/data/bin/ && \
  mkdir -p apu/data/etc/xo/ && \
  mkdir -p apu/data/etc/init.d/ && \
  mkdir -p apu/data/usr/data/modbus/ && \
  mkdir -p apu/control && \
  cp modbus/config/modelconfig.xml apu/data/etc/modbus/ && \
  cp modbus/config/spvconfig.xml apu/data/etc/modbus/ && \
  copyTemplate modbus/diatemplates/ $ROOTACT/apu/data/etc/modbus/diatemplates/ && \
  cp modbus/supervisor/modbus.x apu/data/bin/ && \
  cp modbus/xoref/modbus.xns apu/data/etc/modbus/ && \
  cp modbus/xoref/*.xor apu/data/etc/modbus/ && \
  cp modbus/apu/$1/postinst apu/control/postinst && \
  cp modbus/modbus-init apu/data/etc/init.d/modbus
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "modbus"
}

projectVersion()
{
  cat modbus/Version
}

projectApuRev()
{
  cat modbus/apu/revision
}

projectDescription()
{
  echo "ModBus M2M Driver"
}

projectArchDepend()
{
  # yes, need arch for building modbus
  return 0
}

