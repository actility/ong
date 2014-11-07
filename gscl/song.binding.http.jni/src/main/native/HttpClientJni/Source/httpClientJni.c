
#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>

#include "rtlbase.h"

#include "common.h"
#include "header_t.h"
#include "response_t.h"
#include "transaction_t.h"
#include "transactionsList_t.h"
#include "freeTransactionsList_t.h"
#include "httpClient_t.h"

#include "com_actility_m2m_song_binding_http_jni_impl_JHttpClients.h"

typedef struct msgInfo_t
{
  char msgBuf[MAX_BUF_SIZE_VL];
  int msgLen;
} msgInfo_t;

static JavaVM *g_vm = NULL;
static int g_vmJniVersion = JNI_VERSION_1_1;
static jobject g_httpClJniObj = NULL;
static jmethodID g_httpClDoTrace = NULL;
static jmethodID g_httpClResponseCb = NULL;

static int g_nbClients = 0;
static int g_nbMaxClients = 0;
static httpClient_t ** g_clients = NULL;
int g_traceLevel = 0;
int g_running = 0;

static pthread_mutex_t clientsMutex;
static pthread_mutex_t traceMutex;

/**
 * The trace function.
 * @param level the log level.
 * @param file the source file from where the function was invoked.
 * @param line the line number in th source file from where the function was invoked.
 * @param msg the message to be logged in.
 */
void trace(int level, char *file, int line, char *msg, ...)
{
  static char message[1024];
  static char message2[1024];
  va_list listArg;
#ifndef _NATIVE_TESTER_
  union
  {
    JNIEnv *jenv;
    void *mem;
  } env;

  (*g_vm)->GetEnv(g_vm, &(env.mem), g_vmJniVersion);
#endif

  pthread_mutex_lock(&traceMutex);

  va_start(listArg, msg);
  vsnprintf(message, sizeof(message), msg, listArg);
  va_end(listArg);

  snprintf(message2, sizeof(message2), "[%s:%d] %s", file, line, message);

#ifdef _NATIVE_TESTER_ 
  printf("%s\n", message2);
#else 
  jstring message_j = (*(env.jenv))->NewStringUTF(env.jenv, message2);
  (*(env.jenv))->CallVoidMethod((env.jenv), g_httpClJniObj, g_httpClDoTrace, level, message_j);
  (*(env.jenv))->DeleteLocalRef((env.jenv), message_j);
#endif

  pthread_mutex_unlock(&traceMutex);
}

/**
 * Invokes the java call back method when a transaction is completed.
 * @param issuerHandle the handler provided when invoking new_httpClient_t. (unused)
 * @param transaction a pointer on the completed transaction. 
 */
static void songBindingHttpResponseCb(void *issuerHandler, transaction_t *transaction)
{
  (void) issuerHandler;
  union
  {
    JNIEnv *jenv;
    void *mem;
  } env;

  // in order not to loose the reference on transaction->jTransaction, store it locally
  jobject jTransaction = transaction->jTransaction;
  (*g_vm)->GetEnv(g_vm, &(env.mem), g_vmJniVersion);
  (*(env.jenv))->CallVoidMethod(env.jenv, g_httpClJniObj, g_httpClResponseCb, 
    jTransaction);
  (*(env.jenv))->DeleteGlobalRef(env.jenv, jTransaction);
}

/**
 * Initialize the HttpClients layer.
 * Need to gather the information for program run (the pointers on the call backs method
 * and the tracing method) and to initialize CURL layer.
 */
