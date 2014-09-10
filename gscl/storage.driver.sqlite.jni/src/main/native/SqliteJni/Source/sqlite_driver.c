#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "sqlite3.h"
#include "sqlite_driver.h"
#include "jni.h"
//#include "vld.h"
#include <sys/stat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

static long minDurationBetweenVaccums = 60000L; // 60s
static int maxOpsBeforeVacuum = 500;
static long maxDurationBeforeVacuum = 86400000L; // 1d
static int maxDBUsagePourcentageBeforeVacuum = 90;
static long minDurationBetweenDBUsageComputation = 60000L; // 60s

static int maxPageCount = 0;
static int opsCount = 0;
static long lastVacuum = 0L;
static long lastDbUsageComputation = 0L;

////////////////////////////////////////////////////////////////////
// ML PATCH: add error log in a separate log stream - 2013-09-04
#define MAX_LOG_SIZE 51200 // 50 Ko
static int iLogLevel = 0;
static FILE *pErrLogFile = NULL;
static long lErrLogFileSize = 0;
static char szErrFileName[] = "storage.driver.sqlite-err";

// prototypes
static int isDirectory(char * rootPath, char *szPath);
static char *getLogDirectory();
static void rotate();
static void openErrFile();
static void traceErr(const char *szMessage, ...);
static void startErrLogFile();
static void stopErrLogFile();

/**
 * Indicate if a path refers to a directory.
 * The path to look for is made of the concatenation of rootPath and szPath.
 * @param rootPath the first part of the path to look for.
 * @param szPath the second part of the path to look for.
 * @return 1 if the path refers to a directory, 0 otherwise.
 */
static int isDirectory(char * rootPath, char *szPath)
{
#ifdef _DEBUG_TRACE_
  fprintf(stderr, "isDirectory\n");
#endif
  char szTemp[255];
  DIR *rep;

  sprintf(szTemp, "%s/%s", rootPath, szPath);

  rep = opendir(szTemp);
  if (rep == NULL)
  {
    return 0;
  }
  closedir(rep);
  return 1;
}

/**
 * Detect the log directory for the SCL.
 * @return the path to the SCL log directory.
 */
static char *getLogDirectory()
{
#ifdef _DEBUG_TRACE_
  fprintf(stderr, "getLogDirectory\n");
#endif
  char *rootact;
  static char szErrFullFileName[255];

  rootact = getenv("ROOTACT");
  if ((rootact && *rootact))
  {
    if (isDirectory(rootact, "var/log/nscl"))
    {
      sprintf(szErrFullFileName, "%s/var/log/nscl", rootact);
    }
    else if (isDirectory(rootact, "var/log/gscl"))
    {
      sprintf(szErrFullFileName, "%s/var/log/gscl", rootact);
    }
    else
    {
      sprintf(szErrFullFileName, ".");
    }
  }
  else
  {
    sprintf(szErrFullFileName, ".");
  }

  return szErrFullFileName;
}

/**
 * Do rotate the current log file.
 * It renames the opened one from ..._0.log to ..._1.log.
 */
static void rotate()
{
#ifdef _DEBUG_TRACE_
  fprintf(stderr, "rotate\n");
#endif
  char szErrLogFullFilename0[255];
  char szErrLogFullFilename1[255];

  stopErrLogFile();
  sprintf(szErrLogFullFilename0, "%s/%s_0.log", getLogDirectory(),
      szErrFileName);
  sprintf(szErrLogFullFilename1, "%s/%s_1.log", getLogDirectory(),
      szErrFileName);
  remove(szErrLogFullFilename1);
  rename(szErrLogFullFilename0, szErrLogFullFilename1);
}

/**
 * Open the ..._0.log file.
 * It opens the file for appending.
 */
static void openErrFile()
{
#ifdef _DEBUG_TRACE_
  fprintf(stderr, "openErrFile\n");
#endif
  char szErrLogFullFilename[255];
  struct stat fileStat;

  sprintf(szErrLogFullFilename, "%s/%s_0.log", getLogDirectory(),
      szErrFileName);
  if (0 == stat(szErrLogFullFilename, &fileStat))
  {
    // File exists
    if (fileStat.st_size < MAX_LOG_SIZE)
    {
      lErrLogFileSize = fileStat.st_size;
    }
    else
    {
      // move existing file to _1.log,
      rotate();
      // and restart new file
      lErrLogFileSize = 0;
    }
  }
  else
  {
    lErrLogFileSize = 0;
  }

  pErrLogFile = fopen(szErrLogFullFilename, "a");
}

/**
 * Do log a message in the log file.
 * @param szMessage the message to log in the file.
 * @param vargs the arguments.
 */
static void traceErr(const char *szMessage, ...)
{
#ifdef _DEBUG_TRACE_
  fprintf(stderr, "traceErr\n");
#endif

  static char szFinalMsg[1280];
  static char szLogMessage[1024];
  static char szDateTime[128];
  struct timeval tp;
  struct tm *tmtime;

  if (!iLogLevel)
    return;
  if (!pErrLogFile)
    return;

  va_list marker;
  va_start(marker, szMessage);

  gettimeofday(&tp, NULL);
  tmtime = localtime((time_t *) &tp.tv_sec);
  sprintf(szDateTime, "%02d%02d-%02d%02d%02d.%03d", tmtime->tm_mon + 1,
      tmtime->tm_mday, tmtime->tm_hour, tmtime->tm_min, tmtime->tm_sec,
      (int) tp.tv_usec / 1000);

  vsnprintf(szLogMessage, sizeof(szLogMessage), szMessage, marker);
  va_end(marker);

  snprintf(szFinalMsg, sizeof(szFinalMsg), "[0x%4.4x]  %s  %s\n",
      (unsigned int) pthread_self(), szDateTime, szLogMessage);

  if (lErrLogFileSize >= MAX_LOG_SIZE)
  {
    rotate();
    openErrFile();
  }

  lErrLogFileSize += fwrite(szFinalMsg, sizeof(char), strlen(szFinalMsg),
      pErrLogFile);
  fflush(pErrLogFile);
}

/**
 * Start the log stream.
 */
