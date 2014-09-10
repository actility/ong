
buildProject()
{
  :
}

buildApuImage()
{
  rm -rf apu
  mkdir -p apu/data
  mkdir -p apu/control
  
  cp -r jni/include apu/data/
}

projectGroupId()
{
  echo "com.oracle"
}

projectName()
{
  echo "jni-devel"
}

projectVersion()
{
  cat jni/Version
}

projectApuRev()
{
  cat jni/apu/revision
}

projectDescription()
{
  echo "JNI headers for native compilation with Java"
}

