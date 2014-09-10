
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <curl/curl.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "common.h"
#include "header_t.h"
#include "response_t.h"

static void response_t_init(response_t *This);

/**
 * Initialize the data structure of the object-like.
 * @param This the object-like instance.
 */
static void response_t_init(response_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "response_t::init (This:0x%.8x)\n", This);

  /* Assign the pointers on functions */
  This->getHeader = response_t_getHeader;
  This->getNthHeader = response_t_getNthHeader;
  This->parseHeader = response_t_parseHeader;
  This->removeHeader = response_t_removeHeader;
  This->parseStatusLine = response_t_parseStatusLine;
  This->addToContent = response_t_addToContent;
  This->getMappedStatusCode = response_t_getMappedStatusCode;

  /* Initialize the members */
  This->content = NULL;
  This->contentLength = 0;
  This->protocol = NULL;
  This->reasonPhrase = NULL;
  This->statusCode = 0;  
  INIT_LIST_HEAD(&(This->headers));
}

/**
 * Dynamic allocation of the object-like instance
 * @param name the name of the header.
 * @return the created object-like instance.
 */
response_t *new_response_t()
{
  response_t *This = malloc(sizeof(response_t));
  if (!This)
  {
    return NULL;
  }
  response_t_init(This);
  This->free = response_t_newFree;
  RTL_TRDBG(TRACE_DEBUG, "new response_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * Free resources of the object-like instance.
 * @param This the object-like instance.
 */
void response_t_newFree(response_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "response_t::newFree (This:0x%.8x)\n", This);
  while ( ! list_empty(&(This->headers)))
  {
    header_t * h = list_entry(This->headers.next, header_t, chainLink);
    list_del(&h->chainLink);
    h->free(h);
  }
  if (This->content)
  {
    free(This->content);
  }
  if (This->protocol)
  {
    free(This->protocol);
  }
  if (This->reasonPhrase)
  {
    free(This->reasonPhrase);
  }
  free(This);
}

/**
 * Get the header matching the provided header name.
 * @param This the object-like instance.
 * @param name the header name.
 * @return the matching header_t object, NULL if not found.
 */
header_t *response_t_getHeader(response_t *This, char *name)
{
  header_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->headers))
  {
    header_t *h = list_entry(link, header_t, chainLink);
    if (!strcasecmp(name, h->name))
    {
      res = h;
      break;
    }
  }
  return res;
}



/**
 * Get the N-th header in the list.
 * @param This the object-like instance.
 * @param index the index in the list of the header to look for.
 * @return the matching header_t object, NULL if not found.
 */
header_t *response_t_getNthHeader(response_t *This, int index)
{
  header_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->headers))
  {
    header_t *h = list_entry(link, header_t, chainLink);
    if (0 == index)
    {
      res = h;
      break;
    }
    index --;
  }
  return res;
}

/**
 * Add a new header in the list. If the header is already managed in the list, it appends
 * the provided value to the already known values.
 * @param This the object-like instance.
 * @param headerLine the header line, as received by cURL.
 * @return 1 if the header has been added, 0 otherwise.
 */
int response_t_parseHeader(response_t *This, char *headerLine)
{
  header_t *h = NULL;
  char *ptr = headerLine;
  char szTemp[1024];
  int len;

  RTL_TRDBG(TRACE_DEBUG, "response_t::parseHeader (This:0x%.8x) (headerLine:%s)\n", 
    This, headerLine);

  /* look for the header name */
  if (NULL == (headerLine = strchr(ptr, ':')))
  {
    RTL_TRDBG(TRACE_ERROR, "response_t::addHeader: error - unsupported header line format"
      " (This:0x%.8x) (headerLine:%s)\n", This, headerLine);
    return 0;
  }
  len = headerLine - ptr;
  if (len >= sizeof(szTemp))
  {
    RTL_TRDBG(TRACE_ERROR, "response_t::addHeader: error - header name is too long"
      " (This:0x%.8x) (headerLine:%s)\n", This, headerLine);
    return 0;
  }
  memcpy(szTemp, ptr, len);
  szTemp[len] = 0; 
  
  headerLine++;
  if (headerLine[0] == ' ')
  {
    headerLine++;
  }

  h = This->getHeader(This, szTemp);
  if (NULL == h)
  {
    h = new_header_t(szTemp);
    list_add_tail(&h->chainLink, &(This->headers));
  }
  
  /* look for the header value */
  if (NULL == (ptr = strchr(headerLine, '\r')))
  {
    RTL_TRDBG(TRACE_ERROR, "response_t::addHeader: error - unable to get the header value"
      " (This:0x%.8x) (headerLine:%s)\n", This, headerLine);
    return 0;
  }
  len = ptr - headerLine;
  if (len >= sizeof(szTemp))
  {
    RTL_TRDBG(TRACE_ERROR, "response_t::addHeader: error - header value is too long"
      " (This:0x%.8x) (headerLine:%s)\n", This, headerLine);
    return 0;
  }
  memcpy(szTemp, headerLine, len);
  szTemp[len] = 0;

  h->addValue(h, szTemp);
  return 1;
}