JNIEXPORT jboolean JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_httpClientsInit(JNIEnv *env, jobject obj)
{
  int i;

  // do Java/JNI stuff first
  (*env)->GetJavaVM(env, &g_vm);
  g_vmJniVersion = (*env)->GetVersion(env);

  jclass illegalStateExceptionClass = (*env)->FindClass(env, "java/lang/IllegalStateException");
  if (illegalStateExceptionClass == NULL)
  {
    return (jboolean) 0;
  }

  if (obj == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, "JHttpClient object is null");
  }
  else
  {
    g_httpClJniObj = (*env)->NewGlobalRef(env, obj);
    if (g_httpClJniObj == NULL)
    {
      (*env)->ThrowNew(env, illegalStateExceptionClass, "Cannot create global ref from JHttpClient object");
    }
  }

  jclass httpClientClass = (*env)->GetObjectClass(env, /*g_httpClJniObj*/obj);
  if (httpClientClass == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, "Cannot get a reference on JHttpClients class");
  }

  // retrieve pointer on doTrace method
  g_httpClDoTrace = (*env)->GetMethodID(env, httpClientClass, "doTrace", "(ILjava/lang/String;)V");
  if (g_httpClDoTrace == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, "Cannot get a reference on doTrace");
  }

  // retrieve pointer on doResponseCompletion method
  g_httpClResponseCb = (*env)->GetMethodID(env, httpClientClass, "doResponseCompletion", "(Ljava/lang/Object;)V");
  if (g_httpClResponseCb == NULL)
  {
    (*env)->ThrowNew(env, illegalStateExceptionClass, "Cannot get a reference on doResponseCompletion");
  }

  // and then all the initializations for native part
  pthread_mutex_init(&traceMutex, NULL);
  curl_global_init(CURL_GLOBAL_ALL);
  // 5 client is enough for now on.
  pthread_mutex_init(&clientsMutex, NULL);
  g_nbMaxClients = 5;
  g_nbClients = 0;
  if (g_clients)
  {
    for (i = 0; i < g_nbMaxClients; i++)
    {
      if (NULL != g_clients[i])
        g_clients[i]->free(g_clients[i]);
    }
    free(g_clients);
  }
  g_clients = malloc(g_nbMaxClients * sizeof(httpClient_t *));
  memset(g_clients, 0, g_nbMaxClients * sizeof(httpClient_t *));

  LOG(TRACE_INFO, "Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_init done");

  return (jboolean) 1;
}

/**
 * The main loop for all native processings.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_httpClientsRun(JNIEnv * env, jobject obj)
{
  g_running = 1;
  int i;
  httpClient_t *client;
  int nbClients;

  while (g_running)
  {
    i = 0;
    pthread_mutex_lock(&clientsMutex);
    nbClients = g_nbClients;
    pthread_mutex_unlock(&clientsMutex);

    do
    {
      pthread_mutex_lock(&clientsMutex);
      client = g_clients[i];
      pthread_mutex_unlock(&clientsMutex);
      if (client)
      {
        client->processMessages(client);
        client->trackTimedOutMessages(client);
      }
      i++;
    } while ((client) && (i < nbClients));
    //usleep(10000);
  }
}

/**
 * Stops the main loop for all the native processings.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_httpClientsStop(JNIEnv *env, jobject obj)
{
  g_running = 0;
}

/**
 * Change the log level.
 * @param level the new log level to set.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setTraceLevel(JNIEnv *env, jobject obj, jint level)
{
  g_traceLevel = level;
}

/**
 * Create a new HTTP client instance.
 * @param curlDebugEnabled wether the debug mode for CURL layer is enable or not.
 * @return a pointer on the client data structure, or NULL.
 */
JNIEXPORT jlong JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_createHttpClient(JNIEnv *env, jobject jobj,
    jboolean curlDebugEnabled)
{
  int i;
  httpClient_t * client = NULL;
  pthread_mutex_lock(&clientsMutex);
  if (g_nbClients < g_nbMaxClients)
  {
    for (i = 0; i < g_nbMaxClients; i++)
    {
      if (NULL == g_clients[i])
        break;
    }
    if (i >= g_nbMaxClients)
    {
      client = NULL;
    }
    else
    {
      g_clients[i] = new_httpClient_t((short) curlDebugEnabled, songBindingHttpResponseCb, (void *) ((long) i));
      client = g_clients[i];
      g_nbClients++;
    }
  }
  pthread_mutex_unlock(&clientsMutex);
  return (long) client;
}

