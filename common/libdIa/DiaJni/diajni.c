/*
* Copyright (C) Actility, SA. All Rights Reserved.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License version
* 2 only, as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License version 2 for more details (a copy is
* included at /legal/license.txt).
*
* You should have received a copy of the GNU General Public License
* version 2 along with this work; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
* Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
* or visit www.actility.com if you need additional
* information or have any questions.
*/

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

#include "com_actility_servlet_song_coap_DiaJni.h"

#include <rtlbase.h>
#include <dIa.h>

//	XXX
static void		*tabpoll;
static JNIEnv *g_env = NULL;
static jobject g_obj = NULL;
static jclass g_opCls = NULL;

int network_request (void *tab, int fd, void *ref1, void *ref2, int events) {
	return	POLLIN;
}

int network_input (void *tab, int fd, void *ref1, void *ref2, int events) {
	dia_context_t *dia = (dia_context_t *)ref1;
	//	LOCK
	dia_EnterCS(dia);
	dia_input(dia);
	dia_LeaveCS(dia);
	return	0;
}

jobjectArray optionsToJava (JNIEnv *env, Opt *opt) {
	jobjectArray opt_j;
	int	size;
	if	(!opt)		return	NULL;
	if	(!g_opCls)	return	NULL;

	for	(size=0; opt[size].name; size++)
		;
	if	(!size)	return NULL;

	jmethodID mid = (*env)->GetMethodID(env, g_opCls, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
	if	(!mid)	return NULL;

	opt_j = (*env)->NewObjectArray(env, size, g_opCls, NULL);
	if	(!opt_j) return NULL;

	int	i;
	for (i = 0; i < size; i++) {
		jstring name = (*env)->NewStringUTF(env, opt[i].name);
		jstring val = (*env)->NewStringUTF(env, opt[i].val);
		jobject obj = (*env)->NewObject(env, g_opCls, mid, name, val);
		(*env)->SetObjectArrayElement(env, opt_j, i, obj);
		(*env)->DeleteLocalRef(env, obj);
	}
	return	opt_j;
}

int cb_diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaRetrieveRequest",
		"(Ljava/lang/String;Ljava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;[Lcom/actility/servlet/song/coap/DiaOpt;ILjava/lang/String;)V");
	if	(!mid) {
		rtl_trace (1, "Unable to find JNI method cb_diaRetrieveRequest\n");
		return 0;
	}

	jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
	jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
	jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
	jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, reqEntity_j, targetID_j, optAttr_j, optHeader_j, tid, peer_j);

	(*g_env)->DeleteLocalRef(g_env, reqEntity_j);
	(*g_env)->DeleteLocalRef(g_env, targetID_j);
	if	(optAttr_j) (*g_env)->DeleteLocalRef(g_env, optAttr_j);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
	if	(peer_j) (*g_env)->DeleteLocalRef(g_env, peer_j);

	return	0;
}

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaRetrieveResponse",
		"(Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;I)V");
	if	(!mid) return;

	jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
	jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
	jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
	(*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *)ctt);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, scode_j, bytes, cttType_j, optHeader_j, tid);

	(*g_env)->DeleteLocalRef(g_env, scode_j);
	(*g_env)->DeleteLocalRef(g_env, cttType_j);
	(*g_env)->DeleteLocalRef(g_env, bytes);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaCreateRequest",
		"(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;[Lcom/actility/servlet/song/coap/DiaOpt;ILjava/lang/String;)V");
	if	(!mid) {
		rtl_trace (1, "Unable to find JNI method cb_diaCreateRequest\n");
		return	0;
	}

	jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
	jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
	jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
	(*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *)ctt);
	jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
	jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
	jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, reqEntity_j, targetID_j, bytes, cttType_j, optAttr_j, optHeader_j, tid, peer_j);

	(*g_env)->DeleteLocalRef(g_env, reqEntity_j);
	(*g_env)->DeleteLocalRef(g_env, targetID_j);
	(*g_env)->DeleteLocalRef(g_env, cttType_j);
	(*g_env)->DeleteLocalRef(g_env, bytes);
	if	(optAttr_j) (*g_env)->DeleteLocalRef(g_env, optAttr_j);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
	if	(peer_j) (*g_env)->DeleteLocalRef(g_env, peer_j);
	return	0;
}

