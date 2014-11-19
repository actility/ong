#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "sqlite3.h"
#include "sqlite_driver.h"
//#include "vld.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

#define TRANSACTION_NAME(name) static char * transaction_name = name
#define GET_TRANSACTION_NAME transaction_name


char *getSearchSqlText(char *path, int scope, InternalCondition *condition, int order, int limit);
int deleteAllAttributes(SqliteDB *obj, const char *document_path, sqlite3_int64 doc_id);
int createAttribute(SqliteDB *obj, sqlite3_int64 document_id, const char *attr_name, const char *string_value, const int int_value, const char *date_value, const int attr_type);
InternalAttribute **appendAttributeToList(InternalAttribute **list, int len, char *attr_name, int attr_type, char *string_value, int int_value, char *date_value);
int _createDocument(SqliteDB *obj, const char *path, const char *name, const unsigned char *content, int len, sqlite3_int64 *doc_id);
int walkThroughCondition(const InternalCondition *cond, char **wSQL);
int _updateDocument(SqliteDB *obj, sqlite3_int64 doc_id,  const unsigned char *content, int len);
int appendAttributesToDocument(SqliteDB *obj, subSearchResult *s, char **attributes, int attr_len);
InternalAttribute **sqliteRetrieveAttributes(SqliteDB *obj, sqlite3_int64 id, int *attrLen );
void emptySubSearchResult(subSearchResult *res);
void traceSqlToLog(void* param1,const char* trace);
subSearchResult *createDocumentInMemory(sqlite3_int64 id, const unsigned char *path, const unsigned char *name, signed char *content, int contentLength);
SearchReturn *createSearchReturn();

/**
 * Clear errors in the sqlite database object
 * @param obj the sqlite database object
 */
static void clear_error(SqliteDB *obj)
{
  if (obj && obj->last_error)
  {
    obj->last_error = 0;
  }
}

/**
 * Open the sqlite connection 
 * @param fname the path where the database is saved
 * @return Return a sqlite database object or 0 if a probleme occur
 */
SqliteDB *SqliteOpen(const char *fname)
{
  //    const char * err;
  SqliteDB *obj = (SqliteDB *)malloc(sizeof(SqliteDB));

  if (!obj)
  {
    return 0;
  }

  obj->last_error = sqlite3_open(fname, &obj->db);
  sqlite3_trace(obj->db, traceSqlToLog, NULL);
  return obj;
}
/**
 * Display all the sql queries
 * @param param1 nothing
 * @param trace an SQL Query
 */
void traceSqlToLog(void* param1,const char* trace){
  LOG("%s\n",trace);
}
/**
 * Close the connection with the database
 * @param obj The sqlite database to close
 *
 */
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
/**
 * Check if the sqlite database object is not null
 * @param obj the sqlite database object
 * 
 */
int SqliteIsValid(const SqliteDB *obj)
{
  return obj && obj->db;
}

/**
 * Change the database version number 
 * @param obj the sqlite database object
 * @param major the major version number
 * @param minor the minor version number
 * @return Return 0 if no problem occur
 */
int _updateVersion(SqliteDB *obj, int major, int minor)
{
  int r;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, "UPDATE VERSION SET MAJOR=:?, MINOR = :?",  - 1, &stmt, &tail);
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

/**
 * Set pragma option for db and make vacuum
 * @param obj the sqlite database object
 * @return Return 0 if no problem occur
 */
int activate(SqliteDB *obj)
{
  int r;
  r = sqlite3_exec(obj->db, "pragma auto_vacuum = incremental", 0, 0, 0);
  if (r == SQLITE_DONE)
  {
    r = sqlite3_exec(obj->db, "vacuum", 0, 0, 0);
  }
  return r;
}

/**
 * execute raw sql string. Use it in DB creatation
 * @param obj the sqlite database object
 *
 */
int executeSqlStatementWithOutParameters(SqliteDB *obj, char *sql)
{
  return sqlite3_exec(obj->db, sql, 0, 0, 0);
}
/**
 * check db version with major and minor
 * @param obj the sqlite database object
 * @param major the major version number
 * @param minor the minor version number
 * @return Return 0 if no problem occur. return -1 if more as one record found in table. return -2 if major version is not equal to our version
 */
int checkDBVersion(SqliteDB *obj, int *major, int *minor)
{
  int r;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, "SELECT MAJOR, MINOR FROM VERSION",  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return r;
  }

  r = sqlite3_step(stmt);
  // no records for version, update DB to this version
  if (r == SQLITE_DONE)
  {
    r = -1;
  }
  else if (r == SQLITE_ROW)
  {
    *major = sqlite3_column_int(stmt, 0);
    *minor = sqlite3_column_int(stmt, 1);

    r = sqlite3_step(stmt);
    if (r == SQLITE_ROW)
    {
      r =  - 1; // if find more as one record, return -1
    }

  }

  sqlite3_finalize(stmt);
  return r;
}

/**
 * Append char to end of string
 * @param str the string to append char
 * @param c the number of char
 * @return Return the new string
 */
char *straddchr(char *str, int c)
{
  size_t len = str ? 2 + strlen(str) : 2;
  char *p = (char *)realloc(str, len); /* add space (need space for char and \0)
                    */
  if (!p)
  {
    return str;
  }
  str = p;
  str[len - 2] = c;
  str[len - 1] = 0;
  return str;
}
/**
 * I don't know !!!
 * 
 *
 */
void *realloc_zero(void *pBuffer, size_t oldSize, size_t newSize)
{
  void *pNew = realloc(pBuffer, newSize);
  if (newSize > oldSize && pNew)
  {
    size_t diff = newSize - oldSize;
    void *pStart = ((char *)pNew) + oldSize;
    memset(pStart, 0, diff);
  }
  return pNew;
}
/**
 * Add a string to the end of another string
 * @param str1 The string 
 * @param str2 the string to append
 * @param err if memory error return in err -1, else no error -> err = 0
 * @return Return the new string
 */
char *appendString(char *str1, char *str2, int *err)
{
  size_t len = str1 ? strlen(str1) + strlen(str2) + 1 : strlen(str2) + 1;
  size_t oldLen = str1 ? strlen(str1) + 1 : 0;
  int pos = str1 ? strlen(str1) : 0;
  char *p = (char *)realloc_zero(str1, oldLen, len);

  if (!p)
  {
    *err =  - 1;
    return str1;
  };
  str1 = p;
  strcpy(&str1[pos], str2);
  *err = 0;
  return str1;
}
/**
 * extend path with slash if needed, input parameter is NOT checked
 * 
 *
 */
char *preparePath(const char *path)
{
  char *p = NULL;
  //extend path with slash if needed

  p = (char *)malloc(strlen(path) + 1);
  if (!p)
  {
    LOG_RETURN(0, "Memory Error: preparePath can not malloc memory\n");
  }
  memcpy(p, path, strlen(path) + 1);
  // if path is empty or not end with slash add it
  if (! *p || p[strlen(p) - 1] != PATH_DELIMITER)
  {
    p = straddchr(p, PATH_DELIMITER);
  }
  return p;
}

/**
 * extend path with slash and star if needed, input parameter is NOT checked
 *
 */
char *preparePathWithSlashAndStar(const char *path)
{
  char *p = NULL;
  p = (char *)malloc(strlen(path) + 1);
  if (!p)
  {
    LOG_RETURN(0, "Memory Error: preparePath can not malloc memory\n");
  }
  memcpy(p, path, strlen(path) + 1);
  //extend path with slash if needed
  if (path && path[strlen(path) - 1] != PATH_DELIMITER)
  {
    p = straddchr(p, PATH_DELIMITER);
    if (!p)
    {
      LOG_RETURN(0, "Memory Error: preparePathWithStar can not malloc memory\n");
    }
    p = straddchr(p, STAR_CHAR);
    if (!p)
    {
      LOG_RETURN(0, "Memory Error: preparePathWithStar can not malloc memory\n");
    }


  }
  else
  {
    p = straddchr(p, STAR_CHAR);
    if (!p)
    {
      LOG_RETURN(0, "Memory Error: preparePathWithStar can not malloc memory\n");
    }

  }
  return p;
}

/**
 * Get document id if the path or the id exist in the database and if document check conditions
 * @param obj The sqlite database object
 * @param fullPath the path of the document
 * @param id the id of the document
 * @param condition the condition that the document must be checked
 */