///////////////////////////////////////////////////////////////////////////////
// Client configuration
/**
 * Set the proxy credentials.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param proxyUsername the user account to connect with.
 * @param proxyPassword the user's password.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setProxyCredentials(JNIEnv *env, jobject obj, jlong clientPtr,
    jstring proxyUsername, jstring proxyPassword)
{
  char szTemp[MAX_BUF_SIZE_B];
  const char *proxyUsernameStr;
  const char *proxyPasswordStr;
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  if (!client)
    return;
  if (!proxyUsername)
    return;

  proxyUsernameStr = (*env)->GetStringUTFChars(env, proxyUsername, 0);
  proxyPasswordStr = (*env)->GetStringUTFChars(env, proxyPassword, 0);
  snprintf(szTemp, MAX_BUF_SIZE_S, "%s:%s", proxyUsernameStr, proxyPasswordStr);
  client->setProxyUserPwd(client, szTemp);

  (*env)->ReleaseStringUTFChars(env, proxyUsername, proxyUsernameStr);
  (*env)->ReleaseStringUTFChars(env, proxyPassword, proxyPasswordStr);
}

/**
 * Set the proxy address.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param proxyHost the proxy host (IP or fqdn)
 * @param proxyPort the proxy port.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setProxy(JNIEnv *env, jobject obj, jlong clientPtr,
    jstring proxyHost, jint proxyPort)
{
  char szTemp[MAX_BUF_SIZE_B];
  const char *proxyHostStr;
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  if (!client)
    return;
  if (!proxyHost)
    return;

  proxyHostStr = (*env)->GetStringUTFChars(env, proxyHost, 0);
  snprintf(szTemp, MAX_BUF_SIZE_S, "%s:%d", proxyHostStr, proxyPort);
  client->setProxyHostPort(client, szTemp);

  (*env)->ReleaseStringUTFChars(env, proxyHost, proxyHostStr);
}

/**
 * Set the maximum number of sockets per host.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param maxSocketsPerHost the limit to set.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setMaxSocketsPerHost(JNIEnv *env, jobject obj, jlong clientPtr,
    jint maxSocketsPerHost)
{
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  if (!client)
    return;

  client->setMaxSocketsPerHost(client, maxSocketsPerHost);
}

/**
 * Set the maximum number of sockets in total.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param maxTotalSockets the limit to set.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setMaxTotalSockets(JNIEnv *env, jobject obj, jlong clientPtr,
    jint maxTotalSockets)
{
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  if (!client)
    return;

  client->setMaxTotalSockets(client, maxTotalSockets);
}

/**
 * Set the "TCP NoDelay" flag.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param tcpNoDelay the flag value (up (1) or down (0)).
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setTcpNoDelay(JNIEnv *env, jobject obj, jlong clientPtr,
    jboolean tcpNoDelay)
{
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  if (!client)
    return;

  if (tcpNoDelay)
  {
    client->setTcpNoDelay(client, 1);
  }
  else
  {
    client->setTcpNoDelay(client, 0);
  }
}

/**
 * Set the request timeout value.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param requestTimeout the request timeout value to set.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setRequestTimeout(JNIEnv *env, jobject obj, jlong clientPtr,
    jlong requestTimeout)
{
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  if (!client)
    return;

  client->setRequestTimeout(client, requestTimeout);
}

/**
 * Set the connection timeout value.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param connectionTimeout the connection timeout value to set.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setConnectionTimeout(JNIEnv * env, jobject obj, jlong clientPtr,
    jlong connectionTimeout)
{
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  if (!client)
    return;

  client->setConnectionTimeout(client, connectionTimeout);
}

/**
 * Log function to be invoked in "dumpInfo" function for a httpClient_t.
 * @param issuerHandle unused here.
 * @param msg the built-in representation for a transaction, to be displayed.
 */
static void dumpInfoClientCb(void *issuerHandle, char *msg, ...)
{
  char info[1024];
  msgInfo_t *message = (msgInfo_t *) issuerHandle;
  va_list listArg;

  va_start(listArg, msg);
  vsnprintf(info, sizeof(info), msg, listArg);
  va_end(listArg);

  if (strlen(info) < MAX_BUF_SIZE_VL - message->msgLen)
  {
    strcat(message->msgBuf, info);
    message->msgLen += strlen(info);
  }
}

/**
 * Dump the pending transactions in the logs.
 */
JNIEXPORT jstring
JNICALL Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_dumpPendingTransactions(JNIEnv *env, jobject obj)
{
  msgInfo_t message;
  int i;
  memset(&message, 0, sizeof(msgInfo_t));

  pthread_mutex_lock(&clientsMutex);
  for (i = 0; i < g_nbMaxClients; i++)
  {
    if (NULL == g_clients[i])
      break;
    dumpInfoClientCb(&message, "client #%d:\n", i);
    g_clients[i]->dumpInfo(g_clients[i], dumpInfoClientCb, &message);
  }
  pthread_mutex_unlock(&clientsMutex);

  return (*env)->NewStringUTF(env, message.msgBuf);
}

