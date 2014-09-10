
buildProject()
{
  CURRENT=$(pwd)
  JAVA_ROOT_DIR=$(cd ../../../../../../.. && pwd)
  NATIVE_LIB_SRC_DIR=$JAVA_ROOT_DIR"/src/main/native/HttpClientJni"

  echo "***** pwd=$CURRENT"
  echo "***** JAVA_ROOT_DIR=$JAVA_ROOT_DIR"
  echo "***** NATIVE_LIB_SRC_DIR=$NATIVE_LIB_SRC_DIR"
  echo "***** Copying $NATIVE_LIB_SRC_DIR"
  rsync -av --progress $NATIVE_LIB_SRC_DIR . --exclude .svn --exclude .git 

  echo "***** Compiling HttpClientJni"
  cd HttpClientJni/ && \
  CFLAGS=$CFLAGS" -D_NATIVE_TESTER_ " make ARCH=$1 BIN_TARGET=$CURRENT/lib && \
  echo "***** Compiling MultiThreadTesterHttpClient" && \
  cd ../MultiThreadTesterHttpClient/ && \
  make ARCH=$1
}