sqlite3_int64 getDocumentIdByCondition(SqliteDB *obj, const char *fullPath, sqlite3_int64 id, const InternalCondition *condition)
{
 //printf("Function: getDocumentIdByCondition\n");
  char *sql = "SELECT ID FROM DOCUMENT ";
  char *whereSQL1 = " WHERE PATH=? AND NAME=? ";
  char *whereSQL2 = " WHERE PATH=? AND NAME=? AND ";
  char *whereSQL3 = " WHERE ID=? AND ";


  char *selectSQL = NULL;
  char *whereSQL = NULL;

  int outErr = 0;
  int err = 0;
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  char *path = NULL;
  const char *name;
  sqlite3_int64 doc_id = 0;

  if (id && !condition)
  {
    return id;
  }


  selectSQL = appendString(selectSQL, sql, &err);

  outErr |= err;

  if (outErr)
  {
    free(selectSQL);
  }
  // select document id for document passed condition
  if (id)
  {
   //printf("Id is knowing\n");
    whereSQL = appendString(whereSQL, whereSQL3, &err);
    outErr |= err;
    err = walkThroughCondition(condition, &whereSQL);
    if (err == 0)
    {
      selectSQL = appendString(selectSQL, whereSQL, &err);
      outErr |= err;
    }
    free(whereSQL);
    whereSQL = NULL;
    if (outErr)
    {
      free(selectSQL);
      LOG_RETURN(0, "Can not create SQL query in %s %d\n", __FILE__, __LINE__);
    }
    r = sqlite3_prepare_v2(obj->db, selectSQL,  - 1, &stmt, &tail);
    if (r != SQLITE_OK)
    {
      free(selectSQL);
      LOG_RETURN(0, "sqlite3_prepare fail in %s %d\n", __FILE__, __LINE__);
    }
    r = sqlite3_bind_int64(stmt, 1, id);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      free(selectSQL);
      LOG_RETURN(0, "sqlite3_bind_int64 fail in %s %d\n", __FILE__, __LINE__);
    }
    r = sqlite3_step(stmt);
    if (r != SQLITE_ROW && r != SQLITE_DONE)
    {
      sqlite3_finalize(stmt);
      free(selectSQL);
      LOG_RETURN(0, "sqlite3_step fail in %s %d with result %d\n", __FILE__, __LINE__, r);
    }
    doc_id = (r == SQLITE_ROW ? sqlite3_column_int64(stmt, 0) : 0);
    sqlite3_finalize(stmt);
    free(selectSQL);

  }
  else   // select id for document with this full path passed condition
  {
   //printf("Id isn't knowing\n");
    // return NULL if invalid arguments
    if (!fullPath)
    {
      LOG_RETURN( 0, "Invalid arguments in getDocumentIdByConditionAndPath\n");
    }
    name = strrchr(fullPath, PATH_DELIMITER);
    if (name == NULL)
    {
     //printf("path is null\n");
      name = fullPath;
      path = strdup("/");
    }
    else
    {
     //printf("path isn't null\n");
      ++name;
      path = (char *)malloc(name - fullPath + 1);
      if (path != NULL)
      {
        memset(path, 0, name - fullPath + 1);
        memcpy(path, fullPath, name - fullPath);
      }
    }

    if (path == NULL)
    {
      LOG_RETURN( 0, "Not enought memory in getDocumentIdByConditionAndPath\n");
    }


    if (condition)
    {
     //printf("have condition\n");
      whereSQL = appendString(whereSQL, whereSQL2, &err);
      outErr |= err;
      err = walkThroughCondition(condition, &whereSQL);
      if (err == 0)
      {
        selectSQL = appendString(selectSQL, whereSQL, &err);
       //printf("Select SQL final:%s\n",selectSQL);
        outErr |= err;
      }
      free(whereSQL);
      whereSQL = NULL;
    }
    else
    {
     //printf("haven't condtion\n");
      selectSQL = appendString(selectSQL, whereSQL1, &err);
     //printf("blablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablabla\n ");
     //printf("SELECT SQL:%s\n",selectSQL);
      outErr |=err;
      free(whereSQL);
      whereSQL = NULL;
    }
    if (outErr)
    {
     //printf("Error!!!\n");
      free(selectSQL);
      LOG_RETURN(0, "Can not create SQL query in %s %d\n", __FILE__, __LINE__);
    }
    r = sqlite3_prepare_v2(obj->db, selectSQL,  - 1, &stmt, &tail);
    if (r != SQLITE_OK)
    {
      free(selectSQL);
     //printf("SQL error:%s\n",selectSQL);
      LOG_RETURN(0, "sqlite3_prepare fail in %s %d\n", __FILE__, __LINE__);
    }
    r = sqlite3_bind_text(stmt, 1, path,  - 1, SQLITE_STATIC);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      free(path);
      free(selectSQL);
      LOG_RETURN(0, "sqlite3_bind_text fail in %s %d\n", __FILE__, __LINE__);
    }
    r = sqlite3_bind_text(stmt, 2, name,  - 1, SQLITE_STATIC);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      free(path);
      free(selectSQL);
      LOG_RETURN(0, "sqlite3_bind_text fail in %s %d\n", __FILE__, __LINE__);
    }
    r = sqlite3_step(stmt);

    if (r != SQLITE_ROW && r != SQLITE_DONE)
    {
      sqlite3_finalize(stmt);
      free(path);
      free(selectSQL);
      LOG_RETURN(0, "sqlite3_step fail in getDocumentIdByCondition with result %d\n", r);
    }
    doc_id = (r == SQLITE_ROW ? sqlite3_column_int64(stmt, 0) : 0);
   //printf("MY DOC_ID:%d\n",doc_id);
    sqlite3_finalize(stmt);
    if(name!=NULL)
       free(path);
    free(selectSQL);

  } // else id
 //printf("return\n");
  return doc_id;
}  
/**
 * Retrieve document attributes according to the document id
 * @param obj The sqlite database object
 * @param id The document id
 * @param attrLen The number of attributes return
 * @return Return a table of attributes
 */
InternalAttribute **sqliteRetrieveAttributes(SqliteDB *obj, sqlite3_int64 id, int *attrLen )
{

  char *sql = "SELECT NAME,STRING_VALUE,INT_VALUE,DATE_VALUE, ATTRIBUTE_TYPE FROM ATTRIBUTE WHERE DOCUMENT_ID = ?";
  InternalAttribute **res = NULL;
  int documentSize = 0;
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int i = 0;

  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    LOG_RETURN(NULL, "sqlite3_prepare fail in %s %d\n", __FILE__, __LINE__);
  }

  r = sqlite3_bind_int64(stmt, 1, id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(NULL, "sqlite3_bind_int64 fail in %s %d\n", __FILE__, __LINE__);
  }

  r = sqlite3_step(stmt);

  while (r == SQLITE_ROW)
  {
    const  char *name;
    const  char *string_value;
    int int_value = 0;
    const  char *date_value;
    int attr_type = 0;
    InternalAttribute *attr = (InternalAttribute *)malloc(sizeof(InternalAttribute));
    i++;
    name = (const char *)sqlite3_column_text(stmt, 0);
    string_value = (const char *)sqlite3_column_text(stmt, 1);
    int_value = sqlite3_column_int(stmt, 2);
    date_value = (const char *)sqlite3_column_text(stmt, 3);
    attr_type = sqlite3_column_int(stmt, 4);
    res = (InternalAttribute **) realloc(res, sizeof(InternalAttribute *)*i);
    attr->name = (name ? strdup(name) : NULL);
    attr->string_value = (string_value ? strdup(string_value) : NULL);
    attr->date_value = (date_value ? strdup(date_value) : NULL);
    attr->int_value = int_value;
    attr->type = attr_type;
    res[i - 1] = attr;
    r = sqlite3_step(stmt);
  }

  sqlite3_finalize(stmt);
  *attrLen = i;
  return res;
}

// alloc memory for document and place extracted document in 'document', in 'len' return size of document in bytes
/**
 * Retrieve document from database by given path or id
 * @param obj The sqlite database object
 * @param cfg The configuration
 * @param fullPath the path of the document
 * @param id the id of the document
 * @param condition the condition that the document must be checked
 */
