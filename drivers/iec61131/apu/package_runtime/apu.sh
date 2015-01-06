
NAME=iec61131

buildProject()
{
  OPTIONS=
  if [ $1 == "centos6-x86" -o $1 == "centos6-x86_64" ]
  then
    OPTIONS="UNARY_TEST=on"
  fi

  cd $NAME && \
  make clean && \
  make $OPTIONS

  return $?
}

buildApuImage()
{
  rm -rf apu && \
  mkdir -p apu/data/etc/$NAME && \
  mkdir -p apu/data/bin/ && \
  mkdir -p apu/data/etc/init.d/ && \
  mkdir -p apu/control/ && \
  \
  tar cvf /tmp/__temp__.$$ --exclude=.svn $NAME/files && \
  pushd . && \
  cd apu/data/etc && \
  tar xf /tmp/__temp__.$$ && \
  rm /tmp/__temp__.$$ && \
  popd && \
  \
  cp $NAME/bin/bin/plc.x apu/data/bin/ && \
  cp $NAME/config/spvconfig.std.xml apu/data/etc/$NAME/spvconfig.xml && \
  cp $NAME/$NAME-init apu/data/etc/init.d/$NAME && \
  cp $NAME/apu/$1/postinst apu/control/postinst
}

projectName()
{
  echo $NAME
}

projectDescription()
{
  echo "IEC6131 PLC Engine"
}

projectArchDepend()
{
  # yes, need arch for building $NAME
  return 0
}