static void startErrLogFile()
{
#ifdef _DEBUG_TRACE_
  fprintf(stderr, "startErrLogFile\n");
#endif
  stopErrLogFile();
  openErrFile();
  traceErr("############## Starting Err trace ################");
}

/**
 * Stop the log stream.
 */
static void stopErrLogFile()
{
#ifdef _DEBUG_TRACE_
  fprintf(stderr, "stopErrLogFile\n");
#endif
  if (pErrLogFile)
  {
    fclose(pErrLogFile);
  }
  pErrLogFile = NULL;
}

/**
 * The callback method for sqlite3_trace.
 */
void traceSqlStatementCb(void * context, const char *msg)
{
  traceErr(msg);
}

/**
 * Change the log level.
 * For now on, the tracefor this driver are in a dedicated stream, and are enabled if level
 * is not set to zero.
 * @param level the log level to enable.
 * @param obj the SqliteDB instance.
 */
void setLogLevel(int level, SqliteDB *obj)
{
  if (level)
  {
    iLogLevel = level;
    traceErr("############## enabling err trace ################");
    sqlite3_trace(obj->db, traceSqlStatementCb, NULL);
  }
  else
  {
    // this will be logged only if logs were enabled before.
    traceErr("############## disabling err trace ################");
    // disable it
    sqlite3_trace(obj->db, NULL, NULL);
    iLogLevel = level;
  }
}

////////////////////////////////////////////////////////////////////

char *getSearchSqlText(char *path, int scope, InternalCondition *condition,
    int order, int limit);
int deleteAllAttributesInternal(SqliteDB *obj, sqlite3_int64 document_id);

static void clear_error(SqliteDB *obj)
{
  if (obj && obj->last_error)
  {
    obj->last_error = 0;
  }
}

SqliteDB *SqliteOpen(const char *fname)
{
  //    const char * err;
  SqliteDB *obj = (SqliteDB *) malloc(sizeof(SqliteDB));
  if (!obj)
  {
    return 0;
  }

  obj->last_error = sqlite3_open(fname, &obj->db);
  if (obj->last_error != SQLITE_OK)
  {
    printf("Open error: %s\n", sqlite3_errmsg(obj->db));
  }
  return obj;
}

void SqliteClose(SqliteDB *obj)
{
  if (obj)
  {
    clear_error(obj);
    if (obj->db)
    {
      sqlite3_close(obj->db);
    }
    obj->db = NULL;
    free(obj);
    obj = NULL;
  }

}

int SqliteIsValid(const SqliteDB *obj)
{
  return obj && obj->db;
}

// private function to update version in DB
int _updateVersion(SqliteDB *obj, int major, int minor)
{
  int r;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, "UPDATE VERSION SET MAJOR=:?, MINOR = :?", -1,
      &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return r;
  }
  r = sqlite3_bind_int(stmt, 1, major);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return r;
  }
  r = sqlite3_bind_int(stmt, 2, minor);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return r;
  }
  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  return r;
}

long getCurrentTime()
{
  // Get current time
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  return spec.tv_sec * 1000L + (long) (spec.tv_nsec / 1.0e6);
}

int performVacuum(SqliteDB *obj)
{
  int r = sqlite3_exec(obj->db, "vacuum", 0, 0, 0);
  traceErr("vacuum performed %d", r);
  opsCount = 0;
  lastVacuum = getCurrentTime();
  return r;
}

// set pragma option for db and make vacuum
int activate(SqliteDB *obj)
{
  startErrLogFile();

  int r;
  r = sqlite3_exec(obj->db, "pragma auto_vacuum = 0", 0, 0, 0);
  traceErr("pragma auto_vacuum %d\n", r);
  if (r == SQLITE_OK)
  {
    r = performVacuum(obj);
  }

  return r;
}

// execute raw sql string. Use it in DB creatation
int executeSqlStatementWithOutParameters(SqliteDB *obj, char *sql)
{
  return sqlite3_exec(obj->db, sql, 0, 0, 0);
}

// check db version with major and minor,
//if  major version in DB is equal to %major% and %minor% is not equal to minor in DB, should update version to this
// return -1 if more as one record found in table
// return -2 if major version is not equal to our version
int checkDBVersion(SqliteDB *obj, int major, int minor)
{
  int r;
  int v1, v2;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, "SELECT MAJOR, MINOR FROM VERSION", -1, &stmt,
      &tail);
  if (r != SQLITE_OK)
  {
    return r;
  }

  r = sqlite3_step(stmt);
  // no records for version, update DB to this version
  if (r == SQLITE_DONE)
  {
    r = _updateVersion(obj, major, minor);
  }
  else if (r == SQLITE_ROW)
  {
    v1 = sqlite3_column_int(stmt, 0);
    v2 = sqlite3_column_int(stmt, 1);
    r = sqlite3_step(stmt);
    if (r == SQLITE_ROW)
    {
      r = -1; // if find more as one record, return -1
    }
    else // only one record found, all is OK, check version
    {
      if (major != v1)
      {
        r = -2;
      }
      else if (v2 != minor)
      {
        r = _updateVersion(obj, major, minor);
      }
    }
  }

  sqlite3_finalize(stmt);
  return r;
}

int deactivate(SqliteDB *obj)
{
  stopErrLogFile();
  return 0;
}

// append char to end of string
char *straddchr(char *str, int c)
{
  size_t len = str ? 2 + strlen(str) : 2;
  char *p = (char *) realloc(str, len); /* add space (need space for char and \0) */
  if (!p)
  {
    return str;
  }
  str = p;
  str[len - 2] = c;
  str[len - 1] = 0;
  return str;
}

void *realloc_zero(void *pBuffer, size_t oldSize, size_t newSize)
{
  void *pNew = realloc(pBuffer, newSize);
  if (newSize > oldSize && pNew)
  {
    size_t diff = newSize - oldSize;
    void *pStart = ((char *) pNew) + oldSize;
    memset(pStart, 0, diff);
  }
  return pNew;
}