void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaCreateResponse",
		"(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;I)V");
	if	(!mid)	return;

	jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
	jstring resourceURI_j = (*g_env)->NewStringUTF(g_env, resourceURI);
	jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
	(*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *)ctt);
	jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, scode_j, resourceURI_j, bytes, cttType_j, optHeader_j, tid);

	(*g_env)->DeleteLocalRef(g_env, scode_j);
	(*g_env)->DeleteLocalRef(g_env, resourceURI_j);
	(*g_env)->DeleteLocalRef(g_env, cttType_j);
	(*g_env)->DeleteLocalRef(g_env, bytes);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

int cb_diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaUpdateRequest",
		"(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;[Lcom/actility/servlet/song/coap/DiaOpt;ILjava/lang/String;)V");
	if	(!mid) {
		rtl_trace (1, "Unable to find JNI method cb_diaUpdateRequest\n");
		return 0;
	}

	jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
	jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
	jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
	(*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *)ctt);
	jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
	jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
	jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, reqEntity_j, targetID_j, bytes, cttType_j, optAttr_j, optHeader_j, tid, peer_j);

	(*g_env)->DeleteLocalRef(g_env, reqEntity_j);
	(*g_env)->DeleteLocalRef(g_env, targetID_j);
	(*g_env)->DeleteLocalRef(g_env, cttType_j);
	(*g_env)->DeleteLocalRef(g_env, bytes);
	if	(optAttr_j) (*g_env)->DeleteLocalRef(g_env, optAttr_j);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
	if	(peer_j) (*g_env)->DeleteLocalRef(g_env, peer_j);
	return	0;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaUpdateResponse",
		"(Ljava/lang/String;[BLjava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;I)V");
	if	(!mid)	return;

	jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
	jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
	(*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *)ctt);
	jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, scode_j, bytes, cttType_j, optHeader_j, tid);

	(*g_env)->DeleteLocalRef(g_env, scode_j);
	(*g_env)->DeleteLocalRef(g_env, cttType_j);
	(*g_env)->DeleteLocalRef(g_env, bytes);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaDeleteRequest",
		"(Ljava/lang/String;Ljava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;[Lcom/actility/servlet/song/coap/DiaOpt;ILjava/lang/String;)V");
	if	(!mid) {
		rtl_trace (1, "Unable to find JNI method cb_diaDeleteRequest\n");
		return 0;
	}

	jstring reqEntity_j = (*g_env)->NewStringUTF(g_env, reqEntity);
	jstring targetID_j = (*g_env)->NewStringUTF(g_env, targetID);
	jobjectArray optAttr_j = optionsToJava(g_env, optAttr);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);
	jstring peer_j = (*g_env)->NewStringUTF(g_env, peer);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, reqEntity_j, targetID_j, optAttr_j, optHeader_j, tid, peer_j);

	(*g_env)->DeleteLocalRef(g_env, reqEntity_j);
	(*g_env)->DeleteLocalRef(g_env, targetID_j);
	if	(optAttr_j) (*g_env)->DeleteLocalRef(g_env, optAttr_j);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
	if	(peer_j) (*g_env)->DeleteLocalRef(g_env, peer_j);
	return	0;
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaDeleteResponse",
		"(Ljava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;I)V");
	if	(!mid)	return;

	jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, scode_j, optHeader_j, tid);

	(*g_env)->DeleteLocalRef(g_env, scode_j);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"cb_diaErrorResponse",
		"([BLjava/lang/String;[Lcom/actility/servlet/song/coap/DiaOpt;I)V");
	if	(!mid)	return;

//	jstring scode_j = (*g_env)->NewStringUTF(g_env, scode);
	jbyteArray bytes = (*g_env)->NewByteArray(g_env, len);
	(*g_env)->SetByteArrayRegion(g_env, bytes, 0, len, (jbyte *)ctt);
	jstring cttType_j = (*g_env)->NewStringUTF(g_env, cttType);
	jobjectArray optHeader_j = optionsToJava(g_env, optHeader);

	(*g_env)->CallVoidMethod(g_env, g_obj, mid, bytes, cttType_j, optHeader_j, tid);

//	(*g_env)->DeleteLocalRef(g_env, scode_j);
	(*g_env)->DeleteLocalRef(g_env, cttType_j);
	(*g_env)->DeleteLocalRef(g_env, bytes);
	if	(optHeader_j) (*g_env)->DeleteLocalRef(g_env, optHeader_j);
}

void _traceFunction(int level, char *message) {
	if	(!g_env)
		return;
	jclass cls = (*g_env)->GetObjectClass(g_env, g_obj);
	if	(!cls)
		return;
	jmethodID mid = (*g_env)->GetMethodID(g_env, cls,
		"_traceFunction",
		"(ILjava/lang/String;)V");
	if	(!mid)	return;

	jstring message_j = (*g_env)->NewStringUTF(g_env, message);
	(*g_env)->CallVoidMethod(g_env, g_obj, mid, level, message_j);
	(*g_env)->DeleteLocalRef(g_env, message_j);
}