SearchReturn *sqliteRetrieve(SqliteDB *obj,  ConfigList *cfg, const char *fullPath, sqlite3_int64 id, InternalCondition *condition)
{
  char *sql = "SELECT PATH,NAME,CONTENT FROM DOCUMENT WHERE ID = ?";
  SearchReturn *res = NULL;
  int documentSize = 0;
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  sqlite3_int64 doc_id;
  const unsigned char *document_path;
  const unsigned char *document_name;
  const unsigned char *document_content;
  subSearchResult *s1;
  InternalAttribute **attributes;
  int attrLen;
  doc_id = getDocumentIdByCondition(obj, fullPath, id, condition);
  if (!doc_id)
  {
    return NULL;
  }
  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    LOG_RETURN(NULL, "sqlite3_prepare fail in %s %d\n", __FILE__, __LINE__);
  }
  r = sqlite3_bind_int64(stmt, 1, doc_id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(NULL, "sqlite3_bind_int64 fail in %s %d\n", __FILE__, __LINE__);
  }
  r = sqlite3_step(stmt);
  if (r != SQLITE_ROW)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(NULL, "sqlite3_step fail in %s %d %d\n", __FILE__, __LINE__, r);
  }
  document_path = sqlite3_column_text(stmt, 0);
  document_name = sqlite3_column_text(stmt, 1);
  document_content = (const unsigned char *)sqlite3_column_blob(stmt, 2);
  documentSize = sqlite3_column_bytes(stmt, 2);
  s1 = createDocumentInMemory(doc_id, document_path, document_name, (signed char *)document_content, documentSize);
  sqlite3_finalize(stmt);
  if (s1 == NULL)
  {
    LOG_RETURN(NULL, "Can not create document in memory. %s %d\n", __FILE__, __LINE__);
  }
  res = createSearchReturn();
  if (res != NULL)
  {
    s1->attributes = sqliteRetrieveAttributes(obj, doc_id, &s1->attr_count);
    if (!appendToSearchReturn(res, s1))
    {
      emptySubSearchResult(s1);
      freeSearchReturn(res);
      LOG_RETURN(NULL, "Can not append document to search result. %s %d\n", __FILE__, __LINE__);
    }
  }
  else
  {
    emptySubSearchResult(s1);
    LOG_RETURN(NULL, "Can not create document in memory. %s %d\n", __FILE__, __LINE__);
  }
  return res;
}

/**
 * Create document into the database 
 * @param obj The sqlite database object
 * @param cfg The configuration
 * @param fullPath the path of the document
 * @param content the content of the document
 * @param len the length of the document content
 * @param attrs The table of document attributes
 * @param attrs_len the number of document attributes
 * @param doc_id the document id
 * @param condition the condition that the document must be checked
 * @return Return 101 if no problem occur
 */
int sqliteCreate(SqliteDB *obj, ConfigList *cfg, const char *fullPath, const unsigned char *content, int len, InternalAttribute **attrs, int attrs_len, sqlite3_int64 *doc_id)
{
 //printf("Function: sqliteCreate (Create the document)\n");
  int r = 0;
  int i = 0;
  int err = 0;
  const char *name;
  char *path = NULL;
  InternalAttribute *attr = NULL;
  sqlite3_int64 id;

  // return -1 if invalid arguments
  if (!fullPath || !content)
  {
    LOG_RETURN( - 1, "Invalid arguments in createDocument\n");
  }

  id = getDocumentIdByCondition(obj, fullPath, 0, NULL);
  if (id)
  {
    *doc_id = 0;
    return 0;
  }

  name = strrchr(fullPath, PATH_DELIMITER);
  if (name == NULL)
  {
    name = fullPath;
    path = strdup("/");
  }
  else
  {
    ++name;
    path = (char *)malloc(name - fullPath + 1);
    if (path != NULL)
    {
      memset(path, 0, name - fullPath + 1);
      memcpy(path, fullPath, name - fullPath);
    }
  }
  if (path == NULL)
  {
    LOG_RETURN( -2, "Can not malloc memory for path in sqliteCreate\n");
  }

  r = _createDocument(obj, path, name, content, len, doc_id);

  if (r == SQLITE_DONE)
  {
    if (attrs)
    {
      for (i = 0; i < attrs_len; i++)
      {
        attr = attrs[i];
        if (!attr->name || (r = createAttribute(obj, *doc_id, attr->name, attr->string_value, attr->int_value, attr->date_value, attr->type)) != SQLITE_DONE)
        {
          err = 2;
          LOG("Can not create attributes in function sqliteCreate, sql result = %d, %s %d \n", r, __FILE__, __LINE__, r);
          break;
        }
      }
    }
  }
  else
  {
    err = 1;
    LOG("Can not create document in function sqliteCreate (%s %s) %d\n", path, name, r);
  }

  if (err == 0)
  {
    free(path);
    return SQLITE_DONE;
  }
  free(path);
  return  - 2;
}

/**
 * Create document into the database.Internal function
 * @param obj The sqlite database object
 * @param path the path of the document
 * @param name the name of the document
 * @param content the content of the document
 * @param len the length of the document content
 * @param doc_id the document id
 * @return return -1 if invalid parameters, else if success return SQLITE_DONE = 101
 */
int _createDocument(SqliteDB *obj, const char *path, const char *name, const unsigned char *content, int len, sqlite3_int64 *doc_id)
{
  const char *sql = "INSERT INTO DOCUMENT(PATH, NAME, CONTENT) VALUES(?, ?, ?)";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    LOG_RETURN(r, "Can not sqlite3_prepare_v2 function _createDocument (%s %s),result= %d\n", path, name, r);
  }
  r = sqlite3_bind_text(stmt, 1, path,  - 1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not bind path in function _createDocument (%s %s),result= %d\n", path, name, r);
  }
  r = sqlite3_bind_text(stmt, 2, name,  - 1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not bind name in function _createDocument (%s %s),result= %d\n", path, name, r);
  }

  r = sqlite3_bind_blob(stmt, 3, content, len, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not bind content in function _createDocument (%s %s),result= %d\n", path, name, r);
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  *doc_id = sqlite3_last_insert_rowid(obj->db); // that is risky
  return r;
}

/**
 * Update document into the database 
 * @param obj The sqlite database object
 * @param cfg The configuration
 * @param fullPath the path of the document
 * @param doc_id the document id
 * @param content the content of the document
 * @param len the length of the document content
 * @param attrs The table of document attributes
 * @param attrs_len the number of document attributes
 * @param condition the condition that the document must be checked
 * @return Return 0 or -1 if problem occur
 */
int sqliteUpdate(SqliteDB *obj, ConfigList *cfg, const char *fullPath, sqlite3_int64 doc_id, const unsigned char *content, int len, InternalAttribute **attrs, int attrs_len, InternalCondition *condition)
{
 //printf("Function: sqliteUpdate\n");
  char *path = NULL;
  const char *name;
  int r = 0;
  int err = 0;
  int i = 0;
  int res = 0;
  InternalAttribute *attr = NULL;
  sqlite3_int64 id;

  id = getDocumentIdByCondition(obj, fullPath, doc_id, condition);
 //printf("return getDocumentIdByCondition\n");
  if (!id)
  {
    LOG_RETURN(0, "Not found document with this condition\n");
  }
  // return -1 if invalid arguments

  if (!fullPath)
  {
    LOG_RETURN( - 1, "Invalid arguments in updateDocument\n");
  }
  name = strrchr(fullPath, PATH_DELIMITER);
  if (name == NULL)
  {
    name = fullPath;
    path = strdup("/");
  }
  else
  {
    ++name;
    path = (char *)malloc(name - fullPath + 1);
    if (path != NULL)
    {
      memset(path, 0, name - fullPath + 1);
      memcpy(path, fullPath, name - fullPath);
    }
  }
  if (path == NULL)
  {
    LOG_RETURN( -2, "Can not malloc memory for path in sqliteCreate\n");
  }

  if (content)
  {
    r = _updateDocument(obj, id, content, len);
    if (r != SQLITE_OK)
    {
      LOG("Internal updateDocument return error %d in %s %d\n", r, __FILE__, __LINE__);
      err = r;
    }
    else
    {
      res = sqlite3_changes(obj->db);
    }
  }

  if (attrs && err == 0)
  {
    r = deleteAllAttributes(obj, NULL, id);
    if (r < 0)
    {
      LOG("Internal deleteAllAttributes return error %d in %s %d\n", r, __FILE__, __LINE__);
      err = r;
    }
    else
    {
      for (i = 0; i < attrs_len; i++)
      {
        attr = attrs[i];
        if (!attr->name || (r = createAttribute(obj, id, attr->name, attr->string_value, attr->int_value, attr->date_value, attr->type)) != SQLITE_DONE)
        {
          err = 2;
          LOG("Can not create attributes in %s %d\n", __FILE__, __LINE__);
          break;
        }
      }
    }
  }

  free(path);

  if (err != 0)
  {
    res = 0;
  }
  return res; // return count of changed documents
}