// if memory error return in err -1, else no error -> err = 0
char *appendString(char *str1, char *str2, int *err)
{
  size_t len = str1 ? strlen(str1) + strlen(str2) + 1 : strlen(str2) + 1;
  size_t oldLen = str1 ? strlen(str1) + 1 : 0;
  int pos = str1 ? strlen(str1) : 0;
  char *p = (char *) realloc_zero(str1, oldLen, len);

  if (!p)
  {
    *err = -1;
    traceErr("appendString - unable to realloc string (err:-1)");
    return str1;
  };
  str1 = p;
  strcpy(&str1[pos], str2);
  *err = 0;
  return str1;
}

void parseFullPath(const char* fullPath, const char** pathResult,
    const char** nameResult)
{
  const char* name = NULL;
  const char* path = NULL;
  if (strcmp("", fullPath) == 0)
  {
    path = strdup("");
    name = fullPath;
  }
  else
  {
    name = strrchr(fullPath, PATH_DELIMITER);

    if (name == NULL)
    {
      name = fullPath;
      path = strdup("/");
    }
    else
    {
      ++name;
      path = strndup(fullPath, name - fullPath);
    }
  }
  *pathResult = path;
  *nameResult = name;
}

// extend path with slash if needed, input parameter is NOT checked
char *preparePath(const char *path)
{
  char *p = NULL;
  //extend path with slash if needed

  p = (char *) malloc(strlen(path) + 1);
  if (!p)
  {
    return NULL;
  }
  memcpy(p, path, strlen(path) + 1);
  // if path is empty or not end with slash add it
  if (!*p || p[strlen(p) - 1] != PATH_DELIMITER)
  {
    p = straddchr(p, PATH_DELIMITER);
  }
  return p;
}

// extend path with slash if needed, input parameter is NOT checked
char *preparePathWithStar(const char *path)
{
  char *p = NULL;
  //extend path with slash if needed

  p = (char *) malloc(strlen(path) + 1);
  if (!p)
  {
    return NULL;
  }
  memcpy(p, path, strlen(path) + 1);
  // if path is empty or not end with slash add it
  if (!*p || p[strlen(p) - 1] != STAR_CHAR)
  {
    p = straddchr(p, STAR_CHAR);
  }
  return p;
}

int getPageSize(SqliteDB *obj, int *pages)
{
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int r;

  *pages = 0;
  r = sqlite3_prepare_v2(obj->db, "pragma page_size", -1, &stmt, &tail);
  if (r == SQLITE_OK)
  {
    r = sqlite3_step(stmt);
    if (r == SQLITE_ROW)
    {
      *pages = sqlite3_column_int(stmt, 0);
      r = SQLITE_OK;
    }

  }
  sqlite3_finalize(stmt);
  return r;
}

int getPageCount(SqliteDB *obj, int *pages)
{
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int r;

  *pages = 0;
  r = sqlite3_prepare_v2(obj->db, "pragma page_count", -1, &stmt, &tail);
  if (r == SQLITE_OK)
  {
    r = sqlite3_step(stmt);
    if (r == SQLITE_ROW)
    {
      *pages = sqlite3_column_int(stmt, 0);
      r = SQLITE_OK;
    }

  }
  sqlite3_finalize(stmt);
  return r;
}

int setMaxDatabaseSizeKb(SqliteDB *obj, int maxDatabaseSize)
{
  char buffer[512];
  int r;
  getPageSize(obj, &r);
  maxPageCount = (maxDatabaseSize * 1024) / r;
  sprintf(buffer, "pragma max_page_count = %d", maxPageCount);
  r = sqlite3_exec(obj->db, buffer, 0, 0, 0);
  return r;
}

void setMinDurationBetweenVacuums(long value)
{
  minDurationBetweenVaccums = value;
}

void setMaxOpsBeforeVacuum(int value)
{
  maxOpsBeforeVacuum = value;
}

void setMaxDurationBeforeVacuum(long value)
{
  maxDurationBeforeVacuum = value;
}

void setMaxDBUsagePourcentageBeforeVacuum(int value)
{
  maxDBUsagePourcentageBeforeVacuum = value;
}

void setMinDurationBetweenDBUsageComputation(long value)
{
  minDurationBetweenDBUsageComputation = value;
}

void checkDBConstraints(SqliteDB *obj)
{
  long now = getCurrentTime();
  if ((now - lastVacuum) > minDurationBetweenVaccums)
  {
    // Check ops
    traceErr(
        "checkConstraints (opCountSinceLastVaccum:%d) (timeSinceLastVacuum:%ld) (timeSinceLastDBUsageComputation:%ld)",
        opsCount, (now - lastVacuum), (now - lastDbUsageComputation));
    if (opsCount > maxOpsBeforeVacuum)
    {
      traceErr("vacuum because of ops");
      performVacuum(obj);
    }
    else if ((now - lastVacuum) > maxDurationBeforeVacuum)
    {
      traceErr("vacuum because of duration");
      performVacuum(obj);
    }
    else if ((now - lastDbUsageComputation)
        > minDurationBetweenDBUsageComputation)
    {
      int pageCount = 0;
      getPageCount(obj, &pageCount);
      lastDbUsageComputation = getCurrentTime();
      int pourcentage = pageCount * 100 / maxPageCount;
      if (pourcentage > maxDBUsagePourcentageBeforeVacuum)
      {
        traceErr("vacuum because of usage");
        performVacuum(obj);
      }
    }
  }
}

sqlite3_int64 getIdFromFullPath(SqliteDB *obj, const char *fullPath)
{
  const char *sql = "SELECT ID FROM DOCUMENT WHERE PATH=? AND NAME=?";
  int r = 0;
  sqlite3_stmt *stmt = NULL; // create attribute for given document path, name and attribute name
  sqlite3_int64 res = -1;
  const char *tail = 0;

  // return -1 if invalid arguments
  if (!fullPath)
  {
    return -1;
  }

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return r;
  }

  const char *name = NULL;
  const char *path = NULL;
  parseFullPath(fullPath, &path, &name);

  r = sqlite3_bind_text(stmt, 1, path, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return r;
  }
  r = sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return r;
  }
  r = sqlite3_step(stmt);

  if (r != SQLITE_ROW)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return r;
  }
  // first get text result and next calculate size!!
  res = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  free((char*) path);
  return res;
}

