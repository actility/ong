/* diajni.c -- 
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the libdiajni.so library. Please see
 * README for terms of use.
 */

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <poll.h>

#include <jni.h>
#include <stdio.h>

#include "com_actility_m2m_song_binding_coap_jni_impl_DiaJni.h"

#include <rtlbase.h>
#include <dIa.h>

//	XXX
static void *tabpoll;
static JNIEnv *g_env = NULL;
static jobject g_diaJniObj = NULL;
static jclass g_diaOptClass = NULL;
static jmethodID g_diaOptConst = NULL;
static jmethodID g_diaJniRetrieveRequest = NULL;
static jmethodID g_diaJniRetrieveResponse = NULL;
static jmethodID g_diaJniCreateRequest = NULL;
static jmethodID g_diaJniCreateResponse = NULL;
static jmethodID g_diaJniUpdateRequest = NULL;
static jmethodID g_diaJniUpdateResponse = NULL;
static jmethodID g_diaJniDeleteRequest = NULL;
static jmethodID g_diaJniDeleteResponse = NULL;
static jmethodID g_diaJniErrorResponse = NULL;
static jmethodID g_diaJniTraceFunction = NULL;
static jfieldID g_diaOptName = NULL;
static jfieldID g_diaOptVal = NULL;

int network_request(void *tab, int fd, void *ref1, void *ref2, int events)
{
  return POLLIN;
}

int network_input(void *tab, int fd, void *ref1, void *ref2, int events)
{
  dia_context_t *dia = (dia_context_t *) ref1;
  //	LOCK
  dia_EnterCS(dia);
  dia_input(dia);
  dia_LeaveCS(dia);
  return 0;
}

jobjectArray optionsToJava(JNIEnv *env, Opt *opt)
{
  jobjectArray opt_j;
  int size;
  if (!opt)
    return NULL;
  if (!g_diaOptClass)
    return NULL;
  return NULL;

  for (size = 0; opt[size].name; size++)
  {
  }
  if (!size)
    return NULL;

  opt_j = (*env)->NewObjectArray(env, size, g_diaOptClass, NULL);
  if (!opt_j)
    return NULL;

  int i;
  for (i = 0; i < size; i++)
  {
    jstring name = (*env)->NewStringUTF(env, opt[i].name);
    jstring val = (*env)->NewStringUTF(env, opt[i].val);
    jobject obj = (*env)->NewObject(env, g_diaOptClass, g_diaOptConst, name,
        val);
    (*env)->SetObjectArrayElement(env, opt_j, i, obj);
    (*env)->DeleteLocalRef(env, obj);
    (*env)->DeleteLocalRef(env, val);
    (*env)->DeleteLocalRef(env, name);
  }
  return opt_j;
}

int cb_diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
    Opt *optAttr, Opt *optHeader, int tid, char *peer)
{

  jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
  jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
  jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
  jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);
  jboolean proxyRequest_j = diaIsProxied(dia);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniRetrieveRequest,
      reqEntity_j, targetID_j, optAttr_j, optHeader_j, tid, peer_j, proxyRequest_j);

  (*g_env)->DeleteLocalRef(g_env, reqEntity_j);
  (*g_env)->DeleteLocalRef(g_env, targetID_j);
  if (optAttr_j)
    (*g_env)->DeleteLocalRef(g_env, optAttr_j);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
  (*g_env)->DeleteLocalRef(g_env, peer_j);

  return 0;
}

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len,
    char *cttType, Opt *optHeader, int tid)
{

  jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
  jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
  jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
  (*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *) ctt);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniRetrieveResponse,
      scode_j, bytes, cttType_j, optHeader_j, tid);

  (*g_env)->DeleteLocalRef(g_env, scode_j);
  (*g_env)->DeleteLocalRef(g_env, cttType_j);
  (*g_env)->DeleteLocalRef(g_env, bytes);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID,
    void *ctt, int len, char *cttType, Opt *optAttr, Opt *optHeader, int tid,
    char *peer)
{

  jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
  jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
  jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
  (*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *) ctt);
  jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
  jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
  jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);
  jboolean proxyRequest_j = diaIsProxied(dia);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniCreateRequest,
      reqEntity_j, targetID_j, bytes, cttType_j, optAttr_j, optHeader_j, tid,
      peer_j, proxyRequest_j);

  (*g_env)->DeleteLocalRef(g_env, reqEntity_j);
  (*g_env)->DeleteLocalRef(g_env, targetID_j);
  (*g_env)->DeleteLocalRef(g_env, cttType_j);
  (*g_env)->DeleteLocalRef(g_env, bytes);
  if (optAttr_j)
    (*g_env)->DeleteLocalRef(g_env, optAttr_j);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
  (*g_env)->DeleteLocalRef(g_env, peer_j);
  return 0;
}

