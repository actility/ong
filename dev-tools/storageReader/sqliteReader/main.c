
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "sqlite3.h"

int TraceLevel = 2;
int TraceDebug = 1;
int TraceProto = 0;
int TraceSize = 200 * 1024; // for each file

#define ATTR_TYPE_INTEGER 0
#define ATTR_TYPE_STRING 1
#define ATTR_TYPE_DATE 2
#define ATTR_TYPE_LIST_INTEGER 3
#define ATTR_TYPE_LIST_STRING 4
#define ATTR_TYPE_LIST_DATE 5
#define ATTR_TYPE_OLD_DATE 8
#define ATTR_TYPE_OLD_INTEGER 7
#define ATTR_TYPE_OLD_STRING 6

/**
 * @brief Print help message for program, print help on options.
 * @param prg the program name
 */
void usage(char *prg)
{
  printf("usage: %s [-h] [-v] [-o] [-f <file>] <database> <path>\n", prg);
  printf("where <database> is the absolute filename of the SQLite database\n");
  printf("where <path> is path of the document to retrieve\n");
  printf("and where options are:\n");
  printf(" -h : display this help message\n");
  printf(" -o : specifies that the document content is oBIX format\n");
  printf(" -v : display the version string of the loaded libraries\n");
  printf("\n");
}

/**
 * @brief Print the version string of the loaded library.
 */
void version()
{
  printf("%s\n", rtl_version());
  printf("%s\n", XoVersion());
}


/**
 * @brief Load .xor files for m2m and oBIX schema definition.
 * @param rootact the path to the actility root folder.
 * @return 1 if xor files could have been found and loaded, 0 otherwize.
 */
int loadM2MXoRef(char *rootAct)
{
  char  xfile[1024];
  int ret;

  sprintf (xfile, "%s/etc/xo/m2m.xor", rootAct);
  ret = XoExtLoadRef(xfile);
  if  (ret < 0)
  {
    fprintf(stderr, "ERROR cannot load '%s' ret=%d\n", xfile, ret);
    return 0;
  }

  sprintf (xfile,"%s/etc/xo/xobix.xor",rootAct);
  ret = XoExtLoadRef(xfile);
  if  (ret < 0)
  {
    fprintf(stderr, "ERROR cannot load '%s' ret=%d\n", xfile, ret);
    return 0;
  }

  return 1;
}

/**
 * @brief Open the SQLite database
 * @param dbFile the SQLite database file to open.
 * @return 1 if database can be successfully opened, 0 otherwise.
 */
sqlite3 *openDB(char *dbFile)
{
  sqlite3 *db;
  int res;
  if (access(dbFile, R_OK) != 0)
  {
    fprintf(stderr, "Unable to access file %s\n", dbFile);
    return 0;
  }
  res = sqlite3_open(dbFile, &db);
  if (res != SQLITE_OK)
  {
    fprintf(stderr, "Failed to open database (error:%d) (%s)\n", res, sqlite3_errmsg(db));
    sqlite3_close(db);
    return 0;
  }
  return db;
}

/**
 * @brief Look for the Document content that matches the path provided.
 * @param db the SQLite database connection.
 * @param path the Document path to look for.
 * @param name the Document name to look for.
 * @param documentId the reference on the document identifier, set with DOCUMENT.ID found in
 * result set.
 * @param content the reference on the address that point out the document content. The
 * memory is allocated in this function, and is to be deleted afterward.
 * @param contentSize the address on the content size value. 
 * @param isObix 1 if the content is supposed to be a oBIX document, 0 otherwise.
 * @return 1 if document content can be fetched and decoded, 0 otherwise.
 */
