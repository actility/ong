%module sqlitedriver

%{
  #include <sqlite3.h>
  #include <sqlite_driver.h>
%}

%include various.i
%include typemaps.i


/* Tell SWIG about sqlite3_int64 */
typedef long long sqlite3_int64;

%apply long long *OUTPUT {long long *doc_id};
%apply long long *OUTPUT {long long *id};

%apply int *OUTPUT {int *attrLen};

%apply char *BYTE {unsigned char * content}
%apply int *OUTPUT {int *count};
%apply int *OUTPUT {int *pages};
%apply int *INPUT {int *int_value};
%apply int *INPUT {int *attr_type};
%apply int *OUTPUT {int *major};
%apply int *OUTPUT {int *minor};
%apply char **STRING_ARRAY {  char **attributes}

%newobject sqliteRetrieve;

%typemap(in,numinputs=0,noblock=1) int *len { 
  int length=0;
  $1 = &length;
}


%typemap(jni) signed char *content "jbyteArray"
%typemap(jtype) signed char *content "byte[]"
%typemap(jstype) signed char *content "byte[]"
%typemap(javaout) signed char *content {
  return $jnicall;
}


%typemap(out) signed char * content {
  $result = JCALL1(NewByteArray, jenv, arg1->contentLength);
  JCALL4(SetByteArrayRegion, jenv, $result, 0, arg1->contentLength, $1);
}

%typemap(in) signed char *content {
  $1 = (signed char *)JCALL2(GetByteArrayElements, jenv, $input, 0);
}
%typemap(javain) signed char *content "$javainput"

%newobject search;

%extend SearchReturn {
  ~SearchReturn() {
    freeSearchReturn(self);
  }
}

%{ 

void freeAttribute(InternalAttribute * attribute)
{
  if (attribute == NULL) return;
  //fprintf(stdout,"Free attribute %d\n",attribute);
  if (attribute->name) free(attribute->name);
  if (attribute->string_value) free(attribute->string_value);
  if (attribute->date_value) free(attribute->date_value);
  free(attribute);
}

void freeInternalAttrOperation(InternalAttrOperation * oper)
{
  if (oper == NULL) return;
  //fprintf(stdout,"Free operation %d\n",oper);
  free(oper);
}

 
void freeInternalCondition(InternalCondition * data)
{
  if (data == NULL) return;
  if (data->attributeName) free(data->attributeName);
  if (data->operand) free(data->operand);
  free(data);
}
void freeConfigItem(ConfigItem * item)
{
  if (item == NULL) return;
  if (item->key) free(item->key);
  if (item->value) free(item->value);
  free(item);
}
%}


%pragma(java) jniclasscode=%{ 
    static { 
        try { 
            System.loadLibrary("sqlitedb"); 
        } catch (UnsatisfiedLinkError e) { 
            System.err.println("Native code library failed to load. \n" + e); 
        }
    } 
%}

%extend InternalCondition {
  ~InternalCondition() {
    freeInternalCondition(self);
  }
}

%extend InternalAttribute {
  ~InternalAttribute() {
    freeAttribute(self);
  }
}

%extend ConfigItem {
  ~ConfigItem() {
    freeConfigItem(self);
  }
}


%extend InternalAttrOperation {
  ~InternalAttrOperation() {
    freeInternalAttrOperation(self);
  }
}


%ignore db;
%ignore last_error;
%ignore contentLength;

%include "sqlite_driver.h"

%include <carrays.i>

%ignore search_array_setitem;
%ignore delete_search_array;
%ignore new_search_array;

%array_functions(subSearchResult *, search_array);

%array_functions(InternalAttribute *, attribute_array);
%array_functions(InternalAttrOperation *, operation_array);
%array_functions(ConfigItem *, configItem_array);
%array_functions(InternalCondition *, condition_array);