dia_callbacks_t callbacks = {
	cb_diaRetrieveRequest,
	cb_diaRetrieveResponse,
	cb_diaCreateRequest,
	cb_diaCreateResponse,
	cb_diaUpdateRequest,
	cb_diaUpdateResponse,
	cb_diaDeleteRequest,
	cb_diaDeleteResponse,
	cb_diaErrorResponse
};


JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaInit(JNIEnv *env, jobject obj, jstring host, jstring port) {
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia;
	char *host_str = host ? (char *)(*env)->GetStringUTFChars(env, host, NULL) : "::";
	char *port_str = port ? (char *)(*env)->GetStringUTFChars(env, port, NULL) : "5683";

	rtl_tracemutex ();
	dia_set_log_level (9);
	rtl_tracerotate ("/tmp/DiaJni");

	dia = dia_createContext(host_str, port_str, &callbacks);
	if (!dia) return;

	//	Save dia pointer in java field 'int diaptr'
	(*env)->SetIntField(env, obj, fid, (int)dia);

	dia_setNonBlocking (dia, 20);

	tabpoll = rtl_pollInit ();
	rtl_pollAdd (tabpoll, dia_getFd(dia), network_input, network_request, dia, NULL);

	if (host) (*env)->ReleaseStringUTFChars(env, host, host_str);
	if (port) (*env)->ReleaseStringUTFChars(env, port, port_str);
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaTrace(JNIEnv *env, jobject obj, jstring level, jstring files, jstring sizemax) {
	char *level_str = level ? (char *)(*env)->GetStringUTFChars(env, level, NULL) : NULL;
	char *files_str = files ? (char *)(*env)->GetStringUTFChars(env, files, NULL) : NULL;
	char *sizemax_str = sizemax ? (char *)(*env)->GetStringUTFChars(env, sizemax, NULL) : NULL;

	rtl_tracemutex ();
	dia_setTraceFunction (_traceFunction);

	if	(level_str)	dia_set_log_level (atoi(level_str));
	if	(files_str)	rtl_tracerotate (files_str);
	if	(sizemax_str)	rtl_tracesizemax (atoi(sizemax_str));

	if (level) (*env)->ReleaseStringUTFChars(env, level, level_str);
	if (files) (*env)->ReleaseStringUTFChars(env, files, files_str);
	if (sizemax) (*env)->ReleaseStringUTFChars(env, sizemax, sizemax_str);
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaPoll(JNIEnv *env, jobject obj) {
	struct timeval tv;
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	g_env	= env;
	g_obj	= obj;
	g_opCls = (*env)->FindClass(env, "com/actility/servlet/song/coap/DiaOpt");

	dia_nextTimer (dia, &tv);
	rtl_poll(tabpoll, tv.tv_sec * 1000);
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaRun(JNIEnv *env, jobject obj) {
	struct timeval tv;
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	g_env	= env;
	g_obj	= obj;
	g_opCls = (*env)->FindClass(env, "com/actility/servlet/song/coap/DiaOpt");

	dia->run	= TRUE;
	while	(dia->run) {
		dia_nextTimer (dia, &tv);
		rtl_poll(tabpoll, tv.tv_sec * 1000);
	}
	dia_freeContext(dia);
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaStop(JNIEnv *env, jobject obj) {
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	dia->run	= FALSE;
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaFree(JNIEnv *env, jobject obj) {
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	dia_freeContext(dia);
}


void optionsFromJava (JNIEnv *env, jobjectArray opt_j, Opt *opt) {
	opt->name	= NULL;
	if	(!opt_j)	return;
	if	(!g_opCls)	return;
	jsize len = (*env)->GetArrayLength(env, opt_j);
	int	i;
	jfieldID fid_name = (*env)->GetFieldID(env, g_opCls, "name", "Ljava/lang/String;");
	jfieldID fid_val = (*env)->GetFieldID(env, g_opCls, "val", "Ljava/lang/String;");

	for	(i=0; i<len; i++) {
		jobject obj = (*env)->GetObjectArrayElement(env, opt_j, i);

		jstring jstr = (*env)->GetObjectField(env, obj, fid_name);
		opt->name = (char *)(*env)->GetStringUTFChars(env, jstr, NULL);
		// (*env)->ReleaseStringUTFChars(env, jstr, str);

		jstr = (*env)->GetObjectField(env, obj, fid_val);
		opt->val = (char *)(*env)->GetStringUTFChars(env, jstr, NULL);
		// (*env)->ReleaseStringUTFChars(env, jstr, str);

		opt	++;
	}
	opt->name	= NULL;
}


JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaRetrieveResponse(JNIEnv *env, jobject obj, jstring status, jbyteArray content,
	jstring contentType, jobjectArray optHeader, jint tid, jstring peer)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	char *status_str = status ? (char *)(*env)->GetStringUTFChars(env, status, NULL) : "";
	char *contentType_str = contentType ? (char *)(*env)->GetStringUTFChars(env, contentType, NULL) : "";
	jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
	jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;
	char *peer_str = peer ? (char *)(*env)->GetStringUTFChars(env, peer, NULL) : "";

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);

	dia_EnterCS(dia);
	diaRetrieveResponse(dia, status_str, body, len, contentType_str, optHeader_C, tid, peer_str);
	dia_LeaveCS(dia);

	if (status) (*env)->ReleaseStringUTFChars(env, status, status_str);
	if (contentType) (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
	if (content) (*env)->ReleaseByteArrayElements(env, content, body, 0);
	if (peer) (*env)->ReleaseStringUTFChars(env, peer, peer_str);
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaCreateResponse
  (JNIEnv *env, jobject obj, jstring status, jstring resourceURI, jbyteArray content, jstring contentType, jobjectArray optHeader, jint tid, jstring peer)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	char *status_str = status ? (char *)(*env)->GetStringUTFChars(env, status, NULL) : "";
	char *resourceURI_str = resourceURI ? (char *)(*env)->GetStringUTFChars(env, resourceURI, NULL) : "";
	char *contentType_str = contentType ? (char *)(*env)->GetStringUTFChars(env, contentType, NULL) : "";
	jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
	jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;
	char *peer_str = peer ? (char *)(*env)->GetStringUTFChars(env, peer, NULL) : "";

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);

	dia_EnterCS(dia);
	diaCreateResponse(dia, status_str, resourceURI_str, body, len, contentType_str, optHeader_C, tid, peer_str);
	dia_LeaveCS(dia);

	if (status) (*env)->ReleaseStringUTFChars(env, status, status_str);
	if (resourceURI) (*env)->ReleaseStringUTFChars(env, status, resourceURI_str);
	if (contentType) (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
	if (content) (*env)->ReleaseByteArrayElements(env, content, body, 0);
	if (peer) (*env)->ReleaseStringUTFChars(env, peer, peer_str);
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaUpdateResponse
  (JNIEnv *env, jobject obj, jstring status, jbyteArray content, jstring contentType, jobjectArray optHeader, jint tid, jstring peer)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	char *status_str = status ? (char *)(*env)->GetStringUTFChars(env, status, NULL) : "";
	char *contentType_str = contentType ? (char *)(*env)->GetStringUTFChars(env, contentType, NULL) : "";
	jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
	jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;
	char *peer_str = peer ? (char *)(*env)->GetStringUTFChars(env, peer, NULL) : "";

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);

	dia_EnterCS(dia);
	diaUpdateResponse(dia, status_str, body, len, contentType_str, optHeader_C, tid, peer_str);
	dia_LeaveCS(dia);

	if (status) (*env)->ReleaseStringUTFChars(env, status, status_str);
	if (contentType) (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
	if (content) (*env)->ReleaseByteArrayElements(env, content, body, 0);
	if (peer) (*env)->ReleaseStringUTFChars(env, peer, peer_str);
}

JNIEXPORT void JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaDeleteResponse
  (JNIEnv *env, jobject obj, jstring status, jobjectArray optHeader, jint tid, jstring peer)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);

	char *status_str = status ? (char *)(*env)->GetStringUTFChars(env, status, NULL) : "";
	char *peer_str = peer ? (char *)(*env)->GetStringUTFChars(env, peer, NULL) : "";

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);

	dia_EnterCS(dia);
	diaDeleteResponse(dia, status_str, optHeader_C, tid, peer_str);
	dia_LeaveCS(dia);

	if (status) (*env)->ReleaseStringUTFChars(env, status, status_str);
	if (peer) (*env)->ReleaseStringUTFChars(env, peer, peer_str);
}

JNIEXPORT jint JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaCreateRequest
  (JNIEnv *env, jobject obj, jstring reqEntity, jstring targetID, jbyteArray content, jstring contentType, jobjectArray optAttr, jobjectArray optHeader)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);
	int	tid, ret;

	char *reqEntity_str = reqEntity ? (char *)(*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
	char *targetID_str = targetID ? (char *)(*env)->GetStringUTFChars(env, targetID, NULL) : "";
	char *contentType_str = contentType ? (char *)(*env)->GetStringUTFChars(env, contentType, NULL) : "";
	jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
	jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);
	Opt optAttr_C[20];
	optionsFromJava (env, optAttr, optAttr_C);

	dia_EnterCS(dia);
	ret = diaCreateRequest(dia, reqEntity_str, targetID_str, body, len, contentType_str, optAttr_C, optHeader_C, &tid);
	dia_LeaveCS(dia);

	if (reqEntity) (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
	if (targetID) (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);
	if (contentType) (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
	if (content) (*env)->ReleaseByteArrayElements(env, content, body, 0);

	if	(ret < 0)
		return	ret;
	return	tid;
}

JNIEXPORT jint JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaRetrieveRequest
  (JNIEnv *env, jobject obj, jstring reqEntity, jstring targetID, jobjectArray optAttr, jobjectArray optHeader)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);
	int	tid, ret;

	char *reqEntity_str = reqEntity ? (char *)(*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
	char *targetID_str = targetID ? (char *)(*env)->GetStringUTFChars(env, targetID, NULL) : "";

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);
	Opt optAttr_C[20];
	optionsFromJava (env, optAttr, optAttr_C);

	dia_EnterCS(dia);
	ret = diaRetrieveRequest(dia, reqEntity_str, targetID_str, optAttr_C, optHeader_C, &tid);
	dia_LeaveCS(dia);

	if (reqEntity) (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
	if (targetID) (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);

	if	(ret < 0)
		return	ret;
	return	tid;
}

