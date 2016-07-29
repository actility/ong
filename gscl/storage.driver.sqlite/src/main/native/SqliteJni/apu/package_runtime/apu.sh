
buildProject()
{
  cd SqliteJni

  NATIVE_SRC_DIR="../../../../../src/main/native/SqliteJni/Source"
  JNI_PACKAGE="com.actility.m2m.storage.driver.sqlite.impl"
  JNI_PACKAGE_PATH="com/actility/m2m/storage/driver/sqlite/impl"
  I_FILE="sqlitedriver.i"
  H_FILE="sqlite_driver.h"
  WRAP_FILE="sqlitedriver_wrap.c"
  GENERATED_OUTPUT_DIR="../../../../../src/main/generated/java"
  cd Source
  if [ ! -f $NATIVE_SRC_DIR/$WRAP_FILE -o -n "$([ -f $NATIVE_SRC_DIR/$WRAP_FILE ] && find $NATIVE_SRC_DIR/$I_FILE -newer $NATIVE_SRC_DIR/$WRAP_FILE)" -o -n "$([ -f $NATIVE_SRC_DIR/$WRAP_FILE ] && find $NATIVE_SRC_DIR/$H_FILE -newer $NATIVE_SRC_DIR/$WRAP_FILE)" ]; then
    mkdir -p $GENERATED_OUTPUT_DIR/$JNI_PACKAGE_PATH
    echo "swig -java -package $JNI_PACKAGE -outdir $GENERATED_OUTPUT_DIR/$JNI_PACKAGE_PATH $I_FILE";
    swig -java -package $JNI_PACKAGE -outdir $GENERATED_OUTPUT_DIR/$JNI_PACKAGE_PATH $I_FILE
    cp -f $WRAP_FILE $NATIVE_SRC_DIR
  fi
  cd ..

  # TODO: should use -O1 instead of -O3 for lpv3
  make ARCH=$1
}