/**
 * Remove a header (all its known values) from the response.
 * @param This the object-like instance.
 * @param name the name of the header.
 * @return 1 if the header has been removed successfully, 0 otherwise.
 */
int response_t_removeHeader(response_t *This, char *name)
{
  header_t *toRemove = This->getHeader(This, name);
  if (toRemove)
  {
    list_del(&(toRemove->chainLink));
    toRemove->free(toRemove);
    return 1;
  }
  return 0;
}

/**
 * Parse the received status line.
 * @param This the object-like instance.
 * @param statusLine the received status line.
 * @param statusLineLen the length of the response line. (unused here)
 * @return 1 if the response line can be parsed normally, 0 otherwise.
 */
int response_t_parseStatusLine(response_t *This, char *statusLine, size_t statusLineLen)
{
  char *pPtr = NULL;
  char szTemp[15];
  int len;
  (void)statusLineLen;
  
  RTL_TRDBG(TRACE_DEBUG, "response_t::parseStatusLine (This:0x%.8x) (statusLine:%s) (len:%d)\n", 
    This, statusLine, statusLineLen);

  /* look for the protocol */
  if (NULL == (pPtr = strchr(statusLine, ' ')))
  {
    RTL_TRDBG(TRACE_ERROR, "response_t::parseStatusLine: error - unable to detect the protocol "
      "(This:0x%.8x) (statusLine:%s)\n", This, statusLine);    
    return 0;
  }
  len = pPtr - statusLine;
  This->protocol = malloc(len + 1);
  memcpy(This->protocol, statusLine, len);
  This->protocol[len] = 0;
 
  pPtr++;
  statusLine = pPtr;
 
  /* look for the status code */ 
  if (NULL == (pPtr = strchr(statusLine, ' ')))
  {
    RTL_TRDBG(TRACE_ERROR, "response_t::parseStatusLine: error - unable to detect the status "
      "code (This:0x%.8x) (statusLine:%s)\n", This, statusLine);    
    return 0;
  }
 
  len = pPtr - statusLine;
  memcpy(szTemp, statusLine, len);
  szTemp[len] = 0;
  This->statusCode = atoi(szTemp);
 
  pPtr++;
  statusLine = pPtr;
   
  /* look for the reason phrase */
  if (NULL == (pPtr = strchr(statusLine, '\r')))
  {
    RTL_TRDBG(TRACE_ERROR, "response_t::parseStatusLine: error - unable to detect the reason "
      " phrase (This:0x%.8x) (statusLine:%s)\n", This, statusLine);    
    return 0;
  }
  len = pPtr - statusLine;
  This->reasonPhrase = malloc(len + 1);
  memcpy(This->reasonPhrase, statusLine, len);
  This->reasonPhrase[len] = 0;
 
  return 1;
}

/**
 * Add to the response content.
 * If content length is 0 or if the content is NULL, the former content is erased.
 * @param This the object-like instance.
 * @param content the new content to set.
 * @param contentLen the content length.
 */
void response_t_addToContent(response_t *This, unsigned char *content, size_t contentLen)
{
  size_t newContentLen = This->contentLength + contentLen + 1; 

  RTL_TRDBG(TRACE_DEBUG, "response_t::addToContent (This:0x%.8x) (contentLen:%d)\n", 
    This, contentLen);

  if (content && contentLen)
  {
    This->content = realloc(This->content, newContentLen);
    memcpy(This->content + This->contentLength, content, contentLen);
    This->contentLength = newContentLen;
    // content is not necessarily a \0-terminated string, but this can be usefull:
    This->content[newContentLen-1] = 0; 
  }
}

/**
 * Do translate the cURL result code into HTTP status code.
 * @param This the object-like instance.
 * @return the HTTP status code mapped on the provided cURL result code.
 */