JNIEXPORT jint JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaUpdateRequest
  (JNIEnv *env, jobject obj, jstring reqEntity, jstring targetID, jbyteArray content, jstring contentType, jobjectArray optAttr, jobjectArray optHeader)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);
	int	tid, ret;

	char *reqEntity_str = reqEntity ? (char *)(*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
	char *targetID_str = targetID ? (char *)(*env)->GetStringUTFChars(env, targetID, NULL) : "";
	char *contentType_str = contentType ? (char *)(*env)->GetStringUTFChars(env, contentType, NULL) : "";
	jsize len = content ? (*env)->GetArrayLength(env, content) : 0;
	jbyte *body = content ? (*env)->GetByteArrayElements(env, content, 0) : NULL;

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);
	Opt optAttr_C[20];
	optionsFromJava (env, optAttr, optAttr_C);

	dia_EnterCS(dia);
	ret = diaUpdateRequest(dia, reqEntity_str, targetID_str, body, len, contentType_str, optAttr_C, optHeader_C, &tid);
	dia_LeaveCS(dia);

	if (reqEntity) (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
	if (targetID) (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);
	if (contentType) (*env)->ReleaseStringUTFChars(env, contentType, contentType_str);
	if (content) (*env)->ReleaseByteArrayElements(env, content, body, 0);

	if	(ret < 0)
		return	ret;
	return	tid;
}

