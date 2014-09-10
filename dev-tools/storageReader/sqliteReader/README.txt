
This is an ONG tool. It retrieves a Document content from a SQLite database 
file (typically storage.db), and can:
(1) either decode and display the content on standard output,
(2) either save the raw Xo content into a file.

usage: sqliteReader.x [-h] [-v] [-o] [-f <file>] <database> <path>
where <database> is the absolute filename of the SQLite database
where <path> is path of the document to retrieve
and where options are:
 -h : display this help message
 -o : specifies that the document content is oBIX format
 -v : display the version string of the loaded libraries


(1) How to use it for decoding and displaying the content on standard output:

$ sqliteReader.x storage.db /applications
<?xml version="1.0" encoding="UTF-8"?>
<m2m:applications
xmlns:m2m="http://uri.etsi.org/m2m"><m2m:accessRightID>/accessRights/Locadmin_AR</m2m:accessRightID><m2m:creationTime>2013-06-17T02:21:39.113-07:00</m2m:creationTime><m2m:lastModifiedTime>2013-06-17T02:21:39.498-07:00</m2m:lastModifiedTime></m2m:applications>


(2) How to use it for saving the raw Xo content into a file

one can also use tools that come with Xo, and especially the one so called 
"xotoascii.x":

- First, retrieve the Document content:
 $ sqliteReader.x -o xo-content.bin storage.db /applications

- Then decode the Xo binary content:
 $ cd $ROOTACT/etc/xo/
 $ ../../xo/com/xotoasci.x -xml -shdico -d ../../xo-content.bin
<?xml version="1.0" encoding="UTF-8"?>
<m2m:applications xmlns:m2m="http://uri.etsi.org/m2m">
 <m2m:accessRightID>/accessRights/Locadmin_AR</m2m:accessRightID>
 <m2m:creationTime>2013-06-17T02:21:39.113-07:00</m2m:creationTime>
 <m2m:lastModifiedTime>2013-06-17T02:21:39.498-07:00</m2m:lastModifiedTime>
</m2m:applications>

