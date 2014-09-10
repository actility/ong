
buildProject()
{

  JAVA_ROOT_DIR=$(cd ../../.. && pwd)
  JAVA_SRC_DIR=$JAVA_ROOT_DIR"/src/main/java/"
  TARGET_CLASSES_DIR=$JAVA_ROOT_DIR"/target/classes/"
  TARGET_DEPEND_DIR=$JAVA_ROOT_DIR"/target/dependencies"
  NATIVE_SRC_DIR=$JAVA_ROOT_DIR"/src/main/native/HttpClientJni/Source"
  DEPENDENCIES=$(ls -1 ${TARGET_DEPEND_DIR})

  echo "***** pwd=$(pwd)"
  echo "***** JAVA_ROOT_DIR=$JAVA_ROOT_DIR"
  echo "***** JAVA_SRC_DIR=$JAVA_SRC_DIR"
  echo "***** TARGET_CLASSES_DIR=$TARGET_CLASSES_DIR"
  echo "***** TARGET_DEPEND_DIR=$TARGET_DEPEND_DIR"
  echo "***** NATIVE_SRC_DIR=$NATIVE_SRC_DIR"
  echo "***** DEPENDENCIES=$DEPENDENCIES"

  CLASSPATH_DEPEND=
  for dependency in ${DEPENDENCIES}
  do
    CLASSPATH_DEPEND=$CLASSPATH_DEPEND:$TARGET_DEPEND_DIR/$dependency
  done

  JNI_CLASS="com.actility.m2m.song.binding.http.jni.impl.JHttpClients"
  JNI_CLASS_FILE="com/actility/m2m/song/binding/http/jni/impl/JHttpClients.java"
  H_FILE="com_actility_m2m_song_binding_http_jni_impl_JHttpClients.h"
  cd HttpClientJni/Source
  if [ ! -f $NATIVE_SRC_DIR/$H_FILE -o -n "$([ -f $NATIVE_SRC_DIR/$H_FILE ] && find $JAVA_SRC_DIR/$JNI_CLASS_FILE -newer $NATIVE_SRC_DIR/$H_FILE)" ]; then
    echo "javah -classpath $TARGET_CLASSES_DIR:$CLASSPATH_DEPEND $JNI_CLASS";
    javah -classpath $TARGET_CLASSES_DIR:$CLASSPATH_DEPEND $JNI_CLASS;
    if [ -f $H_FILE ]; then
      touch $H_FILE;
      cp -f $H_FILE $NATIVE_SRC_DIR
    fi
  fi
  cd ..

  make ARCH=$1
}