void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI,
    void *ctt, int len, char* cttType, Opt *optHeader, int tid)
{

  jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
  jstring resourceURI_j = (*g_env)->NewStringUTF(g_env, resourceURI);
  jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
  (*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *) ctt);
  jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);

  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniCreateResponse, scode_j,
      resourceURI_j, bytes, cttType_j, optHeader_j, tid);

  (*g_env)->DeleteLocalRef(g_env, scode_j);
  (*g_env)->DeleteLocalRef(g_env, resourceURI_j);
  (*g_env)->DeleteLocalRef(g_env, cttType_j);
  (*g_env)->DeleteLocalRef(g_env, bytes);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

int cb_diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID,
    void *ctt, int len, char *cttType, Opt *optAttr, Opt *optHeader, int tid,
    char *peer)
{

  jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
  jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
  jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
  (*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *) ctt);
  jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
  jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
  jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);
  jboolean proxyRequest_j = diaIsProxied(dia);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniUpdateRequest,
      reqEntity_j, targetID_j, bytes, cttType_j, optAttr_j, optHeader_j, tid,
      peer_j, proxyRequest_j);

  (*g_env)->DeleteLocalRef(g_env, reqEntity_j);
  (*g_env)->DeleteLocalRef(g_env, targetID_j);
  (*g_env)->DeleteLocalRef(g_env, cttType_j);
  (*g_env)->DeleteLocalRef(g_env, bytes);
  if (optAttr_j)
    (*g_env)->DeleteLocalRef(g_env, optAttr_j);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
  (*g_env)->DeleteLocalRef(g_env, peer_j);
  return 0;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len,
    char* cttType, Opt *optHeader, int tid)
{

  jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
  jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
  (*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *) ctt);
  jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniUpdateResponse, scode_j,
      bytes, cttType_j, optHeader_j, tid);

  (*g_env)->DeleteLocalRef(g_env, scode_j);
  (*g_env)->DeleteLocalRef(g_env, cttType_j);
  (*g_env)->DeleteLocalRef(g_env, bytes);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
    Opt *optAttr, Opt *optHeader, int tid, char *peer)
{

  jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
  jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
  jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
  jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);
  jboolean proxyRequest_j = diaIsProxied(dia);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniDeleteRequest,
      reqEntity_j, targetID_j, optAttr_j, optHeader_j, tid, peer_j, proxyRequest_j);

  (*g_env)->DeleteLocalRef(g_env, reqEntity_j);
  (*g_env)->DeleteLocalRef(g_env, targetID_j);
  if (optAttr_j)
    (*g_env)->DeleteLocalRef(g_env, optAttr_j);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
  (*g_env)->DeleteLocalRef(g_env, peer_j);
  return 0;
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader,
    int tid)
{

  jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniDeleteResponse, scode_j,
      optHeader_j, tid);

  (*g_env)->DeleteLocalRef(g_env, scode_j);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType,
    Opt *optHeader, int tid)
{

//	jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
  jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
  (*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *) ctt);
  jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
  jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniErrorResponse, bytes,
      cttType_j, optHeader_j, tid);