/// function for attributes
// create attribute for given document path, name and attribute name
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
int createAttribute(SqliteDB *obj, sqlite3_int64 document_id,
    const char *attr_name, const char *attr_value, const int int_value,
    const char *date_value, const int attr_type)
{
  const char *sql =
      "INSERT INTO ATTRIBUTE(DOCUMENT_ID, NAME, STRING_VALUE, INT_VALUE, DATE_VALUE, ATTRIBUTE_TYPE) VALUES(?, ?, ?, ?, ? ,?)";

  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  char *p = NULL;

  // return -1 if invalid arguments
  if (!attr_name)
  {
    return -1;
  }

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return r;
  }

  r = sqlite3_bind_int64(stmt, 1, document_id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(p);
    return r;
  }

  r = sqlite3_bind_text(stmt, 2, attr_name, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(p);
    return r;
  }

  r = sqlite3_bind_text(stmt, 3, attr_value, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(p);
    return r;
  }

  r = sqlite3_bind_int(stmt, 4, int_value);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(p);
    return r;
  }

  r = sqlite3_bind_text(stmt, 5, date_value, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(p);
    return r;
  }

  r = sqlite3_bind_int(stmt, 6, attr_type);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(p);
    return r;
  }
  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  free(p);
  return r;
}

// delete all atributes for given document by path and name
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
// return -2 if sql error
int deleteAttributesFromId(SqliteDB *obj, sqlite3_int64 document_id)
{
  const char *sql = "DELETE FROM ATTRIBUTE WHERE DOCUMENT_ID=? ";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return -2;
  }

  r = sqlite3_bind_int64(stmt, 1, document_id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return -2;
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if (r != SQLITE_OK && r != SQLITE_DONE)
  {
    return -2;
  }
  r = sqlite3_changes(obj->db);
  return r;
}

// delete all atributes for given document by path and name
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
// return -2 if sql error
int deleteAttributesFromSql(SqliteDB *obj, const char *sql,
    const char *basePath)
{
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return -2;
  }

  r = sqlite3_bind_text(stmt, 1, basePath, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return -2;
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if (r != SQLITE_OK && r != SQLITE_DONE)
  {
    return -2;
  }
  r = sqlite3_changes(obj->db);
  return r;
}

// delete all atributes for given document by path and name
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
// return -2 if sql error
int deleteAttributesOneLevel(SqliteDB *obj, const char *basePath)
{
  const char *sql =
      "DELETE FROM ATTRIBUTE WHERE EXISTS ( SELECT DOCUMENT.ID FROM DOCUMENT WHERE DOCUMENT.ID = ATTRIBUTE.DOCUMENT_ID AND DOCUMENT.PATH=? )";

  return deleteAttributesFromSql(obj, sql, basePath);
}

// delete all atributes for given document by path and name
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
// return -2 if sql error
int deleteAttributesSubTree(SqliteDB *obj, const char *basePath)
{
  const char *sql =
      "DELETE FROM ATTRIBUTE WHERE EXISTS ( SELECT DOCUMENT.ID FROM DOCUMENT WHERE DOCUMENT.ID = ATTRIBUTE.DOCUMENT_ID AND DOCUMENT.PATH GLOB ? )";

  return deleteAttributesFromSql(obj, sql, basePath);
}

signed char *sqliteRetrieve(SqliteDB *obj, const char *fullPath, int *len)
{
  signed char *document;
  const char *sql = "SELECT CONTENT FROM DOCUMENT WHERE PATH=? AND NAME=?";
  const void *res = NULL;
  int documentSize = 0;
  int r = 0;
  sqlite3_stmt *stmt = NULL; // create attribute for given document path, name and attribute name
  // return -1 if invalid parameters, else if success return SQLITE_DONE = 101
  const char *tail = 0;

  // return -1 if invalid arguments
  if (!fullPath || !len)
  {
    return NULL;
  }

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return NULL;
  }

  const char *name = NULL;
  const char *path = NULL;
  parseFullPath(fullPath, &path, &name);

  r = sqlite3_bind_text(stmt, 1, path, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return NULL;
  }
  r = sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return NULL;
  }
  r = sqlite3_step(stmt);

  if (r != SQLITE_ROW)
  {
    sqlite3_finalize(stmt);
    document = NULL;
    *len = 0;
    free((char*) path);
    return NULL;
  }
  // first get text result and next calculate size!!
  res = sqlite3_column_blob(stmt, 0);
  documentSize = sqlite3_column_bytes(stmt, 0);
  document = NULL;
  *len = 0;
  if (documentSize > 0)
  {
    document = (signed char *) malloc(documentSize);
    memcpy(document, res, documentSize);
    *len = documentSize;
  }

  sqlite3_finalize(stmt);
  free((char*) path);
  return document;
}

// create document by given path, name, context and length for context
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
int sqliteCreate(SqliteDB *obj, const char *fullPath,
    const unsigned char *content, int len, InternalAttribute **attrs,
    int attrs_len)
{
  const char *sql = "INSERT INTO DOCUMENT(PATH, NAME, CONTENT) VALUES(?, ?, ?)";

  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  // return -1 if invalid arguments
  if (!fullPath || !content)
  {
    return -1;
  }

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return r;
  }

  const char* name = NULL;
  const char* path = NULL;
  parseFullPath(fullPath, &path, &name);

  r = sqlite3_bind_text(stmt, 1, path, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return r;
  }
  r = sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return r;
  }

  r = sqlite3_bind_blob(stmt, 3, content, len, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return r;
  }

  r = sqlite3_step(stmt);

  if (r != SQLITE_DONE)
  {
    sqlite3_finalize(stmt);
    free((char*) path);
    return r;
  }

  sqlite3_int64 id = sqlite3_last_insert_rowid(obj->db);

  sqlite3_finalize(stmt);
  free((char*) path);

  InternalAttribute *attr = NULL;
  int i = 0;
  for (i = 0; i < attrs_len; ++i)
  {
    attr = attrs[i];
    createAttribute(obj, id, attr->name, attr->string_value, attr->int_value,
        attr->date_value, attr->type);
  }

  return r;
}

