
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
  mkdir -p apu/data/m2mxoref/xoref/
  mkdir -p apu/control

  M2M_VERSION=`projectVersion`

  cp m2mxoref/xoref/m2m.xor apu/data/m2mxoref/xoref/
  cp m2mxoref/xoref/m2m.xdi apu/data/m2mxoref/xoref/
  cp m2mxoref/xoref/xobix.xor apu/data/m2mxoref/xoref/
  cp m2mxoref/xoref/namespace.xns apu/data/m2mxoref/xoref/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "m2mxoref-devel"
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
  echo "Xo descriptions for M2M XML documents and oBIX Driver documents (devel)"
}

projectArchDepend()
{
  # no, not for this project.
  return 1
}

