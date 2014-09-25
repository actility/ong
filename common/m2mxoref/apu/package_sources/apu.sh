
buildProject()
{
  cd m2mxoref
  if [[ $1 == $HOST_TARGET ]]
  then
    ./MAKE clean

    return $?
  fi

  echo "*** No cross-compilation for m2mxoref !!" >&2
  return 1
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data/
  mkdir -p apu/control

  cp -r m2mxoref apu/data/
}

projectGroupId()
{
  echo "com.actility"
}

projectName()
{
  echo "m2mxoref-src"
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
  echo "Sources of m2mxoref - Xo descriptions for M2M XML documents and oBIX Driver documents"
}

projectArchDepend()
{
  # no, not for this project.
  return 1
}