// update document context by given path, name, context and length for context
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
int sqliteUpdate(SqliteDB *obj, const char *fullPath,
    const unsigned char *content, int len, InternalAttribute **attrs,
    int attrs_len)
{
  ++opsCount;
  const char *sql = "UPDATE DOCUMENT SET CONTENT=? WHERE ID=?";

  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  // return -1 if invalid arguments
  if (!fullPath)
  {
    return -1;
  }

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return 0;
  }

  sqlite3_int64 id = getIdFromFullPath(obj, fullPath);
  if (id < 0)
  {
    sqlite3_finalize(stmt);
    return 0;
  }

  r = sqlite3_bind_blob(stmt, 1, content, len, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return 0;
  }

  r = sqlite3_bind_int64(stmt, 2, id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return 0;
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  r = sqlite3_changes(obj->db);

  if (r > 0)
  {
    deleteAttributesFromId(obj, id);
    InternalAttribute *attr = NULL;
    int i = 0;
    for (i = 0; i < attrs_len; ++i)
    {
      attr = attrs[i];
      createAttribute(obj, id, attr->name, attr->string_value, attr->int_value,
          attr->date_value, attr->type);
    }
  }

  return r;
}

// delete document by given path and name
// if name is null delete all documents for that path
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
// return -2 if sql error
int deleteExact(SqliteDB *obj, const char *fullPath)
{
  const char *sql = "DELETE FROM DOCUMENT WHERE ID=?";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  sqlite3_int64 id = getIdFromFullPath(obj, fullPath);

  if (id < 0)
  {
    return -3;
  }

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return -2;
  }

  r = sqlite3_bind_int64(stmt, 1, id);

  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return -2;
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);

  if (r != SQLITE_OK && r != SQLITE_DONE)
  {
    return -2;
  }

  r = sqlite3_changes(obj->db);

  if (r > 0)
  {
    deleteAttributesFromId(obj, id);
  }

  return r;
}

int deleteFromSql(SqliteDB *obj, const char *sql, const char *basePath)
{
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return -2;
  }

  r = sqlite3_bind_text(stmt, 1, basePath, -1, SQLITE_STATIC);

  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return -2;
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);

  if (r != SQLITE_OK && r != SQLITE_DONE)
  {
    return -2;
  }

  r = sqlite3_changes(obj->db);

  return r;
}

// delete document by given path and name
// if name is null delete all documents for that path
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
// return -2 if sql error
int deleteOneLevel(SqliteDB *obj, const char *basePath)
{
  const char *sql = "DELETE FROM DOCUMENT WHERE PATH=?";

  int r = deleteAttributesOneLevel(obj, basePath);

  if (r < 0)
  {
    return r;
  }

  return deleteFromSql(obj, sql, basePath);
}

// delete document by given path and name
// if name is null delete all documents for that path
// return -1 if invalid parameters, else if success return SQLITE_DONE = 101
// return -2 if sql error
int deleteSubTree(SqliteDB *obj, const char *basePath)
{
  const char *sql = "DELETE FROM DOCUMENT WHERE PATH GLOB ?";

  int r = deleteAttributesSubTree(obj, basePath);

  if (r < 0)
  {
    return r;
  }

  return deleteFromSql(obj, sql, basePath);
}

// -1 no document name in fullPath
int sqliteDelete(SqliteDB *obj, const char *fullPath, int cascadeLevel)
{
  ++opsCount;
  int res = -1;
  char *basePath = NULL;
  char *exactPath = NULL;
  switch (cascadeLevel)
  {
  case CASCADING_NONE:
    res = deleteExact(obj, fullPath);
    break;
  case CASCADING_ONE_LEVEL:
    res = deleteOneLevel(obj, fullPath);
    break;
  case CASCADING_SUBTREE:
    exactPath = strndup(fullPath, strlen(fullPath) - 1);
    if (exactPath != NULL)
    {
      deleteExact(obj, exactPath);
      free(exactPath);
      basePath = preparePathWithStar(fullPath);
      if (basePath != NULL)
      {
        res = deleteSubTree(obj, basePath);
        free(basePath);
      }
      else
      {
        res = -5;
      }
    }
    else
    {
      res = -4;
    }
    break;
  default:
    break;
  }

  return res;
}

// transactions API

int beginTransaction(SqliteDB *obj)
{
  return sqlite3_exec(obj->db, "BEGIN", 0, 0, 0);
}

int commitTransaction(SqliteDB *obj)
{
  int r = sqlite3_exec(obj->db, "COMMIT", 0, 0, 0);

  if (r == SQLITE_OK)
  {
    // Check constraints
    checkDBConstraints(obj);
  }
  return r;
}

int rollbackTransaction(SqliteDB *obj)
{
  int r = sqlite3_exec(obj->db, "ROLLBACK", 0, 0, 0);

  // Check constraints
  checkDBConstraints(obj);

  return r;
}

// initialize %res% with dociument date
void initSubSearchResult(subSearchResult *res, const unsigned char *path,
    const unsigned char *name, signed char *content, int contentLength)
{
  int pathLength = 0;
  int nameLength = 0;

  if (res == NULL)
  {
    return;
  }

  res->name = res->path = NULL;
  res->content = NULL;
  res->contentLength = 0;

  pathLength = strlen((const char*) path) + 1;

  res->path = malloc(pathLength);
  if (res->path)
  {
    memset(res->path, 0, pathLength);
    memcpy(res->path, path, pathLength);
    nameLength = strlen((const char*) name) + 1;
    res->name = malloc(nameLength);
    if (res->name)
    {
      memset(res->name, 0, nameLength);
      memcpy(res->name, name, nameLength);
    }

    if (contentLength > 0)
    {
      res->content = malloc(contentLength);
      if (res->content)
      {
        memcpy(res->content, content, contentLength);
        res->contentLength = contentLength;
      }
    }
  }
}
// delete data in %res%
void emptySubSearchResult(subSearchResult *res)
{
  if (res->path != NULL)
  {
    free(res->path);
  }
  if (res->name != NULL)
  {
    free(res->name);
  }
  if (res->content != NULL)
  {
    free(res->content);
  }
  res->contentLength = 0;
}