int response_t_getMappedStatusCode(response_t *This)
{
  int statusCode = 400;

  if (!This) return statusCode;

  switch (This->statusCode)
  {
    case CURLE_OK:
      // All fine. Proceed as usual.
      statusCode = 200;
      break;

    case CURLE_UNSUPPORTED_PROTOCOL:
      // The URL you passed to libcurl used a protocol that this libcurl does not support.
      // The support might be a compile-time option that you didn't use, it can be a
      // misspelled protocol string or just a protocol libcurl has no code for.
      statusCode = 505;
      break;

    case CURLE_URL_MALFORMAT:
      // The URL was not properly formatted.
      statusCode = 400;
      break;

    case CURLE_NOT_BUILT_IN:
      // A requested feature, protocol or option was not found built-in in this libcurl due
      // to a build-time decision. This means that a feature or option was not enabled or
      // explicitly disabled when libcurl was built and in order to get it to function you
      // have to get a rebuilt libcurl.
      statusCode = 400;
      break;

    case CURLE_COULDNT_RESOLVE_PROXY:
      // Couldn't resolve proxy. The given proxy host could not be resolved.
      statusCode = 404;
      break;

    case CURLE_COULDNT_RESOLVE_HOST:
      // Couldn't resolve host. The given remote host was not resolved.
      statusCode = 404;
      break;

    case CURLE_COULDNT_CONNECT:
      // Failed to connect() to host or proxy.
      statusCode = 503;
      break;

    case CURLE_REMOTE_ACCESS_DENIED:
      // We were denied access to the resource given in the URL. For FTP, this occurs while
      // trying to change to the remote directory.
      statusCode = 403;
      break;

    case CURLE_PARTIAL_FILE:
      // A file transfer was shorter or larger than expected. This happens when the server 
      // first reports an expected transfer size, and then delivers data that doesn't match 
      // the previously given size.
      //statusCode = ;
      break;

    case CURLE_QUOTE_ERROR:
      // When sending custom "QUOTE" commands to the remote server, one of the commands 
      // returned an error code that was 400 or higher (for FTP) or otherwise indicated 
      // unsuccessful completion of the command.
      //statusCode = ;
      break;

    case CURLE_HTTP_RETURNED_ERROR:
      // This is returned if CURLOPT_FAILONERROR is set TRUE and the HTTP server returns 
      // an error code that is >= 400.
      //statusCode = ;
      break;

    case CURLE_WRITE_ERROR:
      // An error occurred when writing received data to a local file, or an error was 
      // returned to libcurl from a write callback.
      //statusCode = ;
      break;

    case CURLE_UPLOAD_FAILED:
      // Failed starting the upload. For FTP, the server typically denied the STOR 
      // command. The error buffer usually contains the server's explanation for this.
      //statusCode = 5XX;
      break;

    case CURLE_READ_ERROR:
      // There was a problem reading a local file or an error returned by the read callback.
      //statusCode = 500;
      break;

    case CURLE_OUT_OF_MEMORY:
      // A memory allocation request failed. This is serious badness and things are 
      // severely screwed up if this ever occurs.
      statusCode = 456;
      break;

    case CURLE_OPERATION_TIMEDOUT:
      // Operation timeout. The specified time-out period was reached according to the 
      // conditions.
      statusCode = 504;
      break;

    case CURLE_RANGE_ERROR:
      // The server does not support or accept range requests.
      //statusCode = ;
      break;

    case CURLE_HTTP_POST_ERROR:
      // This is an odd error that mainly occurs due to internal confusion.
      //statusCode = ;
      break;

    case CURLE_SSL_CONNECT_ERROR:
      // A problem occurred somewhere in the SSL/TLS handshake. You really want the error
      // buffer and read the message there as it pinpoints the problem slightly more. 
      // Could be certificates (file formats, paths, permissions), passwords, and others.
      //statusCode = ;
      break;

    case CURLE_BAD_DOWNLOAD_RESUME:
      // The download could not be resumed because the specified offset was out of the 
      // file boundary.
      //statusCode = ;
      break;

    case CURLE_FILE_COULDNT_READ_FILE:
      // A file given with FILE:// couldn't be opened. Most likely because the file path 
      // doesn't identify an existing file. Did you check file permissions?
      //statusCode = ;
      break;

    case CURLE_FUNCTION_NOT_FOUND:
      // Function not found. A required zlib function was not found.
      //statusCode = ;
      break;

    case CURLE_ABORTED_BY_CALLBACK:
      // Aborted by callback. A callback returned "abort" to libcurl.
      //statusCode = ;
      break;

    case CURLE_BAD_FUNCTION_ARGUMENT:
      // Internal error. A function was called with a bad parameter.
      //statusCode = ;
      break;

    case CURLE_INTERFACE_FAILED:
      // Interface error. A specified outgoing interface could not be used. Set which 
      // interface to use for outgoing connections' source IP address with CURLOPT_INTERFACE.
      //statusCode = ;
      break;

    case CURLE_TOO_MANY_REDIRECTS:
      // Too many redirects. When following redirects, libcurl hit the maximum amount. 
      // Set your limit with CURLOPT_MAXREDIRS.
      //statusCode = ;
      break;

    case CURLE_UNKNOWN_OPTION:
      // An option passed to libcurl is not recognized/known. Refer to the appropriate 
      // documentation. This is most likely a problem in the program that uses libcurl. 
      // The error buffer might contain more specific information about which exact 
      // option it concerns.
      //statusCode = ;
      break;

    case CURLE_PEER_FAILED_VERIFICATION:
      // The remote server's SSL certificate or SSH md5 fingerprint was deemed not OK.
      //statusCode = ;
      break;

    case CURLE_GOT_NOTHING:
      // Nothing was returned from the server, and under the circumstances, getting 
      // nothing is considered an error.
      statusCode = 410;
      break;

    case CURLE_SSL_ENGINE_NOTFOUND:
      // The specified crypto engine wasn't found.
      //statusCode = ;
      break;

    case CURLE_SSL_ENGINE_SETFAILED:
      // Failed setting the selected SSL crypto engine as default!
      //statusCode = ;
      break;

    case CURLE_SEND_ERROR:
      // Failed sending network data.
      //statusCode = ;
      break;

    case CURLE_RECV_ERROR:
      // Failure with receiving network data.
      //statusCode = ;
      break;

    case CURLE_SSL_CERTPROBLEM:
      // problem with the local client certificate.
      //statusCode = ;
      break;

    case CURLE_SSL_CIPHER:
      // Couldn't use specified cipher.
      //statusCode = ;
      break;

    case CURLE_SSL_CACERT:
      // Peer certificate cannot be authenticated with known CA certificates.
      //statusCode = ;
      break;

    case CURLE_BAD_CONTENT_ENCODING:
      // Unrecognized transfer encoding.
      statusCode = 415;
      break;

    case CURLE_FILESIZE_EXCEEDED:
      // Maximum file size exceeded.
      //statusCode = ;
      break;

    case CURLE_SEND_FAIL_REWIND:
      // When doing a send operation curl had to rewind the data to retransmit, 
      // but the rewinding operation failed.
      //statusCode = ;
      break;

    case CURLE_SSL_ENGINE_INITFAILED:
      // Initiating the SSL Engine failed.
      //statusCode = ;
      break;

    case CURLE_LOGIN_DENIED:
      // The remote server denied curl to login (Added in 7.13.1)
      statusCode = 401;
      break;

    case CURLE_REMOTE_DISK_FULL:
      // Out of disk space on the server.
      statusCode = 500;
      break;

    case CURLE_REMOTE_FILE_EXISTS:
      // File already exists and will not be overwritten.
      statusCode = 201;
      break;

    case CURLE_CONV_FAILED:
      // Character conversion failed.
      //statusCode = ;
      break;

    case CURLE_CONV_REQD:
      // Caller must register conversion callbacks.
      //statusCode = ;
      break;

    case CURLE_SSL_CACERT_BADFILE:
      // Problem with reading the SSL CA cert (path? access rights?)
      //statusCode = ;
      break;

    case CURLE_REMOTE_FILE_NOT_FOUND:
      // The resource referenced in the URL does not exist.
      statusCode = 404;
      break;

    case CURLE_SSH:
      // An unspecified error occurred during the SSH session.
      //statusCode = ;
      break;

    case CURLE_SSL_SHUTDOWN_FAILED:
      // Failed to shut down the SSL connection.
      //statusCode = ;
      break;

    case CURLE_SSL_CRL_BADFILE:
      // Failed to load CRL file (Added in 7.19.0)
      //statusCode = ;
      break;

    case CURLE_SSL_ISSUER_ERROR:
      // Issuer check failed (Added in 7.19.0)
      //statusCode = ;
      break;

    case CURLE_CHUNK_FAILED:
      // Chunk callback reported error.
      //statusCode = ;
      break;

    default:
      statusCode = This->statusCode;
      break;
  }

  return statusCode;
}