///////////////////////////////////////////////////////////////////////////////
// transaction management
/**
 * Ask the C layer to create a transaction_t instance.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param jTransaction the java object that wraps up the C transaction_t data structure.
 * @param transId the identifier of the transaction.
 * @param method the request method (GET, PUT, POST, DELETE)
 * @param url the request URL
 * @return a pointer on the http
 */
JNIEXPORT jlong JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_createTransaction(JNIEnv * env, jobject obj, jlong clientPtr,
    jobject jTransaction, jstring transId, jstring method, jstring url)
{
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  transaction_t *trans = NULL;
  jobject jTransGR;
  char szTemp[255];
  jclass illegalArgumentExceptionClass = NULL;
  const char *transIdStr = NULL;
  const char *methodStr = NULL;
  const char *urlStr = NULL;

  if (!client)
    return 0;
  if (!transId)
    return 0;
  if (!method)
    return 0;
  if (!url)
    return 0;

  transIdStr = (*env)->GetStringUTFChars(env, transId, 0);
  methodStr = (*env)->GetStringUTFChars(env, method, 0);
  urlStr = (*env)->GetStringUTFChars(env, url, 0);

  trans = client->getTransaction(client, (char *) transIdStr);
  if (trans)
  {
    illegalArgumentExceptionClass = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
    if (illegalArgumentExceptionClass == NULL)
    {
      LOG(TRACE_FATAL, "cannot find class java/lang/IllegalArgumentException");
    }
    else
    {
      snprintf(szTemp, sizeof(szTemp), "a transaction already exists with this id %s "
          "(client:0x%p) (%s:%d)", transIdStr, client, __FILE__, __LINE__);
      (*env)->ThrowNew(env, illegalArgumentExceptionClass, szTemp);
    }
  }

  client->createReq(client, (char *) transIdStr);
  trans = client->getTransaction(client, (char *) transIdStr);
  jTransGR = (*env)->NewGlobalRef(env, jTransaction);
  trans->setJTransaction(trans, jTransGR);
  trans->setReqMethod(trans, (char *) methodStr);
  trans->setReqUrl(trans, (char *) urlStr);

  (*env)->ReleaseStringUTFChars(env, transId, transIdStr);
  (*env)->ReleaseStringUTFChars(env, method, methodStr);
  (*env)->ReleaseStringUTFChars(env, url, urlStr);

  return (long) trans;
}

/**
 * Ask the C layer to release a transaction_t instance, and to free all associated resources.
 * @param clientPtr the pointer on the httpClient_t data structure.
 * @param transPtr the pointer on the transaction_t data structure to release.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_releaseTransaction(JNIEnv *env, jobject obj, jlong clientPtr,
    jlong transPtr)
{
  httpClient_t *client = (httpClient_t *) ((long) clientPtr);
  transaction_t *trans = (transaction_t *) ((long) transPtr);

  if (!client)
    return;
  if (!trans)
    return;

  client->removeReq(client, trans->transId);
}

// requests
/**
 * Add a header to the request.
 * @param transPtr the identifier of the transaction.
 * @param name the header name
 * @param value the header value to add.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_addRequestHeader(JNIEnv *env, jobject obj, jlong transPtr,
    jstring name, jstring value)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  const char *nameStr = NULL;
  const char *valueStr = NULL;

  if (!trans)
    return;
  if (!name)
    return;
  if (!value)
    return;

  nameStr = (*env)->GetStringUTFChars(env, name, 0);
  valueStr = (*env)->GetStringUTFChars(env, value, 0);
  trans->addReqHeader(trans, (char *) nameStr, (char *) valueStr);
  (*env)->ReleaseStringUTFChars(env, name, nameStr);
  (*env)->ReleaseStringUTFChars(env, value, valueStr);
}

/**
 * Set the request body.
 * @param transPtr the pointer on the transaction_t data structure.
 * @param content the request body to set.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setRequestBody(JNIEnv *env, jobject obj, jlong transPtr,
    jbyteArray content)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  jsize len;
  jbyte *body = NULL;

  if (trans && content)
  {
    len = (*env)->GetArrayLength(env, content);
    body = (*env)->GetByteArrayElements(env, content, 0);
    trans->setReqBody(trans, (unsigned char *) body, len);
  }
  if (content)
  {
    (*env)->ReleaseByteArrayElements(env, content, body, JNI_ABORT);
  }
}

/**
 * Set the request proxy.
 * @param transPtr the pointer on the transaction_t data structure.
 * @param proxyHost the IP or fqdn of the proxy to target for the request.
 * @param proxyPort the port number to use for connecting the proxy for the request.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_setRequestProxy(JNIEnv *env, jobject obj, jlong transPtr,
    jstring proxyHost, jint proxyPort)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  const char *proxyHostStr = NULL;
  char szTemp[MAX_BUF_SIZE_B];

  if (!trans)
    return;
  if (!proxyHost)
    return;
  proxyHostStr = (*env)->GetStringUTFChars(env, proxyHost, 0);
  snprintf(szTemp, MAX_BUF_SIZE_B, "%s:%d", proxyHostStr, proxyPort);
  trans->setReqProxy(trans, szTemp);
  (*env)->ReleaseStringUTFChars(env, proxyHost, proxyHostStr);
}

/**
 * Ask the C layer to send the request.
 * @param transPtr the pointer on the transaction_t data structure.
 */