/**
 * Update document context by given path, name, context and length for context
 * @param obj The sqlite database object
 * @param doc_id the document id
 * @param content the content of the document
 * @param len the length of the document content
 * @return return -1 if invalid parameters, else if success return SQLITE_OK = 0
 */
int _updateDocument(SqliteDB *obj, sqlite3_int64 doc_id,  const unsigned char *content, int len)
{
  const char *sql = "UPDATE DOCUMENT SET CONTENT=? WHERE ID=?";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    return r;
  }

  r = sqlite3_bind_blob(stmt, 1, content, len, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return r;
  }
  r = sqlite3_bind_int64(stmt, 2, doc_id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return r;
  }

  r = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return SQLITE_OK; // sqlite3_changes(obj->db);
}


/**
 * Delete document by given id or path
 * @param obj The sqlite database object
 * @param path the path of the document
 * @param doc_id the document id
 * @return return -1 if invalid parameters, return -2 if sql error,else if success return SQLITE_OK = 0
 */
int deleteDocument(SqliteDB *obj, const char *path, sqlite3_int64 doc_id)
{
  const char *sql2 = "DELETE FROM DOCUMENT WHERE PATH=? ";
  const char *sql3 = "DELETE FROM DOCUMENT WHERE ID=? ";
  const char *sql = sql2;
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  if (doc_id > 0)
  {
    sql = sql3;
  }
  else
  {
    // return -1 if invalid arguments
    if (!path)
    {
      LOG_RETURN( - 1, "Invalid arguments in deleteDocument %s %d\n", __FILE__, __LINE__);
    }
  }

  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    LOG_RETURN( - 2, "Cannot prepare sql in deleteDocument, error = %d\n", r);
  }

  if (doc_id == 0)
  {
    r = sqlite3_bind_text(stmt, 1, path,  - 1, SQLITE_STATIC);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      LOG_RETURN( - 2, "Can not bind path in deleteDocument, error %d\n", r);
    }
  }
  else
  {
    r = sqlite3_bind_int64(stmt, 1, doc_id);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      LOG_RETURN( - 2, "Can not bind id in deleteDocument, error %d\n", r);
    }
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if (r == SQLITE_OK || r == SQLITE_DONE)
  {
    return sqlite3_changes(obj->db);
  }
  else
  {
    LOG_RETURN( - 2, "Can not execute sql in deleteDocument, error %d\n", r);
  }
}
/**
 * Delete exact document by given id or path
 * @param obj The sqlite database object
 * @param cfg The configuration
 * @param fullPath the path of the document
 * @param doc_id the document id
 * @param condition the condition that the document must be checked
 */
int sqliteDeleteExact(SqliteDB *obj, ConfigList *cfg, const char *fullPath, sqlite3_int64 doc_id, InternalCondition *condition)
{
  int r = 0;
  int err = 0;
  sqlite3_int64 id;

  id = getDocumentIdByCondition(obj, fullPath, doc_id, condition);

  if (!id)
  {
    LOG_RETURN(0, "Document not found for delete with path %s and id %d", fullPath, doc_id);
  }

  if (deleteAllAttributes(obj, NULL, id) < 0)
  {
    err = 1;
  }
  else
  {
    r = deleteDocument(obj, NULL, id);
    err = (r < 0 ? 1 : err);
  }

  if (!err)
  {
  }
  else
  {
    LOG("Error, Rollback transaction in deleteExcat\n");
    r = 0;
  }
  return r;
}

/**
 * Delete cascade document by given id or path
 * @param obj The sqlite database object
 * @param cfg The configuration
 * @param fullPath the path of the document
 * @param doc_id the document id
 * @param cascadeLevel Delete subtree or one level
 * @param condition the condition that the document must be checked
 */
int sqliteDeleteCascade(SqliteDB *obj, ConfigList *cfg, const char *fullPath, sqlite3_int64 doc_id, int cascadeLevel, InternalCondition *condition)
{
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int r =  - 1;
  int res = 0;
  sqlite3_int64 id;

  id = getDocumentIdByCondition(obj, fullPath, doc_id, NULL);
  if(!id && condition){
    LOG_RETURN(0, "Document with full path=%s or id=%d not exist but have conditions", fullPath, doc_id);
  }
  if(condition){
    id = getDocumentIdByCondition(obj, fullPath, doc_id, condition);
    if(!id){
      LOG_RETURN(0, "Nothing to delete in delete cascade with full path=%s or id=%d and cascade level=%d", fullPath, doc_id, cascadeLevel);
    }
  }
  switch (cascadeLevel)
  {
    case SCOPE_ONE_LEVEL:
      //Leave /nsc/apps/dataLogging/containers
      //Delete /nsc/apps/dataLogging/containers/[^/]*
      //Leave /nsc/apps/dataLogging/containers/[^/]*/[^/]*
      // delete all documents in path PATH+'/'+NAME+'/'   -> '/nsc/apps/dataLogging/containers/'
      // fullPath is without back slash, add it
    {
      char *p = preparePath(fullPath);
      if (p)
      {
        r = deleteAllAttributes(obj, p, 0);
        if (r >= 0)
        {
          r = deleteDocument(obj, p, 0);
          res = sqlite3_changes(obj->db);
        }
        free(p);
      }
      break;
    }
    case SCOPE_SUBTREE:
    {
      //Delete /nsc/apps/dataLogging/containers - exact document
      //Delete /nsc/apps/dataLogging/containers/[^/]*
      //Delete /nsc/apps/dataLogging/containers/[^/]*/[^/]*

      // delete all documents that path start with fullpath
      char *sqlAllDoc = "DELETE FROM DOCUMENT WHERE PATH GLOB ?";
      char *sqlAllDocAttributes = "DELETE FROM ATTRIBUTE WHERE EXISTS (SELECT 1 FROM DOCUMENT  where DOCUMENT.id = ATTRIBUTE.DOCUMENT_ID AND  DOCUMENT.PATH GLOB ?)";
      // fullPath is without back slash, add it and add star
      char *search = preparePathWithSlashAndStar(fullPath);
      // delete attributes for exact document
      if (id){
        r = deleteAllAttributes(obj, NULL, id);
      }else{
        r=0;
      }
      if (r >= 0)
      {
        if (id){
          r = deleteDocument(obj, NULL, id); // delete exact document
        }else{
          r=0;
        }
        if (r >= 0)
        {
          //delete all attributes
          r = sqlite3_prepare_v2(obj->db, sqlAllDocAttributes,  - 1, &stmt, &tail);
          if (r == SQLITE_OK)
          {
            r = sqlite3_bind_text(stmt, 1, search,  - 1, SQLITE_STATIC);
            if (r == SQLITE_OK)
            {
              // delete all document attributes
              r = sqlite3_step(stmt);
            }
            else
            {
              r = -1;
            }
          }
          else
          {
            LOG("Can not delete all atributes, error=%d\n", r);
            r = -1;
          }
          sqlite3_finalize(stmt);
          if (r == SQLITE_DONE)
          {

            // delete all other documents in subtree with LIKE (GLOB) function
            r = sqlite3_prepare_v2(obj->db, sqlAllDoc,  - 1, &stmt, &tail);
            if (r == SQLITE_OK)
            {
              r = sqlite3_bind_text(stmt, 1, search,  - 1, SQLITE_STATIC);
              if (r == SQLITE_OK)
              {
                // delete all documents
                r = sqlite3_step(stmt);
                if (r == SQLITE_DONE)
                {
                  res = sqlite3_changes(obj->db) + (!id?0:1); // +1 for exact document
                }
                else
                {
                  LOG("sqlite3_step return error %d in deletecascade\n", r);
                  r = -1;
                }
              }
              else
              {
                LOG("Cannot bind search to sql %s %s\n", search, sqlAllDoc, r);
                r = -1;
              }
            }
            else
            {
              LOG("Cannot prepare search %s, error=%d\n", search, r);
              r = -1;
            }
            sqlite3_finalize(stmt);
          }
          else
          {
            LOG("Can not delete all documents for path %s, error=%d\n", search, r);
            r = -1;
          }

        }
      }
      if (search != NULL)
      {
        free(search);
      }
      break;
    }
    default:
      LOG("Unknow cascade level in sqliteDeleteCascade");
      break;

  }

  if (r >= 0)
  {
  }
  else
  {
    LOG("Error, Rollback transaction in deleteCascade\n");
    res = r;
  }
  return res;

}

