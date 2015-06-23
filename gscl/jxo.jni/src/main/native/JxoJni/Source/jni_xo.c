#include <jni.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "com_actility_m2m_xo_jni_JniXo.h"
#include "xoapipr.h"

int initialized = 0;
jclass xoExceptionClass = NULL;
jclass xoParseExceptionClass = NULL;
jclass xoUnknownTypeExceptionClass = NULL;
jclass illegalStateExceptionClass = NULL;

const char MSG_NO_MORE_MEMORY[] = "No more memory";

void throwXoException(JNIEnv* env, size_t size, const char* format, ...)
{
  va_list listArg;
  char* message = malloc(sizeof(char) * size);
  if (message != NULL)
  {
    va_start(listArg, format);
    vsnprintf(message, size, format, listArg);
    va_end(listArg);
    message[size - 1] = '\0';
    (*env)->ThrowNew(env, xoExceptionClass, message);
    free(message);
  }
  else
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
  }
}

void throwXoParseException(JNIEnv* env, size_t size, const char* format, ...)
{
  va_list listArg;
  char* message = malloc(sizeof(char) * size);
  if (message != NULL)
  {
    va_start(listArg, format);
    vsnprintf(message, size, format, listArg);
    va_end(listArg);
    message[size - 1] = '\0';
    (*env)->ThrowNew(env, xoParseExceptionClass, message);
    free(message);
  }
  else
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
  }
}