JNIEXPORT jint JNICALL
Java_com_actility_servlet_song_coap_DiaJni_diaDeleteRequest
  (JNIEnv *env, jobject obj, jstring reqEntity, jstring targetID, jobjectArray optAttr, jobjectArray optHeader)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jfieldID fid = (*env)->GetFieldID(env, cls, "diaptr", "I");
	dia_context_t	*dia = (dia_context_t *)(*env)->GetIntField(env, obj, fid);
	int	tid, ret;

	char *reqEntity_str = reqEntity ? (char *)(*env)->GetStringUTFChars(env, reqEntity, NULL) : "";
	char *targetID_str = targetID ? (char *)(*env)->GetStringUTFChars(env, targetID, NULL) : "";

	Opt optHeader_C[20];
	optionsFromJava (env, optHeader, optHeader_C);
	Opt optAttr_C[20];
	optionsFromJava (env, optAttr, optAttr_C);

	dia_EnterCS(dia);
	ret = diaDeleteRequest(dia, reqEntity_str, targetID_str, optAttr_C, optHeader_C, &tid);
	dia_LeaveCS(dia);

	if (reqEntity) (*env)->ReleaseStringUTFChars(env, reqEntity, reqEntity_str);
	if (targetID) (*env)->ReleaseStringUTFChars(env, targetID, targetID_str);

	if	(ret < 0)
		return	ret;
	return	tid;
}
