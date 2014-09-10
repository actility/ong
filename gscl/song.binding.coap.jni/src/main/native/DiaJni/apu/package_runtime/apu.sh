
buildProject()
{
  cd DiaJni

  JAVA_SRC_DIR="../../../../../src/main/java/"
  TARGET_CLASSES_DIR="../../../../../target/classes/"
  NATIVE_SRC_DIR="../../../../../src/main/native/DiaJni/Source"
  JNI_CLASS="com.actility.m2m.song.binding.coap.jni.impl.DiaJni"
  JNI_CLASS_FILE="com/actility/m2m/song/binding/coap/jni/impl/DiaJni.java"
  H_FILE="com_actility_m2m_song_binding_coap_jni_impl_DiaJni.h"
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