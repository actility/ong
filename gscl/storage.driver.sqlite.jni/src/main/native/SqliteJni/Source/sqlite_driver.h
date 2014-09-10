#ifndef sqllite_driver_h_
#define sqllite_driver_h_

#ifdef __cplusplus
extern "C" {
#endif


#define PATH_DELIMITER '/'
#define STAR_CHAR '*'

#define CASCADING_NONE 7
#define CASCADING_ONE_LEVEL 6
#define CASCADING_SUBTREE   12

#define TYPE_CREATE 8
#define TYPE_UPDATE 9
#define TYPE_DELETE 10

#define SCOPE_EXACT 1
#define SCOPE_SUBTREE 2
#define ORDER_ASC 3
#define ORDER_DESC 4
#define ORDER_UNKNOWN 5

// operatios for condition
#define OPERATOR_EQUAL 1
#define OPERATOR_GREATER 2
#define OPERATOR_LOWER 3
#define OPERATOR_STARTS_WITH 4
#define OPERATOR_AND 5
#define OPERATOR_NOT_EQUAL_TO 9
#define SQLITE_OK 0


// types of operand in condition
#define TYPE_DATE 8
#define TYPE_INTEGER 7
#define TYPE_STRING 6

  typedef struct InternalAttribute
  {
    char *name;
    char *string_value;
    int int_value;
    char *date_value;
    int type;
  } InternalAttribute;

  typedef struct InternalCondition
  {
    char *attributeName;
    int type;
    int oper; // operator
    char *operand;
    struct InternalCondition *condition1;
    struct InternalCondition *condition2;


  } InternalCondition;

  typedef struct
  {

    char *path;
    char *name;
    signed char *content;
    int contentLength;
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

//typedef struct InternalCondition InternalCondition;
//typedef struct SearchReturn SearchReturn;
//typedef struct subSearchResult subSearchResult;


  SqliteDB *SqliteOpen(const char *fname);
  void SqliteClose(SqliteDB *);

  int activate(SqliteDB *obj);
  int deactivate(SqliteDB *obj);
  int checkDBVersion(SqliteDB *obj, int major, int minor);
  void setLogLevel(int level, SqliteDB *obj);

  signed char *sqliteRetrieve(SqliteDB *obj, const char *fullPath, int *len);
  int sqliteCreate(SqliteDB *obj, const char *fullPath, const unsigned char *content, int len, InternalAttribute **attrs, int attrs_len);
  int sqliteUpdate(SqliteDB *obj, const char *fullPath, const unsigned char *content, int len, InternalAttribute **attrs, int attrs_len);
  int sqliteDelete(SqliteDB *obj, const char *fullPath, int cascadeLevel);

  void freeSearchReturn(SearchReturn *data);
  void freeInternalCondition(InternalCondition *data);
  void freeInternalAttribute(InternalAttribute *data);
  void freeInternalAttributeArray(InternalAttribute **data, int length);
  SearchReturn *search(SqliteDB *obj, const char *basePath, int scope, InternalCondition *condition, int order, int limit);

  int beginTransaction(SqliteDB *obj);
  int commitTransaction(SqliteDB *obj);
  int rollbackTransaction(SqliteDB *obj);

  int executeSqlStatementWithOutParameters(SqliteDB *obj, char *sql);

//set on every transaction
//check whether the sum of FreeListCount and PageCount > MaxPageCount ? stop Trasaction : nothing;
//  int getFreeListCount(SqliteDB *obj, int *count);
////set on every transaction
//  int getPageCount(SqliteDB *obj, int *pages);
//set on every connection
  int setMaxDatabaseSizeKb(SqliteDB *obj, int maxDatabaseSizeKb);

  void setMinDurationBetweenVacuums(long value);
  void setMaxOpsBeforeVacuum(int value);
  void setMaxDurationBeforeVacuum(long value);
  void setMaxDBUsagePourcentageBeforeVacuum(int value);
  void setMinDurationBetweenDBUsageComputation(long value);

#ifdef __cplusplus
}
#endif

#endif
