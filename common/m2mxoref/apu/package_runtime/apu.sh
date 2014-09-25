
buildProject()
{
  cd m2mxoref
  if [[ $1 == $HOST_TARGET ]]
  then
    ./MAKE clean
    ./MAKE

    return $?
  fi

  echo "*** No cross-compilation for m2mxoref !!" >&2
  return 1
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/etc/xo
  mkdir -p apu/control

  M2M_VERSION=`projectVersion`

  cp m2mxoref/xoref/m2m.xor apu/data/etc/xo/
  cp m2mxoref/xoref/m2m.xdi apu/data/etc/xo/
  cp m2mxoref/xoref/xobix.xor apu/data/etc/xo/
  cp m2mxoref/xoref/namespace.xns apu/data/etc/xo/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "m2mxoref"
}

projectVersion()
{
  cat m2mxoref/Version
}

projectApuRev()
{
  cat m2mxoref/apu/revision
}

projectDescription()
{
  echo "Xo descriptions for M2M XML documents and oBIX Driver documents"
}

projectArchDepend()
{
  # no, not for this project.
  return 1
}