int getDocumentDBContent(sqlite3 *db, char *path, char *name,
  sqlite3_int64 * documentId, char ** content, int * contentSize, int isObix)
{
  int res;
  char *sql = "SELECT ID, CONTENT FROM DOCUMENT WHERE PATH=? AND NAME=?";
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int documentSize = 0;
  const unsigned char *documentContent;


  res = sqlite3_prepare_v2(db, sql,  - 1, &stmt, &tail);
  if (res != SQLITE_OK)
  {
    fprintf(stderr, "Failed to prepare statement (error:%d) (%s)\n", res, sqlite3_errmsg(db));
    return 0;
  } 

  res = sqlite3_bind_text(stmt, 1, path,  - 1, SQLITE_STATIC);
  if (res != SQLITE_OK)
  {
    fprintf(stderr, "Failed to bind \"path\" in prepared statement (error:%d) (%s)\n", res, 
      sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return 0;
  }

  res = sqlite3_bind_text(stmt, 2, name,  - 1, SQLITE_STATIC);
  if (res != SQLITE_OK)
  {
    fprintf(stderr, "Failed to bind \"name\" in prepared statement (error:%d) (%s)\n", res, 
      sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return 0;
  }

  res = sqlite3_step(stmt);
  if (res != SQLITE_ROW)
  {
    fprintf(stderr, "Failed to retrieve a document that matches this path (res:%d) "
      "(path:%s) (name:%s)\n", res, path, name);
    sqlite3_finalize(stmt);
    return 0;
  }

  *documentId = sqlite3_column_int64(stmt, 0);

  documentContent = (const unsigned char *)sqlite3_column_blob(stmt, 1);
  documentSize = sqlite3_column_bytes(stmt, 1);

  *contentSize = documentSize;
  *content = malloc(documentSize);
  memcpy(*content, documentContent, documentSize);
  
  sqlite3_finalize(stmt);

  return 1;
}

/**
 * @brief Look for the Document attributes.
 * @param db the SQLite database connection.
 * @param documentId the document identifier to look for.
 */
void displayAttributes(sqlite3 *db, sqlite3_int64 documentId)
{
  char *sql = "SELECT NAME,STRING_VALUE,INT_VALUE,DATE_VALUE, ATTRIBUTE_TYPE FROM ATTRIBUTE WHERE DOCUMENT_ID = ?";
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int res;
  int i = 0;

  res = sqlite3_prepare_v2(db, sql,  - 1, &stmt, &tail);
  if (res != SQLITE_OK)
  {
    fprintf(stderr, "Failed to prepare statement (error:%d) (%s)\n", res, sqlite3_errmsg(db));
    return;
  }

  res = sqlite3_bind_int64(stmt, 1, documentId);
  if (res != SQLITE_OK)
  {
    fprintf(stderr, "Failed to bind \"id\" in prepared statement (error:%d) (%s)\n", res, 
      sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
  }

  res = sqlite3_step(stmt);

  while (res == SQLITE_ROW)
  {
    const char *name;
    const char *string_value;
    int int_value = 0;
    const char *date_value;
    int attr_type = 0;
    i++;
    name = (const char *)sqlite3_column_text(stmt, 0);
    string_value = (const char *)sqlite3_column_text(stmt, 1);
    int_value = sqlite3_column_int(stmt, 2);
    date_value = (const char *)sqlite3_column_text(stmt, 3);
    attr_type = sqlite3_column_int(stmt, 4);

    switch (attr_type)
    {
      case ATTR_TYPE_INTEGER:
      case ATTR_TYPE_LIST_INTEGER:
      case ATTR_TYPE_OLD_INTEGER:
        printf("%s: %d\n", name, int_value);
        break;
      case ATTR_TYPE_STRING:
      case ATTR_TYPE_LIST_STRING:
      case ATTR_TYPE_OLD_STRING:
        printf("%s: %s\n", name, string_value);
        break;
      case ATTR_TYPE_DATE:
      case ATTR_TYPE_LIST_DATE:
      case ATTR_TYPE_OLD_DATE:
        printf("%s: %s\n", name, date_value);
        break;
      default:
        fprintf(stderr, "Unsupported attribute type (%d)\n", attr_type);
        break;
    }

    res = sqlite3_step(stmt);
  }
  printf("%d attributes found\n", i);

  sqlite3_finalize(stmt);
 
}

/**
 * @brief Save the Xo raw content into a file.
 * @param content the content to save.
 * @param size the content size.
 * @param fileName the name of the file to create.
 * @return 1 if the operation succeeds, 0 otherwise.
 */
int saveRawContentToFile(char * content, int size, char * fileName)
{
  FILE * output;
  int res;
  output = fopen(fileName, "wb");
  if (size != (res = fwrite(content, sizeof(char), size, output)))
  {
    fprintf(stderr, "Failed to write xo binary buffer into file (size:%d) (res:%d)\n",
      size, res);
    return 0;
  }
  
  return 1;
}

/**
 * @brief Display on standard output the XML buffer as decoded from xo content.
 * @param content the content to save.
 * @param size the content size.
 * @param isObix specifies if the content is supposed to be an oBIX document.
 * @return 1 if the operation succeeds, 0 otherwise.
 */
int displayXmlContent(char * content, int size, int isObix)
{
  void *xoObject = NULL;
  void *w = NULL;
  int flags = 0;
  char *newBuffer = NULL;

  // decode content as XO binary
  if (isObix)
  {
    flags = XOML_PARSE_OBIX;
  }  
  xoObject = XoLoadMem(content, size);

  free(content);
  if (!xoObject)
  {
    fprintf(stderr, "Failed to load Xo buffer\n");
    return 0;
  }

  w = XoWritXmlMem(xoObject, flags, &newBuffer, "");
  if (!w)
  {
    fprintf(stderr, "Failed to decode Xo buffer\n");
    XoFree(xoObject, 1);
    return 0;
  }

  printf(newBuffer);
  XoWritXmlFreeMem(w);
  XoFree(xoObject, 1);
  return 1;
}

/**
 * The main entry point of the program...
 * @param argc number of arguments
 * @param argv the arguments as table of strings
 */
int main(int argc, char *argv[])
{
  char *rootact;
  char tmp[255];
  char path[512];
  char *pPtr;
  int c, res;
  char *fValue = NULL;
  int isObix = 0;
  sqlite3 * db;

  char * content = NULL;
  int contentSize = 0;
  sqlite3_int64 documentId = 0;

  while ((c = getopt (argc, argv, "hvf:o")) != -1)
  {
    switch (c)
    {
      case 'h':
        usage(argv[0]);
        return 0;

      case 'v':
        version();
        return 0;

      case 'o':
        isObix = 1;
        break;

      case 'f':
        fValue = optarg;
        break;

      case '?':
        if (optopt == 'f')
        {
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        }
        else if (isprint (optopt))
        {
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        }
        else
        {
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return 1;
      
      default :
        return 1;
    }
  }

  if (argc - optind != 2)
  {
    fprintf(stderr, "*** Missing parameter\n");
    usage(argv[0]);
    exit(1);
  }

  if (strncmp(argv[optind+1], "/", 1))
  {
    fprintf(stderr, "*** Wrong path entered; must start with a '/'\n");
    exit(1);
  }
  
  if (argv[optind+1][strlen(argv[optind+1]) - 1] ==  '/')
  {
    fprintf(stderr, "*** Wrong path entered; must not end with a '/'\n");
    exit(1);
  }
 
  pPtr = strrchr(argv[optind+1], '/');
  pPtr++;
  memset(path, 0, sizeof(path));
  strncpy(path, argv[optind+1], pPtr - argv[optind+1]);

  rootact = getenv("ROOTACT");
  if ((!rootact || !*rootact))
  {
    fprintf(stderr, "%s unset, abort\n", "ROOTACT");
    exit(1);
  }

  rtl_init();

  rtl_tracemutex();
  rtl_tracelevel(TraceLevel);
  rtl_tracesizemax(TraceSize);
  sprintf(tmp, "ongContentSqliteReader.log");
  rtl_tracerotate(tmp);
  
  XoInit(0);
  sprintf(tmp, "%s/etc/xo/.", rootact);
  XoLoadNameSpaceDir(tmp);
  XoLoadSharedDicoDir(tmp);

  if (! loadM2MXoRef(rootact))
  {
    exit(1);
  }

  if (! (db = openDB(argv[optind])) )
  {
    exit(1);
  }

  res = getDocumentDBContent(db, path, pPtr, &documentId, &content, 
    &contentSize, isObix);
  if (! documentId)
  {
    fprintf(stderr, "Document not found...\n");
    sqlite3_close(db);
    return 1;
  }

  printf("ATTRIBUTES:\n");
  printf("-----------\n");
  displayAttributes(db, documentId);
  sqlite3_close(db);

  printf("\n");
  printf("CONTENT:\n");
  printf("--------\n");
  if (fValue)
  {
    res = saveRawContentToFile(content, contentSize, fValue);
    printf("Saved to file %s\n", fValue);
  }
  else
  {
    res = displayXmlContent(content, contentSize, isObix);
  } 
  return ! res;

}