JNIEXPORT void JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_sendRequest(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);

  if (!trans)
    return;
  trans->sendReq(trans); // should we throw an exception here if request cannot be sent ??
}

// response
/**
 * Get the status code from the response_t.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the statusCode if any, a negative number otherwise.
 */
JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getResponseStatusCode(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  jint res;

  if (!trans)
  {
    res = -1;
  }
  else if (!trans->response)
  {
    res = -2;
  }
  else
  {
    res = trans->response->getMappedStatusCode(trans->response);
  }

  return res;
}

/**
 * Get the raw status code from the response_t (i.e. the unmapped status code).
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the statusCode if any, a negative number otherwise. Note that the cURL
 * stack will set a status code between 1 to 99 to report errors.
 */
JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getResponseRawStatusCode(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  jint res;

  if (!trans)
  {
    res = -1;
  }
  else if (!trans->response)
  {
    res = -2;
  }
  else
  {
    res = trans->response->statusCode;
  }

  return res;
}

/**
 * Get the reason phrase from the response_t.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the reason phrase if any.
 */
JNIEXPORT jstring JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getResponseReasonPhrase(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  jstring res = NULL;

  if ((trans) && (trans->response) && (trans->response->reasonPhrase))
  {
    res = (*env)->NewStringUTF(env, trans->response->reasonPhrase);
  }

  return res;
}

/**
 * Get the value of response header that matches the provided name.
 * @param transPtr the pointer on the transaction_t data structure.
 * @param name the header name to match.
 * @return the matching header value if found (when multi-valued, all values 
 * concatenated and coma-separated), NULL otherwise.
 */
JNIEXPORT jstring JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getResponseHeader(JNIEnv *env, jobject obj, jlong transPtr,
    jstring name)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  const char *nameStr = NULL;
  header_t *h = NULL;
  jstring res = NULL;

  if (!trans)
    return res;
  if (!trans->response)
    return res;
  if (!name)
    return res;
  nameStr = (*env)->GetStringUTFChars(env, name, 0);
  h = trans->response->getHeader(trans->response, (char *) nameStr);
  if (h)
  {
    res = (*env)->NewStringUTF(env, h->values);
  }
  (*env)->ReleaseStringUTFChars(env, name, nameStr);

  return res;
}

/**
 * Get the name of the Nth header from the response message.
 * @param transPtr the pointer on the transaction_t data structure.
 * @param index the header index in the response header list.
 * @return the header name if found, NULL otherwise.
 */
JNIEXPORT jstring JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getNthResponseHeaderName(JNIEnv *env, jobject obj, jlong transPtr,
    jint index)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  header_t *h = NULL;
  jstring res = NULL;

  if (!trans)
    return res;
  if (!trans->response)
    return res;
  h = trans->response->getNthHeader(trans->response, index);
  if (h)
  {
    res = (*env)->NewStringUTF(env, h->name);
  }

  return res;
}

/**
 * Get the value of the Nth header from the response message.
 * @param transPtr the pointer on the transaction_t data structure.
 * @param index the header index in the response header list.
 * @return the header value if found (when multi-valued, all values 
 * concatenated and coma-separated), NULL otherwise.
 */