/**
 * create document attributes for given document id
 * @param obj The sqlite database object
 * @param doc_id the document id
 * @param attr_name the attribute name
 * @param string_value the string value of the attribute
 * @param int_value the int value of the attribute
 * @param date_value the date value of the attribute
 * @param attr_type the type of attribute
 * @return Return -1 if invalid parameters, else if success return SQLITE_DONE = 101
 */
int createAttribute(SqliteDB *obj, sqlite3_int64 document_id, const char *attr_name, const char *string_value, const int int_value, const char *date_value, const int attr_type)
{
 //printf("Function: createAttribute\nName:%s\tType:%d\nString\t->%s\nInt\t->%d\nDate\t->%s\n",attr_name, attr_type, string_value,int_value,date_value);
  const char *sql = "INSERT INTO ATTRIBUTE(DOCUMENT_ID, NAME, STRING_VALUE, INT_VALUE, DATE_VALUE, ATTRIBUTE_TYPE) VALUES(?, ?, ?, ?, ? ,?)";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    LOG_RETURN(r, "Can not prepare sql in _createAttribute, error = %d\nSQL:", r,sql);
  }

  r = sqlite3_bind_int64(stmt, 1, document_id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not sqlite3_bind_int64 %d in _createAttribute, error = %d\n", document_id, r);
  }

  r = sqlite3_bind_text(stmt, 2, attr_name,  - 1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not bind attr_name (%s) in _createAttribute, error = %d\n", attr_name, r);
  }

  r = sqlite3_bind_text(stmt, 3, string_value,  - 1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not bind string_value (%s) in _createAttribute, error = %d\n", string_value, r);
  }
  
  if(attr_type!=ATTR_TYPE_LIST_INTEGER && attr_type!=ATTR_TYPE_INTEGER){
    r = sqlite3_bind_null(stmt, 4);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      LOG_RETURN(r, "Can not bind int_value (%d) in _createAttribute, error = %d\n", int_value, r);
    }
  }else{
    r = sqlite3_bind_int(stmt, 4,  int_value);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      LOG_RETURN(r, "Can not bind int_value (%d) in _createAttribute, error = %d\n", int_value, r);
    }
  }

  r = sqlite3_bind_text(stmt, 5, date_value,  - 1, SQLITE_STATIC);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not bind date_value (%s) in _createAttribute, error = %d\n", date_value, r);
  }

  r = sqlite3_bind_int(stmt, 6,  attr_type);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    LOG_RETURN(r, "Can not bind attr_type (%d) in _createAttribute, error = %d\n", attr_type, r);
  }
  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  return r;
}

/**
 * delete all the document attributes 
 * @param obj The sqlite database object
 * @param document_path the path of the document
 * @param doc_id the document id
 * @return Return -1 if invalid parameters,return -2 if sql error, else if success return SQLITE_DONE = 101
 */
int deleteAllAttributes(SqliteDB *obj, const char *document_path, sqlite3_int64 doc_id)
{
  const char *sql2 = "DELETE FROM ATTRIBUTE WHERE EXISTS ( SELECT DOCUMENT.ID FROM DOCUMENT WHERE DOCUMENT.ID = ATTRIBUTE.DOCUMENT_ID AND DOCUMENT.PATH=? ) ";
  const char *sql_id = "DELETE FROM ATTRIBUTE WHERE DOCUMENT_ID=? ";
  const char *sql = sql2;
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;

  // return -1 if invalid arguments
  if (doc_id == 0)
  {
    if (!document_path)
    {
      LOG_RETURN( - 1, "Invalid arguments in deleteAllAttributes\n");
    }
  }
  else
  {
    sql = sql_id;
  }


  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    LOG_RETURN( - 2, "Can not prepare sql in deleteAllAttributesInternal, error = %d\n", r);
  }

  if (doc_id > 0)
  {
    r = sqlite3_bind_int64(stmt, 1, doc_id);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      return  - 2;
    }

  }
  else
  {
    if (document_path == NULL)
    {
      sqlite3_finalize(stmt);
      LOG_RETURN( - 1, "Memory error, can not allocate memory for preparePath  in %s %d\n", __FILE__, __LINE__);
    }

    r = sqlite3_bind_text(stmt, 1, document_path,  - 1, SQLITE_STATIC);
    if (r != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      return  - 2;
    }
  }

  r = sqlite3_step(stmt);

  sqlite3_finalize(stmt);
  if (r == SQLITE_OK || r == SQLITE_DONE)
  {
    return sqlite3_changes(obj->db);
  }
  else
  {
    return  - 2;
  }
}

// transactions API
/**
 * Begin sql transaction
 * @param obj The sqlite database object
 * @return Return 0 if no problem occur
 */
int beginTransaction(SqliteDB *obj)
{
  int r;
  char *error = "";
  r =  sqlite3_exec(obj->db, "BEGIN", 0, 0, &error);
  if(r != SQLITE_OK)
  {
    LOG_RETURN(r,"Begin error:%s\n",error);
  }else{
    free(error);
    return r;
  }
}
/**
 * Commit sql transaction
 * @param obj The sqlite database object
 * @return Return 0 if no problem occur
 */
int commitTransaction(SqliteDB *obj)
{ 
  int r;
  char *error = "";
  r =  sqlite3_exec(obj->db, "COMMIT", 0, 0, &error);
  if(r != SQLITE_OK)
  {
    LOG_RETURN(r,"Commit error:%s\n",error);
  }else{
    free(error);
    return r;
  }
}
/**
 * Rollback sql transaction
 * @param obj The sqlite database object
 * @return Return 0 if no problem occur
 */
int rollbackTransaction(SqliteDB *obj)
{
  int r;
  char *error = "";
  r =  sqlite3_exec(obj->db, "ROLLBACK", 0, 0, &error);
  if(r != SQLITE_OK)
  {
    LOG_RETURN(r,"Rollback error:%s\n",error);
  }else{
    free(error);
    return r;
  }
}

/**
 * Count the number of page in the DB
 * @param obj The sqlite database object
 * @param pages The number of pages
 * @return Return 0 if no problem occur
 */
