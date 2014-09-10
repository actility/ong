%module sqlitedriver

%{
    #include <sqlite3.h>
    #include <sqlite_driver.h>
%}

%include various.i
%include typemaps.i

# Force unsigned char* content to be of type char *BYTE (this is a byte buffer)
%apply char *BYTE {unsigned char * content}
# Force int* count to int* type (instead of using a wrapper)
%apply int *OUTPUT {int *count};
# Force int* pages to int* type (instead of using a wrapper)
%apply int *OUTPUT {int *pages};
# Force int* int_values to int* type (instead of using a wrapper)
%apply int *INPUT {int *int_value};
# Force int* attr_type to int* type (instead of using a wrapper)
%apply int *INPUT {int *attr_type};


# This allows to convert signed char* from sqliteRetrieve to byte array in java
%typemap(jni) signed char *sqliteRetrieve "jbyteArray"
%typemap(jtype) signed char *sqliteRetrieve "byte[]"
%typemap(jstype) signed char *sqliteRetrieve "byte[]"
%typemap(javaout) signed char *sqliteRetrieve {
  return $jnicall;
}

# We told swig to treat signed char* as a byte array but we need this code to explicitly convert it
%typemap(out) signed char *sqliteRetrieve {
  $result = JCALL1(NewByteArray, jenv, length);
  JCALL4(SetByteArrayRegion, jenv, $result, 0, length, $1);
}

# We told to convert signed char* to byte array but we need to free it after that otherwise, we will have a leak
%typemap(newfree) signed char * "free($1);";
%newobject sqliteRetrieve;

# Remove int* len parameter from sqliteRetrieve in java
# This length parameter is only necessary in native code to know the length of the returned value
%typemap(in,numinputs=0,noblock=1) int *len { 
  int length=0;
  $1 = &length;
}

# This allows to convert signed char* content to byte array in java
%typemap(jni) signed char *content "jbyteArray"
%typemap(jtype) signed char *content "byte[]"
%typemap(jstype) signed char *content "byte[]"
%typemap(javaout) signed char *content {
    return $jnicall;
}

# We told swig to treat signed char* content as a byte array but we need this code to explicitly convert it
%typemap(out) signed char * content {
    $result = JCALL1(NewByteArray, jenv, arg1->contentLength);
    JCALL4(SetByteArrayRegion, jenv, $result, 0, arg1->contentLength, $1);
}
%typemap(in) signed char *content {
    $1 = (signed char *)JCALL2(GetByteArrayElements, jenv, $input, 0);
}
%typemap(javain) signed char *content "$javainput"

# Tells swig that search function returns a new SearchReturn object managed by java
# Otherwise it will not be deleted when java object is finalized
%newobject search;

# Adds a destructor to SearchReturn because this is not automatic with C
%extend SearchReturn {
    ~SearchReturn() {
       freeSearchReturn($self);
    }
}

# Adds a destructor to InternalCondition because this is not automatic with C
%extend InternalCondition {
    ~InternalCondition() {
        freeInternalCondition($self);
    }
}

# Adds a destructor to InternalCondition because this is not automatic with C
%extend InternalAttribute {
    ~InternalAttribute() {
        freeInternalAttribute($self);
    }
}

%typemap(javacode) InternalCondition %{
  // Force swigCMemOwn value
  public void forceSwigCMemOwn(boolean swigCMemOwn) {
    this.swigCMemOwn = swigCMemOwn;
  }
%}

%typemap(javacode) InternalAttribute %{
  // Force swigCMemOwn value
  public void forceSwigCMemOwn(boolean swigCMemOwn) {
    this.swigCMemOwn = swigCMemOwn;
  }
%}

# Adds automatic loading of JNI library in generated code
%pragma(java) jniclasscode=%{ 
  static { 
    try { 
        System.loadLibrary("SqliteJni"); 
    } catch (UnsatisfiedLinkError e) { 
      System.err.println("Native code library failed to load. \n" + e); 
    } 
  } 
%}

# Ignore various things from sqlite_driver.h
%ignore freeSearchReturn;
%ignore freeInternalCondition;
%ignore freeInternalAttribute;
%ignore db;
%ignore last_error;
%ignore contentLength;

%include "sqlite_driver.h"

%include <carrays.i>

# Generate a subSearchResult array
%array_functions(subSearchResult *, searchArray);
# Generate an InternalAttribute array
%array_functions(InternalAttribute *, attributeArray);

# Ignore some array functions
%ignore searchArray_setitem;
%ignore delete_searchArray;
%ignore new_searchArray;
%ignore attributeArray_getitem;
%ignore delete_attributeArray;