// create and initialize search document
subSearchResult *createDocumentInMemory(const unsigned char *path,
    const unsigned char *name, signed char *content, int contentLength)
{
  subSearchResult *res = malloc(sizeof(subSearchResult));
  if (res)
  {
    initSubSearchResult(res, path, name, content, contentLength);
  }
  return res;
}

// create empty search result
SearchReturn *createSearchReturn()
{
  SearchReturn *res = (SearchReturn *) malloc(sizeof(SearchReturn));
  if (res)
  {
    res->allocated = 0;
    res->size = 0;
    res->data = NULL;
    res->hasMore = 0;
    res->result = -1;
  }
  return res;

}

void appendToSearchReturn(SearchReturn *res, subSearchResult *data)
{
  if (data == NULL || res == NULL)
  {
    return;
  }
  if (res->data == NULL)
  {
    res->data = malloc(5 * sizeof(data));
    if (res->data)
    {
      res->allocated += 5;
    }
  }

  if (res->data == NULL)
  {
    return;
  }

  if (res->size == res->allocated)
  {
    res->data = realloc(res->data,
        res->allocated * sizeof(data) + 5 * sizeof(data));
    res->allocated += 5;
  }
  res->data[res->size++] = data;

}

void freeSearchReturn(SearchReturn *data)
{
  int i = 0;
  if (data == NULL)
  {
    return;
  }
  for (i = 0; i < data->size; i++)
  {
    emptySubSearchResult(data->data[i]);
    free(data->data[i]);

  }

  free(data->data);
  free(data);
}

void freeInternalCondition(InternalCondition *data)
{
  if (data == NULL)
  {
    return;
  }
  if (data->attributeName)
  {
    free(data->attributeName);
    data->attributeName = NULL;
  }
  if (data->operand)
  {
    free(data->operand);
    data->operand = NULL;
  }
  if (data->condition1)
  {
    freeInternalCondition(data->condition1);
    data->condition1 = NULL;
  }
  if (data->condition2)
  {
    freeInternalCondition(data->condition2);
    data->condition2 = NULL;
  }
  free(data);
}

void freeInternalAttribute(InternalAttribute *data)
{
  if (data == NULL)
  {
    return;
  }
  if (data->name)
  {
    free(data->name);
    data->name = NULL;
  }
  if (data->string_value)
  {
    free(data->string_value);
    data->string_value = NULL;
  }
  if (data->date_value)
  {
    free(data->date_value);
    data->date_value = NULL;
  }
  free(data);
}

void freeInternalAttributeArray(InternalAttribute **data, int length)
{
  int i = 0;
  for (i = 0; i < length; ++i)
  {
    freeInternalAttribute(data[i]);
  }
  free(data);
}

// return struct, in result variable is result. SQLITE_OK or other, -1 if sql is not executed
SearchReturn *search(SqliteDB *obj, const char *basePath, int scope,
    InternalCondition *condition, int order, int limit)
{
  SearchReturn *res = createSearchReturn();
  char *sql = "";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  char *p = NULL;
  const unsigned char *document_path;
  const unsigned char *document_name;
  const unsigned char *document_content;
  int documentSize;
  int rowCount = 0;

  // return -1 if invalid arguments
  if (!basePath)
  {
    return NULL;
  }

  //extend path  with slash if needed

  p = preparePath(basePath);
  sql = getSearchSqlText(p, scope, condition, order, limit);

  //printf("sql: %s\n", sql);
  if (sql == NULL)
  {
    free(p);

    return res;
  }
  r = sqlite3_prepare_v2(obj->db, sql, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(p);
    free(sql);
    res->result = r;
    return res;
  }

  r = sqlite3_step(stmt);
  while (r == SQLITE_ROW)
  {
    subSearchResult *s1;
    // first get text result and next calculate size!!

    document_path = sqlite3_column_text(stmt, 0);
    document_name = sqlite3_column_text(stmt, 1);
    document_content = sqlite3_column_blob(stmt, 2);
    documentSize = sqlite3_column_bytes(stmt, 2);
    s1 = createDocumentInMemory(document_path, document_name,
        (signed char *) document_content, documentSize);
    appendToSearchReturn(res, s1);
    if ((limit > 0) && (++rowCount == limit))
    {
      res->hasMore = 1;
      break;
    }
    r = sqlite3_step(stmt);
  }
  if (r == SQLITE_ROW || r == SQLITE_DONE)
  {
    res->result = SQLITE_OK;
  }
  else
  {
    res->result = r;
  }
  sqlite3_finalize(stmt);
  free(p);
  free(sql);

  return res;
}

char *walk_DATE_INTEGER(const InternalCondition *cond, char *whereSQL, int *err,
    char *oper)
{

  traceErr("walk_DATE_INTEGER (cond:0x%.8x) (cond->operand:%s) "
      "(cond->attributeName:%s) (cond->type:%d) (cond->oper:%d) (whereSQL:%s) "
      "(oper:%s)", cond, cond ? cond->operand : "N/A",
      cond ? cond->attributeName : "N/A", cond ? cond->type : -1,
      cond ? cond->oper : -1, whereSQL, oper);

  //int err = 0;
  char *w = NULL;
  char *buffer = malloc(strlen(oper) + strlen(cond->operand) + 50);
  if (!buffer)
  {
    traceErr("walk_DATE_INTEGER - end (buffer=NULL)");
    return NULL;
  }

  if (cond->type == TYPE_DATE)
  {
    //char buffer[255];
    sprintf(buffer, ".DATE_VALUE %s '%s'", oper, cond->operand);
    w = appendString(whereSQL, buffer, err);
  }
  else if (cond->type == TYPE_INTEGER)
  {
    //char buffer[255];
    sprintf(buffer, ".INT_VALUE %s %s", oper, cond->operand);
    w = appendString(whereSQL, buffer, err);
  }
  else
  {
    //char buffer[255];
    if (cond->oper == OPERATOR_STARTS_WITH)
    {
      sprintf(buffer, ".STRING_VALUE %s '%s*'", oper, cond->operand);
    }
    else
    {
      sprintf(buffer, ".STRING_VALUE %s '%s'", oper, cond->operand);
    }
    w = appendString(whereSQL, buffer, err);
  }
  free(buffer);

  traceErr("walk_DATE_INTEGER - end (err:%d)", *err);
  return w;
}