void throwXoUnknownTypeException(JNIEnv* env, size_t size, const char* format, ...)
{
  va_list listArg;
  char* message = malloc(sizeof(char) * size);
  if (message != NULL)
  {
    va_start(listArg, format);
    vsnprintf(message, size, format, listArg);
    va_end(listArg);
    message[size - 1] = '\0';
    (*env)->ThrowNew(env, xoUnknownTypeExceptionClass, message);
    free(message);
  }
  else
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
  }
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_init(JNIEnv * env, jclass class)
{
  XoInit(0);
  XoVersion();

  return (jint) sizeof(void*);
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_start(JNIEnv * env, jclass class, jstring xoNamespacesFile,
    jstring xoRefFile, jstring dictFile)
{
  jboolean jresult = JNI_TRUE;

  xoExceptionClass = (*env)->FindClass(env, "com/actility/m2m/xo/XoException");
  if (xoExceptionClass != NULL)
  {
    xoExceptionClass = (*env)->NewGlobalRef(env, xoExceptionClass);
    if (xoExceptionClass == NULL)
    {
      jresult = JNI_FALSE;
      printf("Cannot get a global reference on com.actility.m2m.xo.XoException");
    }
  }
  else
  {
    jresult = JNI_FALSE;
    printf("Cannot get a reference on com.actility.m2m.xo.XoException");
  }
  xoParseExceptionClass = (*env)->FindClass(env, "com/actility/m2m/xo/XoParseException");
  if (xoParseExceptionClass != NULL)
  {
    xoParseExceptionClass = (*env)->NewGlobalRef(env, xoParseExceptionClass);
    if (xoParseExceptionClass == NULL)
    {
      jresult = JNI_FALSE;
      printf("Cannot get a global reference on com.actility.m2m.xo.XoParseException");
    }
  }
  else
  {
    jresult = JNI_FALSE;
    printf("Cannot get a reference on com.actility.m2m.xo.XoParseException");
  }
  xoUnknownTypeExceptionClass = (*env)->FindClass(env, "com/actility/m2m/xo/XoUnknownTypeException");
  if (xoUnknownTypeExceptionClass != NULL)
  {
    xoUnknownTypeExceptionClass = (*env)->NewGlobalRef(env, xoUnknownTypeExceptionClass);
    if (xoUnknownTypeExceptionClass == NULL)
    {
      jresult = JNI_FALSE;
      printf("Cannot get a global reference on com.actility.m2m.xo.XoUnknownTypeException");
    }
  }
  else
  {
    jresult = JNI_FALSE;
    printf("Cannot get a reference on com.actility.m2m.xo.XoUnknownTypeException");
  }
  illegalStateExceptionClass = (*env)->FindClass(env, "java/lang/IllegalStateException");
  if (illegalStateExceptionClass != NULL)
  {
    illegalStateExceptionClass = (*env)->NewGlobalRef(env, illegalStateExceptionClass);
    if (illegalStateExceptionClass == NULL)
    {
      jresult = JNI_FALSE;
      printf("Cannot get a global reference on java.lang.IllegalStateException");
    }
  }
  else
  {
    jresult = JNI_FALSE;
    printf("Cannot get a reference on java.lang.IllegalStateException");
  }

  if (initialized == 0)
  {
    initialized = 1;
    int result = 0;
    if (xoNamespacesFile != NULL)
    {
      // Initialize namespaces
      char* cXoNamespacesFile = (char*) (*env)->GetStringUTFChars(env, xoNamespacesFile, 0);
      result = XoLoadNameSpace(cXoNamespacesFile);
      if (result < 0)
      {
        jresult = JNI_FALSE;
        printf("XoLoadNameSpace() error %d\n", result);
      }
      (*env)->ReleaseStringUTFChars(env, xoNamespacesFile, cXoNamespacesFile);
    }
    if (xoRefFile != NULL)
    {
      // Initialize refs
      char* cXoRefFile = (char*) (*env)->GetStringUTFChars(env, xoRefFile, 0);
      result = XoExtLoadRef(cXoRefFile);
      if (result < 0)
      {
        jresult = JNI_FALSE;
        printf("XoExtLoadRef() error %d\n", result);
      }
      (*env)->ReleaseStringUTFChars(env, xoRefFile, cXoRefFile);
    }
    if (dictFile != NULL)
    {
      // Initialize dictionary
      char* cDictFile = (char*) (*env)->GetStringUTFChars(env, dictFile, 0);
      result = XoLoadSharedDico(cDictFile);
      if (result < 0)
      {
        jresult = JNI_FALSE;
        printf("XoLoadSharedDico() error %d\n", result);
      }
      (*env)->ReleaseStringUTFChars(env, dictFile, cDictFile);
    }
  }

  return jresult;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_stop(JNIEnv * env, jclass class)
{
  (*env)->DeleteGlobalRef(env, xoExceptionClass);
  (*env)->DeleteGlobalRef(env, illegalStateExceptionClass);
  xoExceptionClass = NULL;
  illegalStateExceptionClass = NULL;
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_isBinarySupported(JNIEnv * env, jclass class)
{
  return (jboolean) 1;
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_isXmlSupported(JNIEnv * env, jclass class)
{
  return (jboolean) 1;
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_isExiSupported(JNIEnv * env, jclass class)
{
#ifdef EXI
  return (jboolean) 1;
#else
  return (jboolean) 0;
#endif
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoDumpNbOcc(JNIEnv * env, jclass class)
{
  // int32 XoDumpNbOcc (FILE *fout , int32 not0 , int32 notpredef );
  XoDumpNbOcc(stdout, 0, 0);
}

#ifdef LINUX_64
JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmType
(JNIEnv * env, jclass class, jint xoHandle)
{
  return NULL;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoLoadMem
(JNIEnv * env, jclass class, jbyteArray rawObject)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadXmlMem
(JNIEnv * env, jclass class, jbyteArray rawObject, jboolean obix)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialXmlMem
(JNIEnv * env, jclass class, jbyteArray rawObject, jstring rootType)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadExiMem
(JNIEnv * env, jclass class, jbyteArray rawObject, jboolean obix)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialExiMem
(JNIEnv * env, jclass class, jbyteArray rawObject, jstring rootType)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNew
(JNIEnv * env, jclass class, jstring type)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNewCopy
(JNIEnv * env, jclass class, jint xoHandle, jboolean recurse)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSetNameSpace
(JNIEnv * env, jclass class, jint xoHandle, jstring prefix)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoUnSetNameSpace
(JNIEnv * env, jclass class, jint xoHandle, jstring prefix)
{
  return (jint) 0;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToXml
(JNIEnv * env, jclass class, jint xoHandle, jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToExi
(JNIEnv * env, jclass class, jint xoHandle, jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToBinary
(JNIEnv * env, jclass class, jint xoHandle, jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToExi
(JNIEnv * env, jclass class, jint xoHandle, jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToBinary
(JNIEnv * env, jclass class, jint xoHandle, jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToXml
(JNIEnv * env, jclass class, jint xoHandle, jstring path, jboolean obix)
{
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem
(JNIEnv * env, jclass class, jint xoHandle, jstring defaultNamespace, jboolean obix)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialXmlMem
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jstring defaultNamespace)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem
(JNIEnv * env, jclass class, jint xoHandle, jboolean obix)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialExiMem
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSaveMem
(JNIEnv * env, jclass class, jint xoHandle)
{
  return NULL;
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmContainsAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmGetAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmGetAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBufferNmGetAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return NULL;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmSetAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jint xoHandleValue)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmSetAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jstring value)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBuffergNmSetAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jbyteArray value)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmUnSetAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmAddInAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jint xoHandleValue)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmAddInAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jstring value)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmIndexOf
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jint xoHandleValue)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmIndexOf
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName, jstring value)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNbInAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmFirstAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNextAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmLastAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmSupInAttr
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmDropList
(JNIEnv * env, jclass class, jint xoHandle, jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoFree
(JNIEnv * env, jclass class, jint xoHandle, jboolean cascadeMode)
{
  return (jint) 0;
}

JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmType64
(JNIEnv * env, jclass class, jlong xoHandle)
{
  char* type = XoNmType((void*) xoHandle);
  jstring jType = NULL;
  if (type != NULL)
  {
    jType = (*env)->NewStringUTF(env, type);
  }
  return jType;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoLoadMem64
(JNIEnv * env, jclass class, jbyteArray rawObject)
{
  //void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  void* xoObject = NULL;
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  xoObject = XoLoadMem(buffer, bufferLength);
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (xoObject == NULL)
  {
    (*env)->ThrowNew(env, xoParseExceptionClass, "XoLoadMem() parsing error");
  }
  return (jlong) xoObject;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadXmlMem64
(JNIEnv * env, jclass class, jbyteArray rawObject, jboolean obix)
{
  //void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  int flags = 0;
  void* xoObject = NULL;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  xoObject = XoReadXmlMem(buffer, bufferLength, NULL, flags, &result);
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadXmlMem() unknown type error=%d", result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix && obix == JNI_FALSE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
    else if (!isObix && obix == JNI_TRUE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read oBIX while it is XML", result);
    }
  }
  return (jlong) xoObject;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialXmlMem64
(JNIEnv * env, jclass class, jbyteArray rawObject, jstring rootType)
{
  //void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  void* xoObject = NULL;
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  char* cRootType = NULL;
  if (rootType != NULL)
  {
    cRootType = (char*) (*env)->GetStringUTFChars(env, rootType, 0);
  }
  xoObject = XoReadXmlMem(buffer, bufferLength, cRootType, XOML_PARSE_TARGET, &result);
  if (rootType != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, rootType, cRootType);
  }
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadXmlMem() unknown type error=%d", result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
  }
  return (jlong) xoObject;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadExiMem64
(JNIEnv * env, jclass class, jbyteArray rawObject, jboolean obix)
{
#ifdef EXI
  //void *XoReadExiMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  int flags = 0;
  void* xoObject = NULL;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  xoObject = XoReadExiMem(buffer, bufferLength, NULL, flags, &result);
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadExiMem() unknown type error=%d", result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix && obix == JNI_FALSE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
    else if (!isObix && obix == JNI_TRUE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read oBIX while it is XML", result);
    }
  }
  return (jlong) xoObject;
#else
  return (jlong) 0;
#endif
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialExiMem64
(JNIEnv * env, jclass class, jbyteArray rawObject, jstring rootType)
{
#ifdef EXI
  //void *XoReadExiMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  void* xoObject = NULL;
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  char* cRootType = NULL;
  if (rootType != NULL)
  {
    cRootType = (char*) (*env)->GetStringUTFChars(env, rootType, 0);
  }
  xoObject = XoReadExiMem(buffer, bufferLength, cRootType, XOML_PARSE_TARGET, &result);
  if (rootType != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, rootType, cRootType);
  }
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadExiMem() unknown type error=%d", result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
  }
  return (jlong) xoObject;
#else
  return (jlong) 0;
#endif
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNew64
(JNIEnv * env, jclass class, jstring type)
{
  //void *XoNmNew(char* nameclass)
  char* cType = (char*) (*env)->GetStringUTFChars(env, type, 0);
  void* obj = XoNmNew(cType);
  XoNmSetAttr (obj , XOML_BALISE_ATTR, cType, 0);
  (*env)->ReleaseStringUTFChars(env, type, cType);
  return (jlong) obj;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNewCopy64
(JNIEnv * env, jclass class, jlong xoHandle, jboolean recurse)
{
  //void *XoNewCopy(void* obj, int32 reccur)
  int cRecurse = (recurse == JNI_TRUE) ? 1 : 0;
  return (jlong) XoNewCopy((void*) xoHandle, cRecurse);
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSetNameSpace64
(JNIEnv * env, jclass class, jlong xoHandle, jstring prefix)
{
  char* cPrefix = NULL;
  if (prefix != NULL)
  {
    cPrefix = (char*) (*env)->GetStringUTFChars(env, prefix, 0);
  }
  jint result = (jint) XoSetNameSpace((void*) xoHandle, cPrefix);
  if (prefix != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, prefix, cPrefix);
  }
  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoUnSetNameSpace64
(JNIEnv * env, jclass class, jlong xoHandle, jstring prefix)
{
  char* cPrefix = NULL;
  if (prefix != NULL)
  {
    cPrefix = (char*) (*env)->GetStringUTFChars(env, prefix, 0);
  }
  jint result = (jint) XoUnSetNameSpace((void*) xoHandle, cPrefix);
  if (prefix != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, prefix, cPrefix);
  }
  return result;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToXml64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jboolean obix)
{
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    void* xoObject = XoLoadMem(buffer, bufferLength);

    if (xoObject != NULL)
    {
      // 3. save binary
      char *newBuffer = NULL;
      void *w;
      //int xoLength = 0;
      int flags = 0;
      if (obix == JNI_TRUE)
      {
        flags = XOML_PARSE_OBIX;
      }

      w = XoWritXmlMem(xoObject, flags, &newBuffer, "");
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        int newBufferLength = strlen(newBuffer);
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, newBufferLength);
        XoWritXmlFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritXmlMem() error");
      }
    }
    else
    {
      (*env)->ThrowNew(env, xoParseExceptionClass, "XoLoadMem() parsing error");
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToExi64
(JNIEnv * env, jclass class, jlong xoHandle, jstring path, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    void* xoObject = XoLoadMem(buffer, bufferLength);

    if (xoObject != NULL)
    {
      // 3. save binary
      char *newBuffer = NULL;
      int length;
      void *w;
      //int xoLength = 0;
      int flags = 0;
      if (obix == JNI_TRUE)
      {
        flags = XOML_PARSE_OBIX;
      }

      w = XoWritExiMem(xoObject, flags, &newBuffer, &length, "", 0);
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, length);
        XoWritExiFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritExiMem() error");
      }
    }
    else
    {
      (*env)->ThrowNew(env, xoParseExceptionClass, "XoLoadMem() parsing error");
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToBinary64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jboolean obix)
{
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadXmlMem(buffer, bufferLength, NULL, flags, &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      int newBufferLength = 0;

      XoSaveMemAllocFlg(xoObject, &newBuffer, &newBufferLength, XO_SHARED_DICO | XO_COMPRESS_0);
      XoFree(xoObject, 1);
      if (newBuffer != NULL)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, newBufferLength);
        free(newBuffer);
      }
      else
      {
        (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToExi64
(JNIEnv * env, jclass class, jlong xoHandle, jstring path, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadXmlMem(buffer, bufferLength, NULL, flags, &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      int length;

      w = XoWritExiMem(xoObject, flags, &newBuffer, &length, "", 0);
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, length);
        XoWritExiFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritExiMem() error");
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToBinary64
(JNIEnv * env, jclass class, jlong xoHandle, jstring path, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadExiMem(buffer, bufferLength, NULL, flags, &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64, "XoReadExiMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      int newBufferLength = 0;

      XoSaveMemAllocFlg(xoObject, &newBuffer, &newBufferLength, XO_SHARED_DICO | XO_COMPRESS_0);
      XoFree(xoObject, 1);
      if (newBuffer != NULL)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, newBufferLength);
        free(newBuffer);
      }
      else
      {
        (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToXml64
(JNIEnv * env, jclass class, jlong xoHandle, jstring path, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadExiMem(buffer, bufferLength, NULL, flags, &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64, "XoReadExiMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      int newBufferLength = 0;

      w = XoWritXmlMem(xoObject, flags, &newBuffer, "");
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        int newBufferLength = strlen(newBuffer);
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, newBufferLength);
        XoWritXmlFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritXmlMem() error");
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem64
(JNIEnv * env, jclass class, jlong xoHandle, jstring defaultNamespace, jboolean obix)
{
  //void *XoWritXmlMem(void *obj,int32 flags,char **bufout,char *defns);
  jbyteArray rawObject = NULL;
  void *w;
  char *buffer;

  int flags = 0;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  char* cDefaultNamespace = (char*) (*env)->GetStringUTFChars(env, defaultNamespace, 0);
  w = XoWritXmlMem((void*) xoHandle, flags, &buffer, cDefaultNamespace);
  (*env)->ReleaseStringUTFChars(env, defaultNamespace, cDefaultNamespace);
  if (w)
  {
    int bufferLength = strlen(buffer);
    rawObject = (*env)->NewByteArray(env, bufferLength);
    (*env)->SetByteArrayRegion(env, rawObject, 0, bufferLength, (jbyte*) buffer);
    XoWritXmlFreeMem(w);
  }
  else
  {
    (*env)->ThrowNew(env, xoExceptionClass, "XoWritXmlMem() error");
  }
  return rawObject;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialXmlMem64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jstring defaultNamespace)
{
  //void *XoWritXmlMem(void *obj,int32 flags,char **bufout,char *defns);
  jbyteArray rawObject = NULL;
  void* xoObject = (void*) xoHandle;
  char* attr = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  void* attrValue = NULL;
  void* tmpObject = NULL;
  int32 numObj, numAttr, modeAttr;

  // Get object type
  numObj = XoType(xoObject);
  if (numObj < 0)
  {
    throwXoException(env, 128, "'%ld' cannot retrieve numObj", (long) xoObject);
    return rawObject;
  }

  // Get attribute field
  numAttr = XoNmExistField(xoObject, attr);
  if (numAttr < 0)
  {
    throwXoException(env, 128, "'%s' does not exist in object '%s'", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute value
  attrValue = XoGetAttr(xoObject, numAttr, 0);
  if (attrValue == NULL)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot partially encode a NULL attr", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute mode
  modeAttr = XoModeAttr(numAttr);
  if (modeAttr < 0)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot retrieve modeAttr", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  switch (modeAttr)
  {
    case BASIC_ATTR:
    tmpObject = XoNew(numObj);
    XoSetNameSpace(tmpObject, NULL);
    XoNmSetAttr(tmpObject, attr, attrValue, 0);
    XoNmSetAttr(tmpObject, XOML_BALISE_ATTR, "<", 0);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem64(env, class, (jlong) tmpObject, defaultNamespace, JNI_FALSE);
    XoFree(tmpObject, 1);
    break;
    case OBJ_ATTR:
    XoSetNameSpace(attrValue, NULL);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem64(env, class, (jlong) attrValue, defaultNamespace, JNI_FALSE);
    XoUnSetNameSpace(attrValue, NULL);
    break;
    case LIST_OBJ_ATTR:
    case LIST_BASIC_ATTR:
    throwXoException(env, 128, "'%s'.'%s' cannot partially encode a list attribute", attr, XoGetRefObjName(numObj));
    break;
    default:
    throwXoException(env, 128, "'%s'.'%s' unknown modeAttr %d", attr, XoGetRefObjName(numObj), modeAttr);
    break;
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, attr);

  return rawObject;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem64
(JNIEnv * env, jclass class, jlong xoHandle, jboolean obix)
{
#ifdef EXI
  //void *XoWritExiMem(void *obj,int32 flags,char **bufout,int *len,char *schemaId,int useRootObj);
  jbyteArray rawObject = NULL;
  void *w;
  char *buffer;
  int length;

  int flags = 0;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  w = XoWritExiMem((void*) xoHandle, flags, &buffer, &length, "", 0);
  if (w)
  {
    rawObject = (*env)->NewByteArray(env, length);
    (*env)->SetByteArrayRegion(env, rawObject, 0, length, (jbyte*) buffer);
    XoWritExiFreeMem(w);
  }
  else
  {
    (*env)->ThrowNew(env, xoExceptionClass, "XoWritExiMem() error");
  }
  return rawObject;
#else
  return NULL;
#endif
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialExiMem64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
#ifdef EXI
  //void *XoWritExiMem(void *obj,int32 flags,char **bufout,int *len,char *schemaId,int useRootObj);
  jbyteArray rawObject = NULL;
  void* xoObject = (void*) xoHandle;
  char* attr = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  void* attrValue = NULL;
  void* tmpObject = NULL;
  int32 numObj, numAttr, modeAttr;

  // Get object type
  numObj = XoType(xoObject);
  if (numObj < 0)
  {
    throwXoException(env, 128, "'%ld' cannot retrieve numObj", (long) xoObject);
    return rawObject;
  }

  // Get attribute field
  numAttr = XoNmExistField(xoObject, attr);
  if (numAttr < 0)
  {
    throwXoException(env, 128, "'%s' does not exist in object '%s'", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute value
  attrValue = XoGetAttr(xoObject, numAttr, 0);
  if (attrValue == NULL)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot partially encode a NULL attr", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute mode
  modeAttr = XoModeAttr(numAttr);
  if (modeAttr < 0)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot retrieve modeAttr", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  switch (modeAttr)
  {
    case BASIC_ATTR:
    tmpObject = XoNew(numObj);
    XoSetNameSpace(tmpObject, NULL);
    XoNmSetAttr(tmpObject, attr, attrValue, 0);
    XoNmSetAttr(tmpObject, XOML_BALISE_ATTR, "<", 0);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem(env, class, (jlong) tmpObject, JNI_FALSE);
    XoFree(tmpObject, 1);
    break;
    case OBJ_ATTR:
    XoSetNameSpace(attrValue, NULL);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem(env, class, (jlong) attrValue, JNI_FALSE);
    XoUnSetNameSpace(attrValue, NULL);
    break;
    case LIST_OBJ_ATTR:
    case LIST_BASIC_ATTR:
    throwXoException(env, 128, "'%s'.'%s' cannot partially encode a list attribute", attr, XoGetRefObjName(numObj));
    break;
    default:
    throwXoException(env, 128, "'%s'.'%s' unknown modeAttr %d", attr, XoGetRefObjName(numObj), modeAttr);
    break;
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, attr);

  return rawObject;
#else
  return NULL;
#endif
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSaveMem64
(JNIEnv * env, jclass class, jlong xoHandle)
{
  //int32 XoSaveMemAllocFlg (void *obj , char *fout, int* len, int flags );
  jbyteArray rawObject = NULL;
  char *buffer = NULL;
  int bufferLength = 0;
  int xoLength = 0;

  xoLength = XoSaveMemAllocFlg((void*) xoHandle, &buffer, &bufferLength, XO_SHARED_DICO | XO_COMPRESS_0);
  if (buffer != NULL)
  {
    rawObject = (*env)->NewByteArray(env, xoLength);
    (*env)->SetByteArrayRegion(env, rawObject, 0, xoLength, (jbyte*) buffer);
    free(buffer);
  }
  else
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
  }
  return rawObject;
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmContainsAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32   XoNmIsSetAttr(void *obj, char *nomChp,...)
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int32 result = XoNmIsSetAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return (result) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmGetAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //void *XoNmGetAttr (void *obj , char *nomChp , int32 *lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jlong result = (jlong) XoNmGetAttr((void*) xoHandle, cAttributeName, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmGetAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //void *XoNmGetAttr (void *obj , char *nomChp , int32 *lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  void* attrValue = XoNmGetAttr((void*) xoHandle, cAttributeName, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
  if (attrValue == NULL)
  {
    return NULL;
  }
  else
  {
    return (*env)->NewStringUTF(env, (char*) attrValue);
  }
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBufferNmGetAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //void *XoNmGetAttr (void *obj , char *nomChp , int32 *lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jbyteArray buffer = NULL;
  int bufferLength = 0;
  void* attrValue = XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
  if (attrValue != NULL)
  {
    buffer = (*env)->NewByteArray(env, bufferLength);
    (*env)->SetByteArrayRegion(env, buffer, 0, bufferLength, (jbyte*) attrValue);
  }
  return buffer;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmSetAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jlong xoHandleValue)
{
  //int32 XoNmSetAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  XoNmSetAttr((void*) xoHandle, cAttributeName, (void*) xoHandleValue, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmSetAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jstring value)
{
  //int32 XoNmSetAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  char* cValue = (char*) (*env)->GetStringUTFChars(env, value, 0);
  XoNmSetAttr((void*) xoHandle, cAttributeName, cValue, 0);
  (*env)->ReleaseStringUTFChars(env, value, cValue);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBufferNmSetAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jbyteArray value)
{
  //int32 XoNmSetAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = (int) (*env)->GetArrayLength(env, value);
  char* buffer = (char*) (*env)->GetByteArrayElements(env, value, 0);
  XoNmSetAttr((void*) xoHandle, cAttributeName, buffer, bufferLength);
  (*env)->ReleaseByteArrayElements(env, value, (jbyte*) buffer, JNI_ABORT);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmUnSetAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32 XoNmUnSetAttr(void *obj, char *nomChp,...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  XoNmUnSetAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmAddInAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jlong xoHandleValue)
{
  //int32 XoNmAddInAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmAddInAttr((void*) xoHandle, cAttributeName, (void*) xoHandleValue, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmAddInAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jstring value)
{
  //int32 XoNmAddInAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  char* cValue = (char*) (*env)->GetStringUTFChars(env, value, 0);
  jint result = (jint) XoNmAddInAttr((void*) xoHandle, cAttributeName, cValue, 0);
  (*env)->ReleaseStringUTFChars(env, value, cValue);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmIndexOf64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jlong xoHandleValue)
{
  int lg = 0;
  void* cXoHandle = (void*) xoHandle;
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int pos = XoNmFirstAttr(cXoHandle, cAttributeName);
  int index = 0;
  void* targetHandle = (void*) xoHandleValue;
  void* sub = NULL;
  while (pos > 0)
  {
    sub = XoNmGetAttr(cXoHandle, cAttributeName, &lg);
    if (!sub)
    {
      index = -1;
      break;
    }
    else if (sub == targetHandle)
    {
      break;
    }
    else
    {
      pos = XoNmNextAttr(cXoHandle, cAttributeName);
      if (pos > 0)
      {
        index++;
      }
      else
      {
        // end of the list, and not found
        index = -1;
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return (jint) index;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmIndexOf64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName, jstring value)
{
  int lg = 0;
  void* cXoHandle = (void*) xoHandle;
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int pos = XoNmFirstAttr(cXoHandle, cAttributeName);
  int index = 0;
  char* cValue = (char*) (*env)->GetStringUTFChars(env, value, 0);
  void* sub = NULL;
  while (pos > 0)
  {
    sub = XoNmGetAttr(cXoHandle, cAttributeName, &lg);
    if (!sub)
    {
      index = -1;
      break;
    }
    else if (strcmp((char*) sub, cValue) == 0)
    {
      break;
    }
    else
    {
      pos = XoNmNextAttr(cXoHandle, cAttributeName);
      if (pos > 0)
      {
        index++;
      }
      else
      {
        // end of the list, and not found
        index = -1;
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, value, cValue);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return (jint) index;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNbInAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32 XoNmNbInAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmNbInAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmFirstAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32 XoNmFirstAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmFirstAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNextAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32 XoNmNextAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmNextAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmLastAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32 XoNmLastAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmLastAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmSupInAttr64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32 XoNmSupInAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmSupInAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmDropList64
(JNIEnv * env, jclass class, jlong xoHandle, jstring attributeName)
{
  //int32 XoNmDropList (void *obj , char *nomChp , int32 withfree , int32 reccur , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmDropList((void*) xoHandle, cAttributeName, 1, 1);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoFree64
(JNIEnv * env, jclass class, jlong xoHandle, jboolean cascadeMode)
{
  //int32 XoFree (void *obj , int32 reccur );
  return (jint) XoFree((void*) xoHandle, (int) cascadeMode);
}

#else

JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmType(JNIEnv * env, jclass class, jint xoHandle)
{
  char* type = XoNmType((void*) xoHandle);
  jstring jType = NULL;
  if (type != NULL)
  {
    jType = (*env)->NewStringUTF(env, type);
  }
  return jType;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoLoadMem(JNIEnv * env, jclass class, jbyteArray rawObject)
{
  //void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  void* xoObject = NULL;
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  xoObject = XoLoadMem(buffer, bufferLength);
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (xoObject == NULL)
  {
    (*env)->ThrowNew(env, xoParseExceptionClass, "XoLoadMem() parsing error");
  }
  return (jint) xoObject;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadXmlMem(JNIEnv * env, jclass class, jbyteArray rawObject,
    jboolean obix)
{
  //void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  void* xoObject = NULL;
  int flags = 0;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  xoObject = XoReadXmlMem(buffer, bufferLength, NULL, flags, &result);
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadXmlMem() unknown type error=%d", result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix && obix == JNI_FALSE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
    else if (!isObix && obix == JNI_TRUE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read oBIX while it is XML", result);
    }
  }
  return (jint) xoObject;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialXmlMem(JNIEnv * env, jclass class, jbyteArray rawObject,
    jstring rootType)
{
  //void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  void* xoObject = NULL;
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  char* cRootType = NULL;
  if (rootType != NULL)
  {
    cRootType = (char*) (*env)->GetStringUTFChars(env, rootType, 0);
  }
  xoObject = XoReadXmlMem(buffer, bufferLength, cRootType, XOML_PARSE_TARGET, &result);
  if (rootType != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, rootType, cRootType);
  }
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadXmlMem() unknown type error=%d", result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
  }
  return (jint) xoObject;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadExiMem(JNIEnv * env, jclass class, jbyteArray rawObject,
    jboolean obix)
{
#ifdef EXI
  //void *XoReadExiMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  void* xoObject = NULL;
  int flags = 0;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  xoObject = XoReadExiMem(buffer, bufferLength, NULL, flags, &result);
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadExiMem() unknown type error=%d",
        result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix && obix == JNI_FALSE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
    else if (!isObix && obix == JNI_TRUE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read oBIX while it is XML", result);
    }
  }
  return (jint) xoObject;
#else
  return (jint) 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialExiMem(JNIEnv * env, jclass class, jbyteArray rawObject,
    jstring rootType)
{
#ifdef EXI
  //void *XoReadExiMem(char *buf,int sz,char *roottype,int flags,int *err);
  int bufferLength = (int) (*env)->GetArrayLength(env, rawObject);
  int result = 0;
  void* xoObject = NULL;
  char* buffer = (char*) (*env)->GetByteArrayElements(env, rawObject, 0);
  char* cRootType = NULL;
  if (rootType != NULL)
  {
    cRootType = (char*) (*env)->GetStringUTFChars(env, rootType, 0);
  }
  xoObject = XoReadExiMem(buffer, bufferLength, cRootType, XOML_PARSE_TARGET,
      &result);
  if (rootType != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, rootType, cRootType);
  }
  (*env)->ReleaseByteArrayElements(env, rawObject, (jbyte*) buffer, JNI_ABORT);

  if (result > 0)
  {
    throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
  }
  else if (result < 0)
  {
    throwXoUnknownTypeException(env, 64, "XoReadExiMem() unknown type error=%d",
        result);
  }
  else
  {
    int32 isObix = XoIsObix(xoObject);
    if (isObix && obix == JNI_FALSE)
    {
      XoFree(xoObject, 1);
      xoObject = NULL;
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() try to read XML while it is oBIX", result);
    }
  }
  return (jint) xoObject;
#else
  return (jint) 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNew(JNIEnv * env, jclass class, jstring type)
{
  //void *XoNmNew(char* nameclass)
  char* cType = (char*) (*env)->GetStringUTFChars(env, type, 0);
  void* obj = XoNmNew(cType);
  XoNmSetAttr(obj, XOML_BALISE_ATTR, cType, 0);
  (*env)->ReleaseStringUTFChars(env, type, cType);
  return (jint) obj;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNewCopy(JNIEnv * env, jclass class, jint xoHandle, jboolean recurse)
{
  //void *XoNewCopy(void* obj, int32 reccur)
  int cRecurse = (recurse == JNI_TRUE) ? 1 : 0;
  return (jint) XoNewCopy((void*) xoHandle, cRecurse);
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSetNameSpace(JNIEnv * env, jclass class, jint xoHandle,
    jstring prefix)
{
  char* cPrefix = NULL;
  if (prefix != NULL)
  {
    cPrefix = (char*) (*env)->GetStringUTFChars(env, prefix, 0);
  }
  jint result = (jint) XoSetNameSpace((void*) xoHandle, cPrefix);
  if (prefix != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, prefix, cPrefix);
  }
  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoUnSetNameSpace(JNIEnv * env, jclass class, jint xoHandle,
    jstring prefix)
{
  char* cPrefix = NULL;
  if (prefix != NULL)
  {
    cPrefix = (char*) (*env)->GetStringUTFChars(env, prefix, 0);
  }
  jint result = (jint) XoUnSetNameSpace((void*) xoHandle, cPrefix);
  if (prefix != NULL)
  {
    (*env)->ReleaseStringUTFChars(env, prefix, cPrefix);
  }
  return result;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToXml(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jboolean obix)
{
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    void* xoObject = XoLoadMem(buffer, bufferLength);

    if (xoObject != NULL)
    {
      // 3. save binary
      char *newBuffer = NULL;
      void *w = NULL;
      int flags = 0;
      if (obix == JNI_TRUE)
      {
        flags = XOML_PARSE_OBIX;
      }

      w = XoWritXmlMem(xoObject, flags, &newBuffer, "");
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        int newBufferLength = strlen(newBuffer);
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, newBufferLength);
        XoWritXmlFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritXmlMem() error");
      }
    }
    else
    {
      (*env)->ThrowNew(env, xoParseExceptionClass, "XoLoadMem() parsing error");
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToExi(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName,
      0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName,
      &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    void* xoObject = XoLoadMem(buffer, bufferLength);

    if (xoObject != NULL)
    {
      // 3. save binary
      char *newBuffer = NULL;
      int length;
      void *w = NULL;
      int flags = 0;
      if (obix == JNI_TRUE)
      {
        flags = XOML_PARSE_OBIX;
      }

      w = XoWritExiMem(xoObject, flags, &newBuffer, &length, "", 0);
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer,
            length);
        XoWritExiFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritExiMem() error");
      }
    }
    else
    {
      (*env)->ThrowNew(env, xoParseExceptionClass, "XoLoadMem() parsing error");
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToBinary(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jboolean obix)
{
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadXmlMem(buffer, bufferLength, NULL, flags, &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64, "XoReadXmlMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      int newBufferLength = 0;

      XoSaveMemAllocFlg(xoObject, &newBuffer, &newBufferLength, XO_SHARED_DICO | XO_COMPRESS_0);
      XoFree(xoObject, 1);
      if (newBuffer != NULL)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer, newBufferLength);
        free(newBuffer);
      }
      else
      {
        (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToExi(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName,
      0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName,
      &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadXmlMem(buffer, bufferLength, NULL, flags,
        &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadXmlMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64,
          "XoReadXmlMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      int length;
      void *w = NULL;

      w = XoWritExiMem(xoObject, flags, &newBuffer, &length, "", 0);
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer,
            length);
        XoWritExiFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritExiMem() error");
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToBinary(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName,
      0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName,
      &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadExiMem(buffer, bufferLength, NULL, flags,
        &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64,
          "XoReadExiMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      int newBufferLength = 0;

      XoSaveMemAllocFlg(xoObject, &newBuffer, &newBufferLength,
          XO_SHARED_DICO | XO_COMPRESS_0);
      XoFree(xoObject, 1);
      if (newBuffer != NULL)
      {
        // 4. set attribute
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer,
            newBufferLength);
        free(newBuffer);
      }
      else
      {
        (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToXml(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jboolean obix)
{
#ifdef EXI
  // 1. get attribute
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName,
      0);
  int bufferLength = 0;
  char* buffer = (char*) XoNmGetAttr((void*) xoHandle, cAttributeName,
      &bufferLength);
  if (buffer != NULL)
  {
    // 2. read attribute
    int result = 0;
    int flags = 0;
    if (obix == JNI_TRUE)
    {
      flags = XOML_PARSE_OBIX;
    }
    void* xoObject = XoReadExiMem(buffer, bufferLength, NULL, flags,
        &result);

    if (result > 0)
    {
      throwXoParseException(env, 64, "XoReadExiMem() parsing error=%d", result);
    }
    else if (result < 0)
    {
      throwXoUnknownTypeException(env, 64,
          "XoReadExiMem() unknown type error=%d", result);
    }
    else
    {
      // 3. save binary
      char *newBuffer = NULL;
      void *w = NULL;

      w = XoWritXmlMem(xoObject, flags, &newBuffer, "");
      XoFree(xoObject, 1);
      if (w)
      {
        // 4. set attribute
        int newBufferLength = strlen(newBuffer);
        XoNmSetAttr((void*) xoHandle, cAttributeName, newBuffer,
            newBufferLength);
        XoWritXmlFreeMem(w);
      }
      else
      {
        (*env)->ThrowNew(env, xoExceptionClass, "XoWritXmlMem() error");
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
#endif
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem(JNIEnv * env, jclass class, jint xoHandle,
    jstring defaultNamespace, jboolean obix)
{
  //void *XoWritXmlMem(void *obj,int32 flags,char **bufout,char *defns);
  jbyteArray rawObject = NULL;
  void *w;
  char *buffer;

  int flags = 0;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  char* cDefaultNamespace = (char*) (*env)->GetStringUTFChars(env, defaultNamespace, 0);
  w = XoWritXmlMem((void*) xoHandle, flags, &buffer, cDefaultNamespace);
  (*env)->ReleaseStringUTFChars(env, defaultNamespace, cDefaultNamespace);
  if (w)
  {
    int bufferLength = strlen(buffer);
    rawObject = (*env)->NewByteArray(env, bufferLength);
    (*env)->SetByteArrayRegion(env, rawObject, 0, bufferLength, (jbyte*) buffer);
    XoWritXmlFreeMem(w);
  }
  else
  {
    (*env)->ThrowNew(env, xoExceptionClass, "XoWritXmlMem() error");
  }
  return rawObject;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialXmlMem(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jstring defaultNamespace)
{
  //void *XoWritXmlMem(void *obj,int32 flags,char **bufout,char *defns);
  jbyteArray rawObject = NULL;
  void* xoObject = (void*) xoHandle;
  char* attr = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  void* attrValue = NULL;
  void* tmpObject = NULL;
  int32 numObj, numAttr, modeAttr;

  // Get object type
  numObj = XoType(xoObject);
  if (numObj < 0)
  {
    throwXoException(env, 128, "'%d' cannot retrieve numObj", (int) xoObject);
    return rawObject;
  }

  // Get attribute field
  numAttr = XoNmExistField(xoObject, attr);
  if (numAttr < 0)
  {
    throwXoException(env, 128, "'%s' does not exist in object '%s'", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute value
  attrValue = XoGetAttr(xoObject, numAttr, 0);
  if (attrValue == NULL)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot partially encode a NULL attr", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute mode
  modeAttr = XoModeAttr(numAttr);
  if (modeAttr < 0)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot retrieve modeAttr", attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  switch (modeAttr)
  {
  case BASIC_ATTR:
    tmpObject = XoNew(numObj);
    XoSetNameSpace(tmpObject, NULL);
    XoNmSetAttr(tmpObject, attr, attrValue, 0);
    XoNmSetAttr(tmpObject, XOML_BALISE_ATTR, "<", 0);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem(env, class, (jint) tmpObject, defaultNamespace, JNI_FALSE);
    XoFree(tmpObject, 1);
    break;
  case OBJ_ATTR:
    XoSetNameSpace(attrValue, NULL);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem(env, class, (jint) attrValue, defaultNamespace, JNI_FALSE);
    XoUnSetNameSpace(attrValue, NULL);
    break;
  case LIST_OBJ_ATTR:
  case LIST_BASIC_ATTR:
    throwXoException(env, 128, "'%s'.'%s' cannot partially encode a list attribute", attr, XoGetRefObjName(numObj));
    break;
  default:
    throwXoException(env, 128, "'%s'.'%s' unknown modeAttr %d", attr, XoGetRefObjName(numObj), modeAttr);
    break;
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, attr);

  return rawObject;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem(JNIEnv * env, jclass class, jint xoHandle,
    jboolean obix)
{
#ifdef EXI
  //void *XoWritExiMem(void *obj,int32 flags,char **bufout,int *len,char *schemaId, int useRootObj);
  jbyteArray rawObject = NULL;
  void *w;
  char *buffer;
  int length;

  int flags = 0;
  if (obix == JNI_TRUE)
  {
    flags = XOML_PARSE_OBIX;
  }
  w = XoWritExiMem((void*) xoHandle, flags, &buffer, &length, "", 0);
  if (w)
  {
    rawObject = (*env)->NewByteArray(env, length);
    (*env)->SetByteArrayRegion(env, rawObject, 0, length,
        (jbyte*) buffer);
    XoWritExiFreeMem(w);
  }
  else
  {
    (*env)->ThrowNew(env, xoExceptionClass, "XoWritExiMem() error");
  }
  return rawObject;
#else
  return NULL;
#endif
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialExiMem(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
#ifdef EXI
  //void *XoWritExiMem(void *obj,int32 flags,char **bufout,int *len,char *schemaId, int useRootObj);
  jbyteArray rawObject = NULL;
  void* xoObject = (void*) xoHandle;
  char* attr = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  void* attrValue = NULL;
  void* tmpObject = NULL;
  int32 numObj, numAttr, modeAttr;

  // Get object type
  numObj = XoType(xoObject);
  if (numObj < 0)
  {
    throwXoException(env, 128, "'%d' cannot retrieve numObj", (int) xoObject);
    return rawObject;
  }

  // Get attribute field
  numAttr = XoNmExistField(xoObject, attr);
  if (numAttr < 0)
  {
    throwXoException(env, 128, "'%s' does not exist in object '%s'", attr,
        XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute value
  attrValue = XoGetAttr(xoObject, numAttr, 0);
  if (attrValue == NULL)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot partially encode a NULL attr",
        attr, XoGetRefObjName(numObj));
    return rawObject;
  }

  // Get attribute mode
  modeAttr = XoModeAttr(numAttr);
  if (modeAttr < 0)
  {
    throwXoException(env, 128, "'%s'.'%s' cannot retrieve modeAttr", attr,
        XoGetRefObjName(numObj));
    return rawObject;
  }

  switch (modeAttr)
  {
    case BASIC_ATTR:
    tmpObject = XoNew(numObj);
    XoSetNameSpace(tmpObject, NULL);
    XoNmSetAttr(tmpObject, attr, attrValue, 0);
    XoNmSetAttr(tmpObject, XOML_BALISE_ATTR, "<", 0);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem(env, class,
        (jint) tmpObject, JNI_FALSE);
    XoFree(tmpObject, 1);
    break;
    case OBJ_ATTR:
    XoSetNameSpace(attrValue, NULL);
    rawObject = Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem(env, class,
        (jint) attrValue, JNI_FALSE);
    XoUnSetNameSpace(attrValue, NULL);
    break;
    case LIST_OBJ_ATTR:
    case LIST_BASIC_ATTR:
    throwXoException(env, 128,
        "'%s'.'%s' cannot partially encode a list attribute", attr,
        XoGetRefObjName(numObj));
    break;
    default:
    throwXoException(env, 128, "'%s'.'%s' unknown modeAttr %d", attr,
        XoGetRefObjName(numObj), modeAttr);
    break;
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, attr);

  return rawObject;
#else
  return NULL;
#endif
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSaveMem(JNIEnv * env, jclass class, jint xoHandle)
{
  //int32 XoSaveMemAllocFlg (void *obj , char *fout, int* len, int flags );
  jbyteArray rawObject = NULL;
  char *buffer = NULL;
  int bufferLength = 0;
  int xoLength = 0;

  xoLength = XoSaveMemAllocFlg((void*) xoHandle, &buffer, &bufferLength, XO_SHARED_DICO | XO_COMPRESS_0);
  if (buffer != NULL)
  {
    rawObject = (*env)->NewByteArray(env, xoLength);
    (*env)->SetByteArrayRegion(env, rawObject, 0, xoLength, (jbyte*) buffer);
    free(buffer);
  }
  else
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, MSG_NO_MORE_MEMORY);
  }
  return rawObject;
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmContainsAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32   XoNmIsSetAttr(void *obj, char *nomChp,...)
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int32 result = XoNmIsSetAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return (result) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmGetAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //void *XoNmGetAttr (void *obj , char *nomChp , int32 *lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmGetAttr((void*) xoHandle, cAttributeName, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmGetAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //void *XoNmGetAttr (void *obj , char *nomChp , int32 *lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  void* attrValue = XoNmGetAttr((void*) xoHandle, cAttributeName, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
  if (attrValue == NULL)
  {
    return NULL;
  }
  else
  {
    return (*env)->NewStringUTF(env, (char*) attrValue);
  }
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBufferNmGetAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //void *XoNmGetAttr (void *obj , char *nomChp , int32 *lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jbyteArray buffer = NULL;
  int bufferLength = 0;
  void* attrValue = XoNmGetAttr((void*) xoHandle, cAttributeName, &bufferLength);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
  if (attrValue != NULL)
  {
    buffer = (*env)->NewByteArray(env, bufferLength);
    (*env)->SetByteArrayRegion(env, buffer, 0, bufferLength, (jbyte*) attrValue);
  }
  return buffer;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmSetAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jint xoHandleValue)
{
  //int32 XoNmSetAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  XoNmSetAttr((void*) xoHandle, cAttributeName, (void*) xoHandleValue, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmSetAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jstring value)
{
  //int32 XoNmSetAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  char* cValue = (char*) (*env)->GetStringUTFChars(env, value, 0);
  XoNmSetAttr((void*) xoHandle, cAttributeName, cValue, 0);
  (*env)->ReleaseStringUTFChars(env, value, cValue);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBufferNmSetAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jbyteArray value)
{
  //int32 XoNmSetAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int bufferLength = (int) (*env)->GetArrayLength(env, value);
  char* buffer = (char*) (*env)->GetByteArrayElements(env, value, 0);
  XoNmSetAttr((void*) xoHandle, cAttributeName, buffer, bufferLength);
  (*env)->ReleaseByteArrayElements(env, value, (jbyte*) buffer, JNI_ABORT);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmUnSetAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32 XoNmUnSetAttr(void *obj, char *nomChp,...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  XoNmUnSetAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmAddInAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jint xoHandleValue)
{
  //int32 XoNmAddInAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmAddInAttr((void*) xoHandle, cAttributeName, (void*) xoHandleValue, 0);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmAddInAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jstring value)
{
  //int32 XoNmAddInAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  char* cValue = (char*) (*env)->GetStringUTFChars(env, value, 0);
  jint result = (jint) XoNmAddInAttr((void*) xoHandle, cAttributeName, cValue, 0);
  (*env)->ReleaseStringUTFChars(env, value, cValue);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmIndexOf(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jint xoHandleValue)
{
  int lg = 0;
  void* cXoHandle = (void*) xoHandle;
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int pos = XoNmFirstAttr(cXoHandle, cAttributeName);
  int index = 0;
  void* targetHandle = (void*) xoHandleValue;
  void* sub = NULL;
  while (pos > 0)
  {
    sub = XoNmGetAttr(cXoHandle, cAttributeName, &lg);
    if (!sub)
    {
      index = -1;
      break;
    }
    else if (sub == targetHandle)
    {
      break;
    }
    else
    {
      pos = XoNmNextAttr(cXoHandle, cAttributeName);
      if (pos > 0)
      {
        index++;
      }
      else
      {
        // end of the list, and not found
        index = -1;
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return (jint) index;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmIndexOf(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName, jstring value)
{
  int lg = 0;
  void* cXoHandle = (void*) xoHandle;
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  int pos = XoNmFirstAttr(cXoHandle, cAttributeName);
  int index = 0;
  char* cValue = (char*) (*env)->GetStringUTFChars(env, value, 0);
  void* sub = NULL;
  while (pos > 0)
  {
    sub = XoNmGetAttr(cXoHandle, cAttributeName, &lg);
    if (!sub)
    {
      index = -1;
      break;
    }
    else if (strcmp((char*) sub, cValue) == 0)
    {
      break;
    }
    else
    {
      pos = XoNmNextAttr(cXoHandle, cAttributeName);
      if (pos > 0)
      {
        index++;
      }
      else
      {
        // end of the list, and not found
        index = -1;
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, value, cValue);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return index;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNbInAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32 XoNmNbInAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmNbInAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmFirstAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32 XoNmFirstAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmFirstAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNextAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32 XoNmNextAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmNextAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmLastAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32 XoNmLastAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmLastAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmSupInAttr(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32 XoNmSupInAttr (void *obj , char *nomChp , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmSupInAttr((void*) xoHandle, cAttributeName);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmDropList(JNIEnv * env, jclass class, jint xoHandle,
    jstring attributeName)
{
  //int32 XoNmDropList (void *obj , char *nomChp , int32 withfree , int32 reccur , ...);
  char* cAttributeName = (char*) (*env)->GetStringUTFChars(env, attributeName, 0);
  jint result = (jint) XoNmDropList((void*) xoHandle, cAttributeName, 1, 1);
  (*env)->ReleaseStringUTFChars(env, attributeName, cAttributeName);

  return result;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoFree(JNIEnv * env, jclass class, jint xoHandle,
    jboolean cascadeMode)
{
  //int32 XoFree (void *obj , int32 reccur );
  return (jint) XoFree((void*) xoHandle, (int) cascadeMode);
}

JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmType64(JNIEnv * env, jclass class, jlong xoHandle)
{
  return NULL;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoLoadMem64(JNIEnv * env, jclass class, jbyteArray rawObject)
{
  return (jlong) 0;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadXmlMem64(JNIEnv * env, jclass class, jbyteArray rawObject,
    jboolean obix)
{
  return (jlong) 0;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialXmlMem64(JNIEnv * env, jclass class,
    jbyteArray rawObject, jstring rootType)
{
  return (jlong) 0;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadExiMem64(JNIEnv * env, jclass class, jbyteArray rawObject,
    jboolean obix)
{
  return (jlong) 0;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoReadPartialExiMem64(JNIEnv * env, jclass class,
    jbyteArray rawObject, jstring rootType)
{
  return (jlong) 0;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNew64(JNIEnv * env, jclass class, jstring type)
{
  return (jlong) 0;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNewCopy64(JNIEnv * env, jclass class, jlong xoHandle,
    jboolean recurse)
{
  return (jlong) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSetNameSpace64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring prefix)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoUnSetNameSpace64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring prefix)
{
  return (jint) 0;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToXml64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferBinaryToExi64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToBinary64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferXmlToExi64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToBinary64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring path, jboolean obix)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmXoBufferExiToXml64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring path, jboolean obix)
{
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteXmlMem64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring defaultNamespace, jboolean obix)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialXmlMem64(JNIEnv * env, jclass class,
    jlong xoHandle, jstring attributeName, jstring defaultNamespace)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoSaveMem64(JNIEnv * env, jclass class, jlong xoHandle)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWriteExiMem64(JNIEnv * env, jclass class, jlong xoHandle,
    jboolean obix)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoWritePartialExiMem64(JNIEnv * env, jclass class,
    jlong xoHandle, jstring attributeName)
{
  return NULL;
}

JNIEXPORT jboolean JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmContainsAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return JNI_FALSE;
}

JNIEXPORT jlong JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmGetAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return (jlong) 0;
}

JNIEXPORT jstring JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmGetAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBufferNmGetAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return NULL;
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmSetAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName, jlong xoHandleValue)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmSetAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName, jstring value)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoBufferNmSetAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName, jbyteArray value)
{
}

JNIEXPORT void JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmUnSetAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmAddInAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName, jlong xoHandleValue)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmAddInAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName, jstring value)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoObjectNmIndexOf64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName, jlong xoHandleValue)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoStringNmIndexOf64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName, jstring value)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNbInAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmFirstAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmNextAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmLastAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmSupInAttr64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoNmDropList64(JNIEnv * env, jclass class, jlong xoHandle,
    jstring attributeName)
{
  return (jint) 0;
}

JNIEXPORT jint JNICALL Java_com_actility_m2m_xo_jni_JniXo_xoFree64(JNIEnv * env, jclass class, jlong xoHandle,
    jboolean cascadeMode)
{
  return (jint) 0;
}

#endif //LINUX_64
