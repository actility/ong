#ifndef sqllite_driver_h_
#define sqllite_driver_h_

#ifdef __cplusplus
extern "C" {
#endif


#define PATH_DELIMITER '/'
#define STAR_CHAR '*'

/*#define TYPE_CREATE 8
#define TYPE_UPDATE 9
#define TYPE_DELETE 10
*/
  //search constants
  //#define SCOPE_EXACT 1
#define SCOPE_ONE_LEVEL 0
#define SCOPE_SUBTREE 1
#define NO_LIMIT -1

#define ORDER_ASC 1
#define ORDER_DESC 2
#define ORDER_UNKNOWN 0

  // operations for condition
#define ATTR_OP_EQUAL 0
#define ATTR_OP_NOT_EQUAL 1
#define ATTR_OP_STRICTLY_GREATER 2
#define ATTR_OP_LOWER_OR_EQUAL 3
#define ATTR_OP_STRICTLY_LOWER 4
#define ATTR_OP_GREATER_OR_EQUAL 5
#define ATTR_OP_STARTS_WITH 6
#define COND_OP_AND 0
#define COND_OP_OR 1

#define SQLITE_OK 0

  // types of operand in condition
#define ATTR_TYPE_INTEGER 0
#define ATTR_TYPE_STRING 1
#define ATTR_TYPE_DATE 2
#define ATTR_TYPE_LIST_INTEGER 3
#define ATTR_TYPE_LIST_STRING 4
#define ATTR_TYPE_LIST_DATE 5

  // operation types
#define TYPE_SET  0
#define TYPE_UNSET 1

  typedef struct ConfigItem
  {
    char *key;
    char *value;
  } ConfigItem;

  typedef struct ConfigList
  {
    ConfigItem **items;
    int length;
  } ConfigList;

  typedef struct InternalAttribute
  {
    char *name;
    int type;
    char *string_value;
    int  int_value;
    char *date_value;
  } InternalAttribute;

  typedef struct InternalAttrOperation
  {
    int type; //OPERATION_SET /UNSET
    InternalAttribute *attribute;
  } InternalAttrOperation;

  typedef struct InternalCondition
  {
    char *attributeName;
    int type;
    int oper; // operator
    int length; //Liste length
    char *operand;
    /*struct InternalCondition *condition1;
    struct InternalCondition *condition2;*/
    struct InternalCondition** conditions;


  } InternalCondition;

  typedef struct
  {
    sqlite3_int64 id;
    char *path;
    char *name;
    signed char *content;
    int contentLength;
    InternalAttribute **attributes;
    int attr_count;
  } subSearchResult;


  typedef struct
  {
    int hasMore;
    subSearchResult **data;
    int size;
    int allocated;
    int result;
  } SearchReturn;


  typedef struct SqliteDB
  {
    sqlite3 *db;
    int last_error;
  } SqliteDB;

#if _MSC_VER
#define LOG(format, ...)  \
  {fprintf (stdout, format , __VA_ARGS__);fflush(stdout);}

#define  LOG_RETURN(x, format, ...) \
  {fprintf(stdout, format, __VA_ARGS__);fflush(stdout);return x;}

#else
#define LOG(format, args...)  \
  {fprintf (stdout, format , ## args);fflush(stdout);}

#define  LOG_RETURN(x, format, args...) \
  {fprintf(stdout, format, ## args);fflush(stdout);return x;}
#endif

  SqliteDB *SqliteOpen(const char *fname);
  void SqliteClose(SqliteDB *);

  int activate(SqliteDB *obj);
  int deactivate(SqliteDB *obj);
  int checkDBVersion(SqliteDB *obj, int *major, int *minor);

  SearchReturn *sqliteRetrieve(SqliteDB *obj,  ConfigList *cfg, const char *fullPath, sqlite3_int64 id, InternalCondition *condition);
  int sqliteCreate(SqliteDB *obj, ConfigList *cfg, const char *fullPath, const unsigned char *content, int len, InternalAttribute **attrs, int attrs_len, sqlite3_int64 *doc_id);
  int sqliteUpdate(SqliteDB *obj, ConfigList *cfg, const char *fullPath, sqlite3_int64 doc_id, const unsigned char *content, int len, InternalAttribute **attrs, int attrs_len, InternalCondition *condition);
  int sqliteDeleteCascade(SqliteDB *obj, ConfigList *cfg, const char *fullPath, sqlite3_int64 doc_id, int cascadeLevel, InternalCondition *condition);
  int sqliteDeleteExact(SqliteDB *obj, ConfigList *cfg, const char *fullPath, sqlite3_int64 doc_id, InternalCondition *condition);
  SearchReturn *search(SqliteDB *obj, ConfigList *cfg, const char *basePath, int scope, InternalCondition *condition, int order, int limit, int withContent, char **attributes, int attr_len);
  int sqlitePartialUpdate(SqliteDB *obj, ConfigList *cfg, char *fullPath, sqlite3_int64 doc_id, const unsigned char *content, int content_len, InternalAttrOperation **attrOps, int oper_len, InternalCondition *condition);
  void freeSearchReturn(SearchReturn *data);


  int beginTransaction(SqliteDB *obj);
  int commitTransaction(SqliteDB *obj);
  int rollbackTransaction(SqliteDB *obj);

  int executeSqlStatementWithOutParameters(SqliteDB *obj, char *sql);

  //set on every transaction
  //check whether the sum of FreeListCount and PageCount > MaxPageCount ? stop Trasaction : nothing;
  int getFreeListCount(SqliteDB *obj, int *count);
  //set on every transaction
  int getPageCount(SqliteDB *obj, int *pages);
  //set on every connection
  int setMaxPageCount(SqliteDB *obj, int maxPageCount);


#ifdef __cplusplus
}
#endif

#endif