int walkThroughCondition(const InternalCondition *cond, int i, char **sSQL,
    char **wSQL)
{
  int res = 0;
  char alias[100] = "";
  int err = 0;
  char *whereSQL = *wSQL;
  char *selectSQL = *sSQL;
  static char *format_operator_equal =
      "%s.DOCUMENT_ID = doc.ID AND %s.NAME = '%s' AND %s";
  static char *format_attribute = ", ATTRIBUTE AS %s";

  sprintf(alias, "att%d", i);

  if (cond->oper == OPERATOR_AND)
  {
    whereSQL = appendString(whereSQL, " ( ", &err);
    res = res | err;
    res = res
        | walkThroughCondition(cond->condition1, 2 * i, &selectSQL, &whereSQL);
    whereSQL = appendString(whereSQL, " AND ", &err);
    res = res | err;
    res = res
        | walkThroughCondition(cond->condition2, 2 * i + 1, &selectSQL,
            &whereSQL);
    whereSQL = appendString(whereSQL, " ) ", &err);
    res = res | err;
  }
  else if (NULL == cond->attributeName)
  {
    traceErr("walkThroughCondition - cond->attributeName is null (oper:%d)",
        cond->oper);
    return -1;
  }
  else
  {
    char *strTemp = (char *) malloc(
        strlen(format_operator_equal) - 10 + 4 * strlen(alias)
            + strlen(cond->attributeName) + 1);
    if (strTemp)
    {
      sprintf(strTemp, format_operator_equal, alias, alias, cond->attributeName,
          alias);
      whereSQL = appendString(whereSQL, strTemp, &err);
      free(strTemp);
      res |= err;
    }

    strTemp = (char *) malloc(strlen(format_attribute) - 2 + strlen(alias) + 1);
    if (strTemp)
    {
      sprintf(strTemp, format_attribute, alias);
      /*
       selectSQL = appendString(selectSQL,", ATTRIBUTE AS ", &err);
       res |=err;
       selectSQL = appendString(selectSQL,alias,&err);
       res |=err;
       */
      selectSQL = appendString(selectSQL, strTemp, &err);
      res |= err;
      free(strTemp);
    }

    if (cond->oper == OPERATOR_EQUAL)
    {
      whereSQL = walk_DATE_INTEGER(cond, whereSQL, &err, "=");
      res |= err;

    }
    else if (cond->oper == OPERATOR_NOT_EQUAL_TO)
    {
      //Joins AATTRIBUTE table

      whereSQL = walk_DATE_INTEGER(cond, whereSQL, &err, "!=");
      res |= err;
    }
    else if (cond->oper == OPERATOR_GREATER)
    {

      whereSQL = walk_DATE_INTEGER(cond, whereSQL, &err, ">");
      res |= err;

    }
    else if (cond->oper == OPERATOR_LOWER)
    {
      whereSQL = walk_DATE_INTEGER(cond, whereSQL, &err, "<");
      res |= err;
    }
    else if (cond->oper == OPERATOR_STARTS_WITH)
    {
      whereSQL = walk_DATE_INTEGER(cond, whereSQL, &err, "GLOB");
      res |= err;

    }
    else
    {
      return -666;
    }
  }
  *wSQL = whereSQL;
  *sSQL = selectSQL;

  return res;
}

char *getSearchSqlText(char *path, int scope, InternalCondition *condition,
    int order, int limit)
{
  static char *format_ordering = "ORDER BY doc.PATH %s, doc.NAME %s";
  char *startSQL1 =
      "SELECT DISTINCT doc.PATH, doc.NAME, doc.CONTENT FROM DOCUMENT AS doc ";
  char *whereSQL1 = " WHERE ( doc.PATH ";
  char *selectSQL = NULL;
  char *whereSQL = NULL;
  char *ordering = NULL;
  char limiting[255] = "";
  char *res = NULL;
  int err;
  int outErr = 0;
  char *strTemp;

  selectSQL = appendString(selectSQL, startSQL1, &err);
  outErr |= err;
  whereSQL = appendString(whereSQL, whereSQL1, &err);
  outErr |= err;

  if (scope == SCOPE_EXACT)
  {
    char *buffer = (char *) malloc(strlen(path) + 10);
    if (buffer)
    {
      sprintf(buffer, "='%s' )", path);

      whereSQL = appendString(whereSQL, buffer, &err);
      free(buffer);
    }
    else
    {
      err = -1;
    }
  }
  else if (scope == SCOPE_SUBTREE)
  {
    char *buffer = (char *) malloc((2 * strlen(path)) + 50);
    char* exactFullPath = strdup(path);
    if (buffer && exactFullPath)
    {
      exactFullPath[strlen(exactFullPath) - 1] = 0;
      const char* exactName = NULL;
      const char* exactPath = NULL;
      parseFullPath(exactFullPath, &exactPath, &exactName);
      sprintf(buffer, " GLOB '%s*' OR (doc.PATH='%s' AND doc.NAME='%s') ) ",
          path, exactPath, exactName);
      whereSQL = appendString(whereSQL, buffer, &err);
      free(buffer);
      free(exactFullPath);
      free((char*) exactPath);
    }
    else if (exactFullPath != NULL)
    {
      free(exactFullPath);
      err = -1;
    }
    else
    {
      err = -1;
    }
  }
  else
  {
    err = -1;
  }

  outErr |= err;

  if (condition)
  {
    whereSQL = appendString(whereSQL, " AND ", &err);
    outErr |= err;
    outErr |= walkThroughCondition(condition, 1, &selectSQL, &whereSQL);
  }

  // ordering = appendString(ordering,"ORDER BY doc.PATH, doc.NAME", &err);
  // outErr |= err;

  ordering = (char *) malloc(strlen(format_ordering) - 4 + 12 + 1);

  if (ordering)
  {
    if (order == ORDER_DESC)
    {
      sprintf(ordering, format_ordering, " DESC ", " DESC ");
      //ordering = appendString(ordering," DESC ", &err);
      //outErr |= err;
    }
    else
    {
      sprintf(ordering, format_ordering, " ASC ", " ASC ");
      //ordering = appendString(ordering," ASC ", &err);
      //outErr |= err;
    }
  }
  else
  {
    outErr |= -1;
  }

  if (limit > 0)
  {
    sprintf(limiting, " LIMIT %d ", limit + 1);
  }

  strTemp = (char *) malloc(
      strlen(selectSQL) + strlen(whereSQL) + strlen(ordering) + strlen(limiting)
          + 5);
  if (strTemp)
  {
    sprintf(strTemp, "%s %s %s %s", selectSQL, whereSQL, ordering, limiting);
    res = appendString(res, strTemp, &err);
    free(strTemp);
  }
  else
  {
    err = -1;
  }
  outErr |= err;

  /*
   res = appendString(res,selectSQL,&err);
   outErr |= err;
   res = appendString(res," ",&err);
   outErr |= err;
   res = appendString(res,whereSQL,&err);
   outErr |= err;
   res = appendString(res," ",&err);
   outErr |= err;
   res = appendString(res,ordering,&err);
   outErr |= err;
   res = appendString(res,limiting,&err);
   outErr |= err;
   */
  free(selectSQL);
  free(whereSQL);
  free(ordering);
  if (outErr != 0)
  {
    free(res);
    res = NULL;
  }
  return res;
}

