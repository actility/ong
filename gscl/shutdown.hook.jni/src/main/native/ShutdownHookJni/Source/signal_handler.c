#include <jni.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include "com_actility_m2m_shutdown_hook_jni_impl_JShutdownHook.h"

#define MAX_SEND 100

int serverPort = 0;

//static JavaVM * pJVM = 0;
//static jobject shutdownObj;
/**
 * Call back on signal receive.
 * @param signal received signal
 */
void handleSignal(int signal)
{
//  JNIEnv * pEnv = 0;
//
//  fprintf(stderr, "Signal handled !! (signal=%d)\n", signal);
//  pJVM->AttachCurrentThread((void **)&pEnv, NULL);
//  jclass shutdownClass = pEnv->GetObjectClass(shutdownObj); 
//  pEnv->DeleteGlobalRef(shutdownObj);
//  jmethodID shutdownNow = pEnv->GetStaticMethodID(shutdownClass, "shutdownNow", "()V");
//  pEnv->CallStaticVoidMethod(shutdownClass, shutdownNow);
//  pJVM->DetachCurrentThread();
//  fprintf(stderr, "thread detached from JVM\n");

  fprintf(stderr, "Signal handled !! (signal=%d)\n", signal);
  int socket_connect, sockaddr_len;
  struct sockaddr_in sockaddr_connect;
 
  if ((socket_connect = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("Socket creation error !\n" );
    return ;
  }
 
  sockaddr_connect.sin_family = AF_INET;
  sockaddr_connect.sin_addr.s_addr = inet_addr("127.0.0.1");
  sockaddr_connect.sin_port = htons(serverPort);
 
  sockaddr_len = sizeof(sockaddr_connect);
 
  if (connect(socket_connect, (struct sockaddr*)&sockaddr_connect, sockaddr_len) < 0)
  {
    printf("Socket connection error !\n" );
    return ;
  }
  write(socket_connect, "SHUTDOWN\n", MAX_SEND);
  write(socket_connect, "\n", MAX_SEND);
  close(socket_connect); 
}

/**
 * JNI method invoked from Java to settle the signal handler.
 */
JNIEXPORT void JNICALL Java_com_actility_m2m_shutdown_hook_jni_impl_JShutdownHook_setSignalHandler
  (JNIEnv * pEnv, jobject obj, jint jServerPort) {

//  pEnv->GetJavaVM(&pJVM);
//  shutdownObj = pEnv->NewGlobalRef(obj);

  serverPort = (int) jServerPort;
  signal( SIGINT, handleSignal );
  signal( SIGTERM, handleSignal );
//  signal( SIGQUIT, handleSignal );

}


