
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dIa.h>

int
XXX_split_uri(unsigned char *str_var, size_t len, coap_uri_t *uri) {
  unsigned char *p, *q;
  int res = 0;

  if (!str_var || !uri)
    return -1;

  memset(uri, 0, sizeof(coap_uri_t));
  uri->port = COAP_DEFAULT_PORT;

  /* search for scheme */
  p = str_var;
  if (*p == '/') {
    q = p;
    goto path;
  }
#if 0
  q = (unsigned char *)COAP_DEFAULT_SCHEME;
  while (len && *q && tolower(*p) == *q) {
    ++p; ++q; --len;
  }
  
  /* If q does not point to the string end marker '\0', the schema
   * identifier is wrong. */
  if (*q) {
    res = -1;
    goto error;
  }

  /* There might be an additional 's', indicating the secure version: */
  if (len && (secure = tolower(*p) == 's')) {
    ++p; --len;
  }
  q = (unsigned char *)"://";
  while (len && *q && tolower(*p) == *q) {
    ++p; ++q; --len;
  }

  if (*q) {
    res = -2;
    goto error;
  }
#endif
  q = strstr((char *)p, "://");
  if (!q) {
    res = -2;
    goto error;
  }
  len -= q+3-p;
  p = q+3;

  /* p points to beginning of Uri-Host */
  q = p;
  if (len && *p == '[') {	/* IPv6 address reference */
    ++p;
    
    while (len && *q != ']') {
      ++q; --len;
    }

    if (!len || *q != ']' || p == q) {
      res = -3;
      goto error;
    } 

//    COAP_SET_STR(&uri->host, q - p, p);
    // Version with brackets !!!
    COAP_SET_STR(&uri->host, q - p + 2, p - 1);
    ++q; --len;
  } else {			/* IPv4 address or FQDN */
    while (len && *q != ':' && *q != '/' && *q != '?') {
      *q = tolower(*q);
      ++q;
      --len;
    }

    if (p == q) {
      res = -3;
      goto error;
    }

    COAP_SET_STR(&uri->host, q - p, p);
  }

  /* check for Uri-Port */
  if (len && *q == ':') {
    p = ++q;
    --len;
    
    while (len && isdigit(*q)) {
      ++q;
      --len;
    }

    if (p < q) {		/* explicit port number given */
      int uri_port = 0;
    
      while (p < q)
	uri_port = uri_port * 10 + (*p++ - '0');

      uri->port = uri_port;
    } 
  }
  
 path:		 /* at this point, p must point to an absolute path */

  if (!len)
    goto end;
  
  if (*q == '/') {
    p = ++q;
    --len;
    while (len && *q != '?') {
      ++q;
      --len;
    }
  
    if (p < q) {
      COAP_SET_STR(&uri->path, q - p, p);
      p = q;
    }
  }

  /* Uri_Query */
  /* FIXME: split at & sign, parse:
        query-pattern = search-token [ "*" ]
        search-token = *search-char
        search-char = unreserved / pct-encoded
                    / ":" / "@"   ; from pchar
                    / "/" / "?"   ; from query
                    / "!" / "$" / "'" / "(" / ")"
                    / "+" / "," / ";" / "="  ; from sub-delims
   */
  if (len && *p == '?') {
    ++p;
    --len;
    COAP_SET_STR(&uri->query, len, p);
    len = 0;
  }

  end:
  return len ? -1 : 0;
  
  error:
  return res;
}


int main() {
	coap_uri_t uri;
	char targetID[500];

	strcpy (targetID, "coap://www.actility.com:5388/hello");
	XXX_split_uri((unsigned char *)targetID, strlen(targetID), &uri );
	printf ("%s\n", targetID);

	exit (0);
}