JNIEXPORT jstring JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getNthResponseHeaderValue(JNIEnv *env, jobject obj,
    jlong transPtr, jint index)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  header_t *h = NULL;
  jstring res = NULL;

  if (!trans)
    return res;
  if (!trans->response)
    return res;
  h = trans->response->getNthHeader(trans->response, index);
  if (h)
  {
    res = (*env)->NewStringUTF(env, h->values);
  }

  return res;
}

/**
 * Get the length of the response content.
 * This length is computed from the received buffer, but not deduced from 
 * the 'Content-Length' header.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the length of the received content if any, 0 otherwise.
 */
JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getResponseContentLength(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  jint res = 0;

  if ((trans) && (trans->response))
  {
    res = trans->response->contentLength;
  }

  return res;
}

/**
 * Get the response content.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the received content if any, NULL otherwise.
 */
JNIEXPORT jbyteArray JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getResponseBody(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  jbyteArray res = NULL;

  if ((trans) && (trans->response) && (trans->response->content))
  {
    res = (*env)->NewByteArray(env, trans->response->contentLength);
    (*env)->SetByteArrayRegion(env, res, 0, trans->response->contentLength, (jbyte *) trans->response->content);
  }

  return res;
}

/**
 * Get the remote address for the specified transaction.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the remote address used for the transaction.
 */
JNIEXPORT jstring JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getRemoteAddress(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  char *remoteAddr = NULL;
  jstring res = NULL;

  if (trans)
  {
// TEST-ML-20140520
//pthread_mutex_lock(&clientsMutex); // TEST
    curl_easy_getinfo(trans->curlEasyHandle, CURLINFO_PRIMARY_IP, &remoteAddr);
//pthread_mutex_unlock(&clientsMutex); // TEST
    if (remoteAddr && *remoteAddr)
    {
      res = (*env)->NewStringUTF(env, remoteAddr);
    }
  }

  return res;
}

/**
 * Get the remote port for the specified transaction.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the remote port used for the transaction.
 */
JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getRemotePort(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  long remotePort = 0;

  if (trans)
  {
// TEST-ML-20140520
//pthread_mutex_lock(&clientsMutex); // TEST
    curl_easy_getinfo(trans->curlEasyHandle, CURLINFO_PRIMARY_PORT, &remotePort);
//pthread_mutex_unlock(&clientsMutex); // TEST
  }

  return (jlong) remotePort;
}

/**
 * Get the local address for the specified transaction.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the local address used for the transaction.
 */
JNIEXPORT jstring JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getLocalAddress(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  char *localAddr = NULL;
  jstring res = NULL;

  if (trans)
  {
// TEST-ML-20140520
//pthread_mutex_lock(&clientsMutex); // TEST
    curl_easy_getinfo(trans->curlEasyHandle, CURLINFO_LOCAL_IP, &localAddr);
//pthread_mutex_unlock(&clientsMutex); // TEST
    if (localAddr && *localAddr)
    {
      res = (*env)->NewStringUTF(env, localAddr);
    }
  }

  return res;
}

/**
 * Get the local port for the specified transaction.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the local port used for the transaction.
 */
JNIEXPORT jint JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getLocalPort(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  long localPort = 0;

  if (trans)
  {
// TEST-ML-20140520
//pthread_mutex_lock(&clientsMutex); // TEST
    curl_easy_getinfo(trans->curlEasyHandle, CURLINFO_LOCAL_PORT, &localPort);
//pthread_mutex_unlock(&clientsMutex); // TEST
  }

  return (jlong) localPort;
}

/**
 * Get the response protocol extracted from the status line.
 * @param transPtr the pointer on the transaction_t data structure.
 * @return the received protocol if detected, NULL otherwise.
 */
JNIEXPORT jstring JNICALL
Java_com_actility_m2m_song_binding_http_jni_impl_JHttpClients_getProtocol(JNIEnv *env, jobject obj, jlong transPtr)
{
  transaction_t *trans = (transaction_t *) ((long) transPtr);
  jstring res = NULL;

  if ((trans) && (trans->response) && (trans->response->protocol))
  {
    res = (*env)->NewStringUTF(env, trans->response->protocol);
  }

  return res;
}

