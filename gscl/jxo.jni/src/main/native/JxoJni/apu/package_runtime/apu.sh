
buildProject()
{
  cd JxoJni

  JAVA_SRC_DIR="../../../../../src/main/java/"
  TARGET_CLASSES_DIR="../../../../../target/classes/"
  NATIVE_SRC_DIR="../../../../../src/main/native/JxoJni/Source"
  JNI_CLASS="com.actility.m2m.xo.jni.JniXo"
  JNI_CLASS_FILE="com/actility/m2m/xo/jni/JniXo.java"
  H_FILE="com_actility_m2m_xo_jni_JniXo.h"
  cd Source
  if [ ! -f $NATIVE_SRC_DIR/$H_FILE -o -n "$([ -f $NATIVE_SRC_DIR/$H_FILE ] && find $JAVA_SRC_DIR/$JNI_CLASS_FILE -newer $NATIVE_SRC_DIR/$H_FILE)" ]; then
    echo "javah -classpath $TARGET_CLASSES_DIR $JNI_CLASS";
    javah -classpath $TARGET_CLASSES_DIR $JNI_CLASS;
    touch $H_FILE;
    cp -f $H_FILE $NATIVE_SRC_DIR
  fi
  cd ..

  make ARCH=$1
}