int getPageCount(SqliteDB *obj, int *pages)
{
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int r;

  *pages = 0;
  r = sqlite3_prepare_v2(obj->db, "pragma page_count",  - 1, &stmt, &tail);
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

/**
 * Count the number of freelist
 * @param obj The sqlite database object
 * @param pages The number of freelist
 * @return Return 0 if no problem occur
 */
int getFreeListCount(SqliteDB *obj, int *count)
{
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int r;
  *count = 0;
  r = sqlite3_prepare_v2(obj->db, "pragma freelist_count",  - 1, &stmt, &tail);
  if (r == SQLITE_OK)
  {
    r = sqlite3_step(stmt);
    if (r == SQLITE_ROW)
    {
      *count = sqlite3_column_int(stmt, 0);
      r = SQLITE_OK;
    }

  }
  sqlite3_finalize(stmt);
  return r;
}
/**
 * Set the max page count
 * @param obj The sqlite database object
 * @param pages The max of page count
 * @return Return 0 if no problem occur
 */
int setMaxPageCount(SqliteDB *obj, int maxPageCount)
{

  char buffer[512];
  int r;
  sprintf(buffer, "pragma max_page_count = %d", maxPageCount);
  r = sqlite3_exec(obj->db, buffer, 0, 0, 0);
  return r;
}

/**
 * Init sub search result
 * @param res The subsearch result
 * @param id The document id
 * @param path the document path
 * @param name the document name
 * @param content the document content
 * @param contentLength The content length
 * @return Return 0 if no problem occur
 */
void initSubSearchResult(subSearchResult *res, sqlite3_int64 id, const /*unsigned */char *path, const /* unsigned*/char *name, signed char *content, int contentLength)
{
  int pathLength = 0;
  int nameLength = 0;

  if (res == NULL)
  {
    LOG("Null parameter subSearchResult in initSubSearchResult\n");
    return ;
  }

  res->id = id;
  res->name = res->path = NULL;
  res->content = NULL;
  res->contentLength = 0;
  res->attributes = NULL;
  res->attr_count = 0;

  if (path == NULL || strlen(path) == 0)
  {
    LOG("Invalid arguments in initSubSearchResult\n");
    return ;
  }
  pathLength = strlen(path) + 1;

  res->path = (char *)malloc(pathLength);
  if (res->path)
  {
    memset(res->path, 0, pathLength);
    memcpy(res->path, path, pathLength);
    if (name != NULL && strlen(name) > 0)
    {
      nameLength = strlen(name) + 1;
      res->name = (char *)malloc(nameLength);
      if (res->name)
      {
        memset(res->name, 0, nameLength);
        memcpy(res->name, name, nameLength);
      }
      else
      {
        LOG("Can not malloc memory in initSubSearchResult for name\n");
      }
    }

    if (contentLength > 0)
    {
      res->content = (signed char *)malloc(contentLength);
      if (res->content)
      {
        memcpy(res->content, content, contentLength);
        res->contentLength = contentLength;
      }
      else
      {
        LOG("Can not malloc memory in initSubSearchResult for content\n");
      }
    }
  }
  else
  {
    LOG("Can not malloc memory in initSubSearchResult for path\n");
  }
}
/**
 * Delete data in table of attributes
 * @param res The table of attributes
 * @param len The number of attributes
 */
void freeAttributes(InternalAttribute **res, int len)
{
  int i;
  InternalAttribute *attr;
  if (res == NULL)
  {
    return ;
  }
  for (i = 0; i < len; i++)
  {
    attr = res[i];
    free(attr->name);
    free(attr->string_value);
    free(attr->date_value);
    free(attr);
  }
  free(res);
}

/**
 * Delete data in subsearch result
 * @param res The subsearch result
 */
void emptySubSearchResult(subSearchResult *res)
{
  if (res == NULL)
  {
    return ;
  }
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
  freeAttributes(res->attributes, res->attr_count); // after this function res->attribute is invalid
  res->attributes = NULL;
  res->attr_count = 0;
}
/**
 * Add attributes to the list of attributes
 * @param list The old list of attributes
 * @param len the length of the list
 * @param attr_name the name of the new attribute
 * @param attr_type the type of the new attribute
 * @param string_value the string value of the new attribute
 * @param int_value the int value of the new attribute
 * @param date_value the date value of the new attribute
 * @return Return the new list of attributes
 */
InternalAttribute **appendAttributeToList(InternalAttribute **list, int len, char *attr_name, int attr_type, char *string_value, int int_value, char *date_value)
{
  InternalAttribute **res;
  InternalAttribute *attr;
  res = (InternalAttribute ** )realloc(list, sizeof(InternalAttribute *) * (len + 1));
  if (!res)
  {
    LOG_RETURN(NULL, "Cannot append attribute to list, not enought memory");
  }
  attr = (InternalAttribute *)malloc(sizeof(InternalAttribute));
  if (!attr)
  {
    LOG_RETURN(NULL, "Cannot append attribute to list, not enought memory");
  }
  memset(attr, 0, sizeof(InternalAttribute));
  attr->date_value = (date_value ? strdup(date_value) : NULL);
  attr->int_value = int_value;
  attr->name = (attr_name ? strdup(attr_name) : NULL);
  attr->type = attr_type;
  attr->string_value = (string_value ? strdup(string_value) : NULL);
  res[len] = attr;
  return res;
}
/**
 * Get attributes to the document and append it to the search result
 * @param obj the sqlite database object
 * @param subSearchResult the sub search result 
 * @param attributes the table of the attributes name 
 * @param attr_len the length of the attributes table
 * @return return 1 if no problem occur
 */
int appendAttributesToDocument(SqliteDB *obj, subSearchResult *s, char **attributes, int attr_len)
{
  char *sql = "SELECT NAME,STRING_VALUE, INT_VALUE, DATE_VALUE, ATTRIBUTE_TYPE FROM ATTRIBUTE WHERE DOCUMENT_ID = ?\0";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  int i;
  int err = 0;
  char *format = " AND NAME IN (\0";
  char *selectSQL = NULL;

  if (!s)
  {
    return 0;
  }

  // attributes can not be null, but attr_len can be < 0 if we dont need attributes
  // if attr_len == 0 we need all attributes
  if (attributes && attr_len < 0)
  {
    return 1;  // if empty array dont append attributes, return TRUE
  }

  selectSQL = appendString(selectSQL, sql, &err);
  if (err)
  {
    if (selectSQL)
    {
      free(selectSQL);
    }
    LOG_RETURN(0, "Memory error in appendAttributesToDocument\n");
  }

  if (attributes && attr_len > 0)
  {
    int len = strlen(format) + 2 * attr_len + 1;
    char *p = (char *) malloc(len);  // ?,?,?) = 6 = 2*3 -1
    memset(p, 0, len);

    memcpy(p, format, strlen(format));
    for (i = 0; i < attr_len; i++)
    {
      p[strlen(format) + 2 * i] = '?';
      p[strlen(format) + 2 * i + 1] = ',';
    }
    p[strlen(format) + 2 * i - 1] = ')';
    selectSQL = appendString(selectSQL, p, &err);
    free(p);
    if (err)
    {
      free(selectSQL);
      LOG_RETURN(0, "Memory error in appendAttributesToDocument");
    }
  }

  r = sqlite3_prepare_v2(obj->db, selectSQL, -1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    free(selectSQL);
    LOG_RETURN(0, "sqlite3_prepare_v2 fail in appendAttributesToDocument with result %d", r);
  }

  r = sqlite3_bind_int64(stmt, 1, s->id);
  if (r != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    free(selectSQL);
    LOG_RETURN(0, "sqlite3_bind_int64 fail in appendAttributesToDocument with result %d", r);
  }

  if (attributes && attr_len > 0)
  {
    for (i = 0; i < attr_len; i++)
    {
      char *p = (char *)malloc(strlen(attributes[i]) + 3);
      if (p == NULL)
      {
        sqlite3_finalize(stmt);
        free(selectSQL);
        LOG_RETURN(0, "Memory error in appendAttributesToDocument, cannot allock memory for attribute name");
      }
      memset(p, 0, strlen(attributes[i]) + 3);
      sprintf(p, "%s", attributes[i]);

      r = sqlite3_bind_text(stmt, i + 2, p,  - 1, SQLITE_TRANSIENT);
      free(p);
      if (r != SQLITE_OK)
      {
        sqlite3_finalize(stmt);
        free(selectSQL);
        LOG_RETURN(0, "sqlite3_bind_text fail in appendAttributesToDocument with result %d ", r);
      }
    }

  }
  r = sqlite3_step(stmt);
  if (r != SQLITE_ROW && r != SQLITE_DONE)
  {
    sqlite3_finalize(stmt);
    free(selectSQL);
    LOG_RETURN(0, "sqlite3_step error %d\n", r);
  }
  while (r == SQLITE_ROW)
  {
    InternalAttribute **list = appendAttributeToList(s->attributes, s->attr_count, (char *)sqlite3_column_text(stmt, 0), sqlite3_column_int(stmt, 4),
        (char *)sqlite3_column_text(stmt, 1), sqlite3_column_int(stmt, 2),
        (char *)sqlite3_column_text(stmt, 3));
    if (list)
    {
      s->attributes = list;
      s->attr_count++;
    }
    r = sqlite3_step(stmt);
  }

  sqlite3_finalize(stmt);
  free(selectSQL);

  if ( r != SQLITE_OK && r != SQLITE_DONE)
  {
    LOG_RETURN(0, "read attribute values fail in appendAttributesToDocument with result %d", r);
  }
  return 1; // return TRUE;
}


/**
 * create and initialize search document
 * @param id the document id
 * @param path the document path
 * @param name the document name
 * @param content the document content
 * @param contentLength the content length
 * @return Return a search document
 */
subSearchResult *createDocumentInMemory(sqlite3_int64 id, const unsigned char *path, const unsigned char *name, signed char *content, int contentLength)
{
  subSearchResult *res = (subSearchResult *)malloc(sizeof(subSearchResult));
  if (res)
  {
    initSubSearchResult(res, id, (const char *)path, (const char *)name, content, contentLength);
  }
  else
  {
    LOG("Can not malloc memory in createDocumentInMemory for subSearchResult\n");
  }
  return res;
}
/**
 * Create empty search result
 * @return Return a SearchReturn
 *
 */
SearchReturn *createSearchReturn()
{
  SearchReturn *res = (SearchReturn *)malloc(sizeof(SearchReturn));
  if (res)
  {
    res->allocated = 0;
    res->size = 0;
    res->data = NULL;
    res->hasMore = 0;
    res->result =  - 1;

  }
  else
  {
    LOG("Can not malloc memory in createSearchReturn for SearchReturn\n");
  }
  return res;

}

/**
 * Append a document search to the search return
 * @param res the search return
 * @param data the document search
 * @return Return 1 if no problem occur
 */
int appendToSearchReturn(SearchReturn *res, subSearchResult *data)
{
  if (data == NULL || res == NULL)
  {
    LOG_RETURN(0, "Invalid arguments in appendToSearchReturn\n");
  }
  if (res->data == NULL)
  {
    res->data = (subSearchResult **)malloc(5 * sizeof(data));
    if (res->data)
    {
      res->allocated += 5;
    }
    else
    {
      LOG_RETURN(0, "Can not malloc memory in appendToSearchReturn for data\n");
    }
  }

  if (res->data == NULL)
  {
    return  - 1;
  }

  if (res->size == res->allocated)
  {
    res->data = (subSearchResult **)realloc(res->data, res->allocated * sizeof(data) + 5 * sizeof(data));
    if (res->data)
    {
      res->allocated += 5;
    }
    else
    {
      LOG_RETURN(0, "Can not realloc memory in appendToSearchReturn for data\n");
    }

  }
  res->data[res->size++] = data;
  return 1;
}

/**
  * Delete data into SearchReturn 
  * @param data the search return
  */
void freeSearchReturn(SearchReturn *data)
{
  int i = 0;
  if (data == NULL)
  {
    return ;
  }
  for (i = 0; i < data->size; i++)
  {
    emptySubSearchResult(data->data[i]);
    free(data->data[i]);

  }

  free(data->data);
  free(data);
}

/**
 * Search document and return a list of document 
 * @param obj The sqlite database object
 * @param cfg The configuration
 * @param basePath the path of the document
 * @param scope subtree or one level
 * @param condition the condition that the document must be checked
 * @param order how order the list
 * @param limit Limit the list with n result
 * @param withContent get content to the list of Document
 * @param attributes The table of document attributes
 * @param attrs_len the number of document attributes
 * @return Return null if a probleme occur
 */
SearchReturn *search(SqliteDB *obj, ConfigList *cfg, const char *basePath, int scope, InternalCondition *condition, int order, int limit, int withContent, char **attributes, int attr_len)
{
  SearchReturn *res = NULL;
  char *sql = "";
  int r = 0;
  sqlite3_stmt *stmt = NULL;
  const char *tail = 0;
  //char *p = NULL;
  sqlite3_int64 document_id = 0;
  char *document_path;
  char *document_name;
  char *document_content = NULL;
  int documentSize = 0;
  int rowCount = 0;


  // return NULL result if invalid arguments
  if (!basePath)
  {
    LOG("Invalid arguments in search\n");
    return res;
  }
  //extend path  with slash if needed

  sql = getSearchSqlText(basePath, scope, condition, order, limit);
  if (sql == NULL)
  {
    // free(p);
    LOG("Can not create sql string in search %s %d\n", __FILE__, __LINE__);
    return res;
  }
  r = sqlite3_prepare_v2(obj->db, sql,  - 1, &stmt, &tail);
  if (r != SQLITE_OK)
  {
    LOG("Can not prepare sql string in search  %d %s\n", r, sql);
    // free(p);
    free(sql);
    return res;
  }
  r = sqlite3_step(stmt);
  res = createSearchReturn();
  if (res != NULL)
  {
    while (r == SQLITE_ROW && res != NULL)
    {
      subSearchResult *s1;
      int i;
      // first get text result and next calculate size!!

      document_id = sqlite3_column_int64(stmt, 0);
      // printf("id:%d\t",document_id);
      document_path = sqlite3_column_text(stmt, 1);
      document_name = sqlite3_column_text(stmt, 2);
      // printf("current:%s%s\n",document_path,document_name);
      if (withContent)
      {
        document_content = (const unsigned char *)sqlite3_column_blob(stmt, 3);
        documentSize = sqlite3_column_bytes(stmt, 3);
      }
      s1 = createDocumentInMemory(document_id, document_path, document_name, (signed char *)document_content, documentSize);
      if (s1 == NULL)
      {
        LOG("Can not create document in memory. %s %d\n", __FILE__, __LINE__);
        r =  - 1;
        break;
      }

      if (!appendAttributesToDocument(obj, s1, attributes, attr_len))
      {
        LOG("Can not append attributes to document. %s %d\n", __FILE__, __LINE__);
        r =  - 1;
        break;
      }

      if (!appendToSearchReturn(res, s1))
      {
        LOG("Can not append document to search result. %s %d\n", __FILE__, __LINE__);
        r =  - 1;
        break;
      }
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
      freeSearchReturn(res);
      res = NULL;
      LOG("Error in executing sql in search function, sqlite error code = %d \n", r);
    }
  }
  sqlite3_finalize(stmt);
  // free(p);
  free(sql);

  return res;
}
/**
 * Update juste a part of a document
 * @param obj The sqlite database object
 * @param cfg The configuration
 * @param fullPath the path of the document
 * @param doc_id the document iod
 * @param content the document content
 * @param content_len the content len
 * @param attrOps the list of attributes operation
 * @param oper_len the number of attributes operation
 * @param condition the condition that the document must be checked
 * @return Return 1 if no problem occur
 */
int sqlitePartialUpdate(SqliteDB *obj, ConfigList *cfg, char *fullPath, sqlite3_int64 doc_id, const unsigned char *content, int content_len, InternalAttrOperation **attrOps, int oper_len, InternalCondition *condition)
{
 //printf("Function: sqlitePartialUpdate\nTable size: %d\n",oper_len);
  char *deleteAttribute = "DELETE FROM ATTRIBUTE WHERE DOCUMENT_ID= ? AND NAME = ?";
  char *updateAttribute = "UPDATE ATTRIBUTE SET STRING_VALUE = ?, INT_VALUE= ?, DATE_VALUE = ? WHERE DOCUMENT_ID = ? AND NAME = ?";
  int r =  - 1;
  int err = 0;

  sqlite3_int64 id;

  id = getDocumentIdByCondition(obj, fullPath, doc_id, condition);
  if (!id)
  {
    return 0;
  }

  // internalTransactionBegin(obj, GET_TRANSACTION_NAME);
  if (content && content_len >= 0)
  {
    r = _updateDocument(obj, id, content, content_len);
    if (r != SQLITE_OK)
    {
      LOG("sqlitePartialUpdate return error %d in %s %d\n", r, __FILE__, __LINE__);
      err = r;
    }
  }

  if (!err && attrOps)
  {
    int i = 0;
    sqlite3_stmt *stmt = NULL;
    const char *tail = 0;
    InternalAttrOperation *oper;

    r = sqlite3_prepare_v2(obj->db, deleteAttribute, -1, &stmt, &tail);
    if (r != SQLITE_OK)
    {
      err = 1;
      LOG("Can not prepare sql statement for delete attributes in sqlitePartialUpdate, result is %d", r);
    }
    else
    {

      for (i = 0; i < oper_len; i++)
      {
        oper = attrOps[i];
       //printf("Name:%s\tType:%d\nString\t->%s\nInt\t->%d\nDate\t->%s\n",oper->attribute->name,oper->type,oper->attribute->string_value,oper->attribute->int_value,oper->attribute->date_value);
        //Delete all attributes
        r = sqlite3_bind_int64(stmt, 1, id);
        if (r != SQLITE_OK)
        {
          err = 1;
          LOG("Can not bind document ID in sqlitePartialUpdate, result is %d", r);
          break;
        }
        r = sqlite3_bind_text(stmt, 2, oper->attribute->name, -1, SQLITE_STATIC);
        if (r != SQLITE_OK)
        {
          err = 1;
          LOG("Can not bind attribute name in sqlitePartialUpdate, result is %d", r);
          break;
        }
        r = sqlite3_step(stmt);
        if (r != SQLITE_OK && r != SQLITE_DONE)
        {
          err = 1;
          LOG("Can not delete attribute with name %s in sqlitePartialUpdate, result is %d", oper->attribute->name, r);
          break;
        }
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
      }
      sqlite3_finalize(stmt);
      for (i = 0; i < oper_len; i++)
      {
        oper = attrOps[i];
        if (oper->type == TYPE_SET)
        {
          r = createAttribute(obj, id, oper->attribute->name, oper->attribute->string_value, oper->attribute->int_value, oper->attribute->date_value, oper->attribute->type);
          if (r != SQLITE_OK && r != SQLITE_DONE)
          {
            err = 1;
            LOG("Can not update attribute with name %s in sqlitePartialUpdate, result is %d", oper->attribute->name, r);
            break;
          }
        }
      }
    }
  }
  if (err)
  {
    LOG("Error, Rollback transaction in sqlitePartialUpdate\n");
    //printf("Error : rollback");
    return 0;
  }
  else
  {
   //printf("Success : commit");
    return 1;
  }
}
/**
 * Convert condition value to a where of sql query 
 * @param cond the condition
 * @param whereSQL the sql query
 * @param err if error occur
 * @param oper the condition operator
 */
char *walk_DATE_INTEGER(const InternalCondition *cond, char *whereSQL, int *err, char *oper)
{
  char *w = NULL;
  char *buffer = (char *)malloc(strlen(oper) + strlen(cond->operand) + 50);
  if (!buffer)
  {
    LOG_RETURN(0, "Can not malloc memory for buffer in walk_DATE_INTEGER\n");
  }

  if (cond->type == ATTR_TYPE_DATE)
  {
    sprintf(buffer, "DATE_VALUE %s '%s'", oper, cond->operand);
    w = appendString(whereSQL, buffer, err);
    if (!w)
    {
      LOG("Can not appendString in walk_DATE_INTEGER %s %d\n", __FILE__, __LINE__);
    }
  }
  else if (cond->type == ATTR_TYPE_INTEGER)
  {
    sprintf(buffer, "INT_VALUE %s %s", oper, cond->operand);
    w = appendString(whereSQL, buffer, err);
    if (!w)
    {
      LOG("Can not appendString in walk_DATE_INTEGER %s %d\n", __FILE__, __LINE__);
    }
  }
  else
  {
    if (cond->oper == ATTR_OP_STARTS_WITH)
    {
      sprintf(buffer, "STRING_VALUE %s '%s*'", oper, cond->operand);
    }
    else
    {
      sprintf(buffer, "STRING_VALUE %s '%s'", oper, cond->operand);
    }
    w = appendString(whereSQL, buffer, err);
    if (!w)
    {
      LOG("Can not appendString in walk_DATE_INTEGER %s %d\n", __FILE__, __LINE__);
    }
  }
  free(buffer);
  return w;
}

/**
 * Convert condition to a where of sql query 
 * @param cond the condition
 * @param wSQL the sql query
 * @return Return 0 if no problem occur
 */
int walkThroughCondition(const InternalCondition *cond, char **wSQL){
  int res = 0;
  int err = 0;
  char *whereSQL =  *wSQL;
  static char *format_operator_equal = " id in ( select document_id as \"id\" from attribute where NAME = '%s' AND ";

  if(cond->operand==NULL){
       whereSQL = appendString(whereSQL, " ( ", &err);
       res = res | err;
       int forCounter = 0;
       for(forCounter = 0 ; forCounter < cond->length; forCounter++){
           res = res | walkThroughCondition(cond->conditions[forCounter], &whereSQL);
           whereSQL = appendString(whereSQL, forCounter!=cond->length-1?(cond->oper==COND_OP_AND?" AND ":" OR "):" ", &err);
         res = res | err;
       }
  }else{
     char *strTemp = (char *)malloc(strlen(format_operator_equal) - 2 + strlen(cond->attributeName) + 1);
     if (strTemp)
     {
       sprintf(strTemp, format_operator_equal, cond->attributeName);
       whereSQL = appendString(whereSQL, strTemp, &err);
       free(strTemp);
       res |= err;
     }
     else
     {
       LOG("Can not malloc memory in walkThroughCondition %s %d\n", __FILE__, __LINE__);
     }

     char* operator;
     switch(cond->oper){
        case ATTR_OP_EQUAL:
          operator = "=";           
        break;
        case ATTR_OP_NOT_EQUAL:
          operator = "!=";
        break;
        case ATTR_OP_STRICTLY_GREATER:
          operator = ">";
        break;
        case ATTR_OP_LOWER_OR_EQUAL:
          operator = "<=";
        break;
        case ATTR_OP_STRICTLY_LOWER:
          operator = "<";
        break;
        case ATTR_OP_GREATER_OR_EQUAL:
          operator = ">=";
        break;
        case ATTR_OP_STARTS_WITH:
          operator = "GLOB";
        break;
        default:
          LOG("Unknown operator in walkThroughCondition %s %d\n", __FILE__, __LINE__);
          return  - 666;
        break;
     }
     whereSQL = walk_DATE_INTEGER(cond, whereSQL, &err, operator);
     res |= err;
  }
  *wSQL = whereSQL;
  *wSQL = appendString(*wSQL, ")", &err);
  res |= err;
  return res;
}

/**
 * Create a sql query from the search attributes
 * @param path the search path
 * @param scope search in one level or sub tree
 * @param condition the condition for the search
 * @param order how sort the search
 * @param limit number of result in the search
 * @return Return the sql query if no problem occur
 */
char *getSearchSqlText(char *path, int scope, InternalCondition *condition, int order, int limit)
{
  static char *format_ordering = "ORDER BY PATH||NAME %s";
  char *startSQL1 = "SELECT DISTINCT ID, PATH, NAME, CONTENT FROM DOCUMENT ";
  char *whereSQL1 = " WHERE (PATH ";
  char *selectSQL = NULL;
  char *whereSQL = NULL;
  char *ordering = NULL;
  char limiting[255] = "";
  char *res = NULL;
  int err;
  int outErr = 0;
  char *strTemp;
  char *p = NULL;
  p = preparePath(path);
  
  if (p == NULL)
  {
    LOG("Memory error, can not allocate memory for preparePath  in %s %d\n", __FILE__, __LINE__);
    res = NULL; 
    return res;
  }
  selectSQL = appendString(selectSQL, startSQL1, &err);
  outErr |= err;
  whereSQL = appendString(whereSQL, whereSQL1, &err);
  outErr |= err;

  if (scope == SCOPE_ONE_LEVEL)
  {
    char *buffer = (char *)malloc(strlen(p) + 11);
    if (buffer)
    {
      sprintf(buffer, "='%s')", p);

      whereSQL = appendString(whereSQL, buffer, &err);
      free(buffer);
    }
    else
    {
      err =  - 1;
    }
  }
  else if (scope == SCOPE_SUBTREE)
  {
      const char *name;
      char *newPath = NULL;
      name = strrchr(path, PATH_DELIMITER);
      if (name == NULL)
      {
       name = path;
       newPath = strdup("/");
      }
      else
      {
       ++name;
       newPath  = (char *)malloc(name - path + 1);
       if (newPath  != NULL)
       {
         memset(newPath , 0, name - path + 1);
         memcpy(newPath , path, name - path);
       }
      }
      if(newPath==NULL){
        LOG("Memory error, can not allocate memory for preparePath  in %s %d\n", __FILE__, __LINE__);
        res = NULL; 
        return res;
      }
      char *buffer = (char *)malloc(strlen(path) + 15 + 29+strlen(newPath)+strlen(name));
    if (buffer)
    {
      sprintf(buffer, " GLOB '%s*' OR (PATH = '%s' AND NAME ='%s')) ", p, newPath,name);
      whereSQL = appendString(whereSQL, buffer, &err);
      free(buffer);
      if(newPath!=NULL)
        free(newPath);
    }
    else
    {
      err =  - 1;
    }
  }
  else
  {
    err =  - 1;
  }

  outErr |= err;

  if (condition)
  {
    whereSQL = appendString(whereSQL, " AND ", &err);
    outErr |= err;
    outErr |= walkThroughCondition(condition, &whereSQL);
  }


  ordering = (char *)malloc(strlen(format_ordering) - 4 + 12 + 1);

  if (ordering)
  {
    if (order == ORDER_DESC)
    {
      sprintf(ordering, format_ordering," DESC ");
    }
    else
    {
      if (order == ORDER_ASC)
      {
        sprintf(ordering, format_ordering, " ASC ");
      }
      else
      {
        // printf("NO ORDERING\n");
        sprintf(ordering,"%s",""); 
      }
    }
  }
  else
  {
    outErr |=  - 1;
  }


  if (limit > 0)
  {
    sprintf(limiting, " LIMIT %d ", limit);
  }

  strTemp = (char *)malloc(strlen(selectSQL) + strlen(whereSQL) + strlen(ordering) + strlen(limiting) + 5);
  if (strTemp)
  {
    sprintf(strTemp, "%s %s %s %s", selectSQL, whereSQL, ordering, limiting);
    res = appendString(res, strTemp, &err);
    free(strTemp);
  }
  else
  {
    err =  - 1;
  }
  outErr |= err;
  free(selectSQL);
  free(whereSQL);
  free(ordering);
  if (outErr != 0)
  {
    free(res);
    res = NULL;
    LOG("Can not malloc memory in getSearchSqlText %s %d\n", __FILE__, __LINE__);
  }
  return res;
}