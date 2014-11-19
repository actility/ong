#ifdef _BACKUP_


#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <sqlite_driver.h>


#ifndef SWIGEXPORT
#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#if defined(STATIC_LINKED)
#define SWIGEXPORT
#else
#define SWIGEXPORT __declspec(dllexport)
#endif
#else
#if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#define SWIGEXPORT __attribute__ ((visibility("default")))
#else
#define SWIGEXPORT
#endif
#endif
#endif


int backupDb(JNIEnv *env, jobject jobj, SqliteDB *obj,  /* Database to back
                                                        up */
    const char *zFilename,  /* Name of file to back up to */
    int nbPagePerStep, int sleepDelayBetweenStep)
{
  int rc; /* Function return code */
  sqlite3 *pFile; /* Database connection opened on zFilename */
  sqlite3_backup *pBackup; /* Backup handle used to copy data */
  sqlite3 *pDb = obj->db;
  jstring logStr;
  int prg = 0;
  int remaining = 0;
  int pCount = 0;

  jclass cls = (*env)->GetObjectClass(env, jobj);
  jmethodID mid = (*env)->GetMethodID(env, cls, "callback", "(Ljava/lang/String;)V");


  /* Open the database file identified by zFilename. */
  rc = sqlite3_open(zFilename, &pFile);
  if (rc == SQLITE_OK)
  {

    /* Open the sqlite3_backup object used to accomplish the transfer */
    pBackup = sqlite3_backup_init(pFile, "main", pDb, "main");
    if (pBackup)
    {

      /* Each iteration of this loop copies 5 database pages from database
      ** pDb to the backup database. If the return value of backup_step()
      ** indicates that there are still further pages to copy, sleep for
      ** 250 ms before repeating. */
      do
      {
        char buffer[512];
        rc = sqlite3_backup_step(pBackup, nbPagePerStep);
        if (mid != 0)
        {
          remaining = sqlite3_backup_remaining(pBackup);
          pCount = sqlite3_backup_pagecount(pBackup);
          prg = 100 * ((pCount * 1.0 - remaining) / (pCount * 1.0));
          sprintf(buffer, "Completion = %d%%\n", prg);

          logStr = (jstring)(*env)->NewStringUTF(env, buffer);
          (*env)->CallVoidMethod(env, jobj, mid, logStr);
          (*env)->DeleteLocalRef(env, logStr);

        }

        if (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED)
        {
          sqlite3_sleep(sleepDelayBetweenStep);
        }
      }
      while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

      /* Release resources allocated by backup_init(). */
      (void)sqlite3_backup_finish(pBackup);
    }
    rc = sqlite3_errcode(pFile);
  }

  /* Close the database connection opened on database file zFilename
  ** and return the result of this function. */
  (void)sqlite3_close(pFile);
  return rc;
}


SWIGEXPORT jint JNICALL Java_com_actility_storage_driver_sqlite_impl_BackupCommand_backupDb(JNIEnv *jenv, jclass jcls, jlong jarg1, jobject jarg1_, jstring jarg2, jint jarg3, jint jarg4)
{
  jint jresult = 0;
  SqliteDB *arg1 = (SqliteDB *)0;
  char *arg2 = (char *)0;
  int arg3;
  int arg4;
  int result;

  (void)jenv;
  (void)jcls;
  (void)jarg1_;
  arg1 = *(SqliteDB **) &jarg1;
  arg2 = 0;

  if (jarg2)
  {
    arg2 = (char *)(*jenv)->GetStringUTFChars(jenv, jarg2, 0);
    if (!arg2)
    {
      return 0;
    }
  }
  arg3 = (int)jarg3;
  arg4 = (int)jarg4;
  result = (int)backupDb(jenv, jcls, arg1, (char const *)arg2, arg3, arg4);
  jresult = (jint)result;
  if (arg2)
  {
    (*jenv)->ReleaseStringUTFChars(jenv, jarg2, (const char *)arg2);
  }
  return jresult;
}

#endif
