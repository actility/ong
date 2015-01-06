
buildProject()
{
  cd knx && \
  make clean && \
  make

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
  mkdir -p apu/data/etc/knx/diatemplates && \
  mkdir -p apu/data/bin/ && \
  mkdir -p apu/data/etc/xo/ && \
  mkdir -p apu/data/etc/init.d/ && \
  mkdir -p apu/data/usr/data/knx/ && \
  mkdir -p apu/control && \
  cp knx/config/dpt.xml apu/data/usr/data/knx/dpt.xml && \
  cp knx/config/manufacturer.xml apu/data/usr/data/knx/manufacturer.xml && \
  cp knx/config/ranges.xml apu/data/usr/data/knx/ranges.xml && \
  cp knx/config/modelconfig.xml apu/data/etc/knx/ && \
  cp knx/config/spvconfig.xml apu/data/etc/knx/ && \
  copyTemplate knx/diatemplates/ $ROOTACT/apu/data/etc/knx/diatemplates/ && \
  cp knx/supervisor/bin/knx.x apu/data/bin/ && \
  cp knx/xoref/knx.xns apu/data/etc/knx/ && \
  cp knx/xoref/*.xor apu/data/etc/knx/ && \
  cp knx/apu/$1/postinst apu/control/postinst && \
  cp knx/knx-init apu/data/etc/init.d/knx
}

projectName()
{
  echo "knx"
}

projectDescription()
{
  echo "KNX M2M Driver"
}

projectArchDepend()
{
  # yes, need arch for building knx
  return 0
}