/*
 ** Perform an online backup of database pDb to the database file named
 ** by zFilename. This function copies 5 database pages from pDb to
 ** zFilename, then unlocks pDb and sleeps for 250 ms, then repeats the
 ** process until the entire database is backed up.
 **
 ** The third argument passed to this function must be a pointer to a progress
 ** function. After each set of 5 pages is backed up, the progress function
 ** is invoked with two integer parameters: the number of pages left to
 ** copy, and the total number of pages in the source file. This information
 ** may be used, for example, to update a GUI progress bar.
 **
 ** While this function is running, another thread may use the database pDb, or
 ** another process may access the underlying database file via a separate
 ** connection.
 **
 ** If the backup process is successfully completed, SQLITE_OK is returned.
 ** Otherwise, if an error occurs, an SQLite error code is returned.
 */
/*
 void xProgress(int remaining, int pageCount)
 {
 int prg = 0;
 if (pageCount > 0)
 {
 prg=  ((pageCount - remaining) / pageCount);
 fprintf(stdout,"Backup progresss %d\n",prg);
 fflush(stdout);
 //printf("Backup progresss %d\n",prg);
 }
 }


 int backupDb_1(
 SqliteDB *obj,               // Database to back up
 const char *zFilename,      // Name of file to back up to
 int nbPagePerStep, int sleepDelayBetweenStep)
 {
 int rc;                     // Function return code
 sqlite3 *pFile;             // Database connection opened on zFilename
 sqlite3_backup *pBackup;    // Backup handle used to copy data
 sqlite3 *pDb = obj->db;


 // Open the database file identified by zFilename.
 rc = sqlite3_open(zFilename, &pFile);
 if( rc==SQLITE_OK ){

 // Open the sqlite3_backup object used to accomplish the transfer
 pBackup = sqlite3_backup_init(pFile, "main", pDb, "main");
 if( pBackup ){

 // Each iteration of this loop copies 5 database pages from database
 // pDb to the backup database. If the return value of backup_step()
 // indicates that there are still further pages to copy, sleep for
 // 250 ms before repeating.
 do {
 rc = sqlite3_backup_step(pBackup, nbPagePerStep);
 xProgress(
 sqlite3_backup_remaining(pBackup),
 sqlite3_backup_pagecount(pBackup)
 );
 if( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED ){
 sqlite3_sleep(sleepDelayBetweenStep);
 }
 } while( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED );

 // Release resources allocated by backup_init().
 (void)sqlite3_backup_finish(pBackup);
 }
 rc = sqlite3_errcode(pFile);
 }

 // Close the database connection opened on database file zFilename
 // and return the result of this function.
 (void)sqlite3_close(pFile);
 return rc;
 }
 */

/*

 int main()
 {
 SearchReturn * ser;
 SqliteDB * obj = SqliteOpen("C:/Users/sbaltov/Desktop/storage.db");
 void * document= NULL;
 int len = 0;
 int r = 0;
 int limit =0;
 char * se;
 const unsigned char content[] =  "ABCDEFG\0HJKLMN";


 char * path = "/";
 const char * name = "11112";
 char * fullPath = "/11112";

 InternalCondition cond;

 cond.attributeName = "A1";
 cond.oper = OPERATOR_STARTS_WITH;
 cond.operand = "aaa";
 cond.type = TYPE_STRING;


 se = getSearchSqlText(path,1,&cond,0,100);
 printf(se);printf("\n");
 free(se);


 if (obj && obj->last_error ==SQLITE_OK)
 {
 printf("OPEN OK\n");
 //        deleteCascade(obj,fullPath,1);

 r = deleteDocument(obj, path, name,1);
 r = createDocument(obj,path,name, content, sizeof(content));
 if (r == SQLITE_DONE)
 {

 printf ("create document OK\n");
 r = retrieveDocument(obj, path,name, &document, &len);
 if (document != NULL)
 {
 if (memcmp(document,content, len) ==0)
 {
 printf("test is passed: %s\n",document);
 } else     printf("test is not passed %s\n",document);

 free(document);
 document = NULL;

 r = createAttribute(obj, path, name, cond.attributeName,"aaalabala",&limit,NULL,&cond.type);

 ser = search(obj,"/",SCOPE_SUBTREE, &cond,0,0);
 if (ser->size == 1)
 {
 printf("Search passed\n");
 }
 freeSearchReturn(ser);




 r = deleteDocument(obj,path,name,1);
 if (r !=1)
 {
 printf("Document is not deleted %d\n",r);
 }

 r = retrieveDocument(obj, path,name, &document, &len);
 printf("retrieve deleted document %d \n",r);
 free(document);

 }
 } else
 {
 printf("cannot create document %s", sqlite3_errmsg(obj->db));
 }

 SqliteClose(obj);

 } else
 {
 printf("NOT OPEN !");
 }
 printf("This is a native C program.\n");
 return 0;
 }

 */