//	(*g_env)->DeleteLocalRef(g_env, scode_j);
  (*g_env)->DeleteLocalRef(g_env, cttType_j);
  (*g_env)->DeleteLocalRef(g_env, bytes);
  if (optHeader_j)
    (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

void _traceFunction(int level, char *message)
{

  jstring message_j = (*g_env)->NewStringUTF(g_env, message);
  (*g_env)->CallVoidMethod(g_env, g_diaJniObj, g_diaJniTraceFunction, level,
      message_j);
  (*g_env)->DeleteLocalRef(g_env, message_j);
}

dia_callbacks_t callbacks = { cb_diaRetrieveRequest, cb_diaRetrieveResponse,
    cb_diaCreateRequest, cb_diaCreateResponse, cb_diaUpdateRequest,
    cb_diaUpdateResponse, cb_diaDeleteRequest, cb_diaDeleteResponse,
    cb_diaErrorResponse };

JNIEXPORT jlong JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaInit(JNIEnv *env, jobject obj,
    jstring host, jstring port, jint level, jstring logFile, jint sizeMax)
{

  dia_context_t *dia;
  char *host_str =
      host ? (char *) (*env)->GetStringUTFChars(env, host, NULL) : "::";
  char *port_str =
      port ? (char *) (*env)->GetStringUTFChars(env, port, NULL) : "5683";
  char *logFile_str =
      logFile ?
          (char *) (*env)->GetStringUTFChars(env, logFile, NULL) :
          "/tmp/DiaJni.log";

  rtl_tracemutex();
//	rtl_setTraceFunction(_traceFunction);
  dia_set_log_level((int) level);
  rtl_tracelevel((int) level);
  rtl_tracerotate(logFile_str);
  if ((int) sizeMax)
    rtl_tracesizemax((int) sizeMax);

  dia = dia_createContext(host_str, port_str, &callbacks);
  if (!dia)
    return -1;

  dia_setNonBlocking(dia, 20);

  tabpoll = rtl_pollInit();
  rtl_pollAdd(tabpoll, dia_getFd(dia), network_input, network_request, dia,
      NULL);

  if (host)
    (*env)->ReleaseStringUTFChars(env, host, host_str);
  if (port)
    (*env)->ReleaseStringUTFChars(env, port, port_str);
  if (logFile)
    (*env)->ReleaseStringUTFChars(env, logFile, logFile_str);

  return (jlong) dia;
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaTrace(JNIEnv *env, jobject obj,
    jint level)
{

  rtl_tracemutex();

  dia_set_log_level((int) level);
}

JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaPrepare(JNIEnv *env, jobject obj)
{
  // Java references are specific to a thread and must be buffered in this method !
  g_env = env;

  jclass illegalStateExceptionClass = (*env)->FindClass(env,
      "java/lang/IllegalStateException");
  if (illegalStateExceptionClass == NULL)
  {
    return (jint) - 1;
  }

  g_diaJniObj = obj;
  if (g_diaJniObj == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, "DiaJni object is null");
  }
  else
  {
    g_diaJniObj = (*env)->NewGlobalRef(env, g_diaJniObj);
    if (g_diaJniObj == NULL)
    {
      (*env)->ThrowNew(env, illegalStateExceptionClass,
          "Cannot create global ref from DiaJni object");
    }
  }

  jclass diaJniClass = (*env)->GetObjectClass(env, g_diaJniObj);
  if (diaJniClass == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on DiaJni class");
  }

  g_diaOptClass = (*env)->FindClass(env,
      "com/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl");
  if (g_diaOptClass == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on DiaOptImpl class");
  }
  else
  {
    g_diaOptClass = (*env)->NewGlobalRef(env, g_diaOptClass);
    if (g_diaOptClass == NULL)
    {
      (*env)->ThrowNew(env, illegalStateExceptionClass,
          "Cannot get a global reference on DiaOptImpl class");
    }
  }

  g_diaOptConst = (*env)->GetMethodID(env, g_diaOptClass, "<init>",
      "(Ljava/lang/String;Ljava/lang/String;)V");
  if (g_diaOptConst == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on DiaOptImpl constructor");
  }
  g_diaJniRetrieveRequest =
      (*env)->GetMethodID(env, diaJniClass, "cbDiaRetrieveRequest",
          "(Ljava/lang/String;Ljava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;ILjava/lang/String;Z)V");
  if (g_diaJniRetrieveRequest == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaRetrieveRequest");
  }
  g_diaJniRetrieveResponse =
      (*env)->GetMethodID(env, diaJniClass, "cbDiaRetrieveResponse",
          "(Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;I)V");
  if (g_diaJniRetrieveResponse == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaRetrieveResponse");
  }
  g_diaJniCreateRequest =
      (*env)->GetMethodID(env, diaJniClass, "cbDiaCreateRequest",
          "(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;ILjava/lang/String;Z)V");
  if (g_diaJniCreateRequest == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaCreateRequest");
  }
  g_diaJniCreateResponse =
      (*env)->GetMethodID(env, diaJniClass, "cbDiaCreateResponse",
          "(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;I)V");
  if (g_diaJniCreateResponse == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaCreateResponse");
  }
  g_diaJniUpdateRequest =
      (*env)->GetMethodID(env, diaJniClass, "cbDiaUpdateRequest",
          "(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;ILjava/lang/String;Z)V");
  if (g_diaJniUpdateRequest == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaUpdateRequest");
  }
  g_diaJniUpdateResponse =
      (*env)->GetMethodID(env, diaJniClass, "cbDiaUpdateResponse",
          "(Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;I)V");
  if (g_diaJniUpdateResponse == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaUpdateResponse");
  }
  g_diaJniDeleteRequest =
      (*env)->GetMethodID(env, diaJniClass, "cbDiaDeleteRequest",
          "(Ljava/lang/String;Ljava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;ILjava/lang/String;Z)V");
  if (g_diaJniDeleteRequest == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaDeleteRequest");
  }
  g_diaJniDeleteResponse = (*env)->GetMethodID(env, diaJniClass,
      "cbDiaDeleteResponse",
      "(Ljava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;I)V");
  if (g_diaJniDeleteResponse == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaDeleteResponse");
  }
  g_diaJniErrorResponse = (*env)->GetMethodID(env, diaJniClass,
      "cbDiaErrorResponse",
      "([BLjava/lang/String;[Lcom/actility/m2m/song/binding/coap/jni/impl/DiaOptImpl;I)V");
  if (g_diaJniErrorResponse == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on cbDiaErrorResponse");
  }
  g_diaJniTraceFunction = (*env)->GetMethodID(env, diaJniClass, "traceFunction",
      "(ILjava/lang/String;)V");
  if (g_diaJniTraceFunction == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on traceFunction");
  }
  g_diaOptName = (*env)->GetFieldID(env, g_diaOptClass, "name",
      "Ljava/lang/String;");
  if (g_diaOptName == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on DiaOptImpl name field");
  }
  g_diaOptVal = (*env)->GetFieldID(env, g_diaOptClass, "val",
      "Ljava/lang/String;");
  if (g_diaOptVal == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass,
        "Cannot get a reference on DiaOptImpl val field");
  }

  return (jint) 0;
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaRun(JNIEnv *env, jobject obj,
    jlong diaPtr)
{
  struct timeval tv;

  dia_context_t *dia = (dia_context_t *) diaPtr;

  dia->run = TRUE;
  while (dia->run)
  {
    dia_nextTimer(dia, &tv);
    rtl_poll(tabpoll, tv.tv_sec * 1000);
  }
  dia_freeContext(dia);

  (*env)->DeleteGlobalRef(env, g_diaJniObj);
  (*env)->DeleteGlobalRef(env, g_diaOptClass);

  g_env = NULL;
  g_diaJniObj = NULL;
  g_diaOptClass = NULL;
  g_diaOptConst = NULL;
  g_diaJniRetrieveRequest = NULL;
  g_diaJniRetrieveResponse = NULL;
  g_diaJniCreateRequest = NULL;
  g_diaJniCreateResponse = NULL;
  g_diaJniUpdateRequest = NULL;
  g_diaJniUpdateResponse = NULL;
  g_diaJniDeleteRequest = NULL;
  g_diaJniDeleteResponse = NULL;
  g_diaJniErrorResponse = NULL;
  g_diaJniTraceFunction = NULL;
  g_diaOptName = NULL;
  g_diaOptVal = NULL;
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaStop(JNIEnv *env, jobject obj,
    jlong diaPtr)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;

  dia->run = FALSE;
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaEnterCS(JNIEnv *env, jobject obj,
    jlong diaPtr)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;

  dia_EnterCS(dia);
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaLeaveCS(JNIEnv *env, jobject obj,
    jlong diaPtr)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;

  dia_LeaveCS(dia);
}

void optionsFromJava(JNIEnv *env, jobjectArray opt_j, jsize len, jstring* str,
    Opt *opt)
{
  opt->name = NULL;
  if (!opt_j)
    return;
  int i;

  for (i = 0; i < len; i++)
  {
    jobject obj = (*env)->GetObjectArrayElement(env, opt_j, i);

    jstring jstr = (*env)->GetObjectField(env, obj, g_diaOptName);
    opt->name = (char *) (*env)->GetStringUTFChars(env, jstr, NULL);
    *str = jstr;
    ++str;

    jstr = (*env)->GetObjectField(env, obj, g_diaOptVal);
    opt->val = (char *) (*env)->GetStringUTFChars(env, jstr, NULL);
    *str = jstr;
    ++str;

    opt++;
  }
  opt->name = NULL;
}

void releaseOptionsFromJava(JNIEnv* env, jsize len, jstring* str, Opt* opt)
{
  int i;
  for (i = 0; i < len; i++)
  {
    (*env)->ReleaseStringUTFChars(env, *str, opt->name);
    ++str;
    (*env)->ReleaseStringUTFChars(env, *str, opt->val);
    ++str;
    ++opt;
  }
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaRetrieveResponse(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring status, jbyteArray content,
    jstring contentType, jobjectArray optHeader, jint tid, jstring peer)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;

  char *status_str =
      status ? (char *) (*env)->GetStringUTFChars(env, status, NULL) : "";
  char *contentType_str =
      contentType ?
          (char *) (*env)->GetStringUTFChars(env, contentType, NULL) : "";
  jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
  jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;
  char *peer_str =
      peer ? (char *) (*env)->GetStringUTFChars(env, peer, NULL) : "";

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);

  diaRetrieveResponse(dia, status_str, body, len, contentType_str, optHeader_C,
      tid, peer_str);

  if (status)
    (*env)->ReleaseStringUTFChars(env, status, status_str);
  if (contentType)
    (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
  if (content)
    (*env)->ReleaseByteArrayElements(env, content, body, JNI_ABORT);
  if (peer)
    (*env)->ReleaseStringUTFChars(env, peer, peer_str);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaCreateResponse(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring status, jstring resourceURI,
    jbyteArray content, jstring contentType, jobjectArray optHeader, jint tid,
    jstring peer)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;

  char *status_str =
      status ? (char *) (*env)->GetStringUTFChars(env, status, NULL) : "";
  char *resourceURI_str =
      resourceURI ?
          (char *) (*env)->GetStringUTFChars(env, resourceURI, NULL) : "";
  char *contentType_str =
      contentType ?
          (char *) (*env)->GetStringUTFChars(env, contentType, NULL) : "";
  jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
  jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;
  char *peer_str =
      peer ? (char *) (*env)->GetStringUTFChars(env, peer, NULL) : "";

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);

  diaCreateResponse(dia, status_str, resourceURI_str, body, len,
      contentType_str, optHeader_C, tid, peer_str);

  if (status)
    (*env)->ReleaseStringUTFChars(env, status, status_str);
  if (resourceURI)
    (*env)->ReleaseStringUTFChars(env, status, resourceURI_str);
  if (contentType)
    (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
  if (content)
    (*env)->ReleaseByteArrayElements(env, content, body, JNI_ABORT);
  if (peer)
    (*env)->ReleaseStringUTFChars(env, peer, peer_str);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaUpdateResponse(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring status, jbyteArray content,
    jstring contentType, jobjectArray optHeader, jint tid, jstring peer)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;

  char *status_str =
      status ? (char *) (*env)->GetStringUTFChars(env, status, NULL) : "";
  char *contentType_str =
      contentType ?
          (char *) (*env)->GetStringUTFChars(env, contentType, NULL) : "";
  jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
  jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;
  char *peer_str =
      peer ? (char *) (*env)->GetStringUTFChars(env, peer, NULL) : "";

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);

  diaUpdateResponse(dia, status_str, body, len, contentType_str, optHeader_C,
      tid, peer_str);

  if (status)
    (*env)->ReleaseStringUTFChars(env, status, status_str);
  if (contentType)
    (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
  if (content)
    (*env)->ReleaseByteArrayElements(env, content, body, JNI_ABORT);
  if (peer)
    (*env)->ReleaseStringUTFChars(env, peer, peer_str);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
}

JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaDeleteResponse(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring status, jobjectArray optHeader, jint tid,
    jstring peer)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;

  char *status_str =
      status ? (char *) (*env)->GetStringUTFChars(env, status, NULL) : "";
  char *peer_str =
      peer ? (char *) (*env)->GetStringUTFChars(env, peer, NULL) : "";

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);

  diaDeleteResponse(dia, status_str, optHeader_C, tid, peer_str);

  if (status)
    (*env)->ReleaseStringUTFChars(env, status, status_str);
  if (peer)
    (*env)->ReleaseStringUTFChars(env, peer, peer_str);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
}

JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaCreateRequest(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring reqEntity, jstring targetID,
    jbyteArray content, jstring contentType, jobjectArray optAttr,
    jobjectArray optHeader)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;
  int tid, ret;

  char *reqEntity_str =
      reqEntity ? (char *) (*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
  char *targetID_str =
      targetID ? (char *) (*env)->GetStringUTFChars(env, targetID, NULL) : "";
  char *contentType_str =
      contentType ?
          (char *) (*env)->GetStringUTFChars(env, contentType, NULL) : "";
  jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
  jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);
  jsize attrLen = 0;
  if (optAttr)
  {
    attrLen = (*env)->GetArrayLength(env, optAttr);
  }
  jstring attrJString[((int) attrLen) * 2];
  Opt optAttr_C[((int) attrLen) + 1];
  optionsFromJava(env, optAttr, attrLen, attrJString, optAttr_C);

  ret = diaCreateRequest(dia, reqEntity_str, targetID_str, body, len,
      contentType_str, optAttr_C, optHeader_C, &tid);

  if (reqEntity)
    (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
  if (targetID)
    (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);
  if (contentType)
    (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
  if (content)
    (*env)->ReleaseByteArrayElements(env, content, body, JNI_ABORT);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
  releaseOptionsFromJava(env, attrLen, attrJString, optAttr_C);

  if (ret < 0)
    return ret;
  return tid;
}

JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaRetrieveRequest(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring reqEntity, jstring targetID,
    jobjectArray optAttr, jobjectArray optHeader)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;
  int tid, ret;

  char *reqEntity_str =
      reqEntity ? (char *) (*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
  char *targetID_str =
      targetID ? (char *) (*env)->GetStringUTFChars(env, targetID, NULL) : "";

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);
  jsize attrLen = 0;
  if (optAttr)
  {
    attrLen = (*env)->GetArrayLength(env, optAttr);
  }
  jstring attrJString[((int) attrLen) * 2];
  Opt optAttr_C[((int) attrLen) + 1];
  optionsFromJava(env, optAttr, attrLen, attrJString, optAttr_C);

  ret = diaRetrieveRequest(dia, reqEntity_str, targetID_str, optAttr_C,
      optHeader_C, &tid);

  if (reqEntity)
    (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
  if (targetID)
    (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
  releaseOptionsFromJava(env, attrLen, attrJString, optAttr_C);

  if (ret < 0)
    return ret;
  return tid;
}

JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaUpdateRequest(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring reqEntity, jstring targetID,
    jbyteArray content, jstring contentType, jobjectArray optAttr,
    jobjectArray optHeader)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;
  int tid, ret;

  char *reqEntity_str =
      reqEntity ? (char *) (*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
  char *targetID_str =
      targetID ? (char *) (*env)->GetStringUTFChars(env, targetID, NULL) : "";
  char *contentType_str =
      contentType ?
          (char *) (*env)->GetStringUTFChars(env, contentType, NULL) : "";
  jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
  jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);
  jsize attrLen = 0;
  if (optAttr)
  {
    attrLen = (*env)->GetArrayLength(env, optAttr);
  }
  jstring attrJString[((int) attrLen) * 2];
  Opt optAttr_C[((int) attrLen) + 1];
  optionsFromJava(env, optAttr, attrLen, attrJString, optAttr_C);

  ret = diaUpdateRequest(dia, reqEntity_str, targetID_str, body, len,
      contentType_str, optAttr_C, optHeader_C, &tid);

  if (reqEntity)
    (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
  if (targetID)
    (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);
  if (contentType)
    (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
  if (content)
    (*env)->ReleaseByteArrayElements(env, content, body, JNI_ABORT);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
  releaseOptionsFromJava(env, attrLen, attrJString, optAttr_C);

  if (ret < 0)
    return ret;
  return tid;
}

JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_coap_jni_impl_DiaJni_diaDeleteRequest(JNIEnv *env,
    jobject obj, jlong diaPtr, jstring reqEntity, jstring targetID,
    jobjectArray optAttr, jobjectArray optHeader)
{
  dia_context_t *dia = (dia_context_t *) diaPtr;
  int tid, ret;

  char *reqEntity_str =
      reqEntity ? (char *) (*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
  char *targetID_str =
      targetID ? (char *) (*env)->GetStringUTFChars(env, targetID, NULL) : "";

  jsize optLen = 0;
  if (optHeader)
  {
    optLen = (*env)->GetArrayLength(env, optHeader);
  }
  jstring optJString[((int) optLen) * 2];
  Opt optHeader_C[((int) optLen) + 1];
  optionsFromJava(env, optHeader, optLen, optJString, optHeader_C);
  jsize attrLen = 0;
  if (optAttr)
  {
    attrLen = (*env)->GetArrayLength(env, optAttr);
  }
  jstring attrJString[((int) attrLen) * 2];
  Opt optAttr_C[((int) attrLen) + 1];
  optionsFromJava(env, optAttr, attrLen, attrJString, optAttr_C);

  ret = diaDeleteRequest(dia, reqEntity_str, targetID_str, optAttr_C,
      optHeader_C, &tid);

  if (reqEntity)
    (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
  if (targetID)
    (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);
  releaseOptionsFromJava(env, optLen, optJString, optHeader_C);
  releaseOptionsFromJava(env, attrLen, attrJString, optAttr_C);

  if (ret < 0)
    return ret;
  return tid;
}
