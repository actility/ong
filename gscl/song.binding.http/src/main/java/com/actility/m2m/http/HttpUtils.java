/*
 * Copyright   Actility, SA. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 *
 * id $Id: HttpUtils.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.http;

/**
 * Some useful HTTP constants
 */
public interface HttpUtils {

    /**
     * HTTP/1.0 string constant
     */
    String PT_HTTP_1_0 = "HTTP/1.0";

    /**
     * HTTP/1.1 string constant
     */
    String PT_HTTP_1_1 = "HTTP/1.1";

    /**
     * Reason phrase for 500 HTTP response
     */
    String RP_INTERNAL_SERVER_ERROR = "Internal Server Error";
    /**
     * Reason phrase for 400 HTTP response
     */
    String RP_BAD_REQUEST = "Bad Request";
    /**
     * Reason phrase for 403 HTTP response
     */
    String RP_FORBIDDEN = "Forbidden";
    /**
     * Reason phrase for 413 HTTP response
     */
    String RP_REQUEST_ENTITY_TOO_LARGE = "Request Entity Too Large";
    /**
     * Reason phrase for 411 HTTP response
     */
    String RP_LENGTH_REQUIRED = "Length Required";
    /**
     * Reason phrase for 501 HTTP response
     */
    String RP_NOT_IMPLEMENTED = "Not Implemented";

    /**
     * The GET method means retrieve whatever information (in the form of an entity) is identified by the Request-URI. If the
     * Request-URI refers to a data-producing process, it is the produced data which shall be returned as the entity in the
     * response and not the source text of the process, unless that text happens to be the output of the process.
     *
     * @since HTTP/1.0
     */
    int MD_GET_CODE = 0;

    /**
     * The POST method is used to request that the origin server accept the entity enclosed in the request as a new subordinate
     * of the resource identified by the Request-URI in the Request-Line. POST is designed to allow a uniform method to cover
     * the following functions:
     * <ul>
     * <li>Annotation of existing resources.</li>
     * <li>Posting a message to a bulletin board, newsgroup, mailing list, or similar group of articles.</li>
     * <li>Providing a block of data, such as the result of submitting a form, to a data-handling process.</li>
     * <li>Extending a database through an append operation.</li>
     * </ul>
     *
     * @since HTTP/1.0
     */
    int MD_POST_CODE = 1;

    /**
     * The PUT method requests that the enclosed entity be stored under the supplied Request-URI. If the Request-URI refers to
     * an already existing resource, the enclosed entity SHOULD be considered as a modified version of the one residing on the
     * origin server. If the Request-URI does not point to an existing resource, and that URI is capable of being defined as a
     * new resource by the requesting user agent, the origin server can create the resource with that URI. If a new resource is
     * created, the origin server MUST inform the user agent via the 201 (Created) response. If an existing resource is
     * modified, either the 200 (OK) or 204 (No Content) response codes SHOULD be sent to indicate successful completion of the
     * request. If the resource could not be created or modified with the Request-URI, an appropriate error response SHOULD be
     * given that reflects the nature of the problem. The recipient of the entity MUST NOT ignore any Content-* (e.g.
     * Content-Range) headers that it does not understand or implement and MUST return a 501 (Not Implemented) response in such
     * cases.
     *
     * @since HTTP/1.1
     */
    int MD_PUT_CODE = 2;

    /**
     * The DELETE method requests that the origin server delete the resource identified by the Request-URI. This method MAY be
     * overridden by human intervention (or other means) on the origin server. The client cannot be guaranteed that the
     * operation has been carried out, even if the status code returned from the origin server indicates that the action has
     * been completed successfully. However, the server SHOULD NOT indicate success unless, at the time the response is given,
     * it intends to delete the resource or move it to an inaccessible location.
     *
     * @since HTTP/1.1
     */
    int MD_DELETE_CODE = 3;

    /**
     * The GET method means retrieve whatever information (in the form of an entity) is identified by the Request-URI. If the
     * Request-URI refers to a data-producing process, it is the produced data which shall be returned as the entity in the
     * response and not the source text of the process, unless that text happens to be the output of the process.
     *
     * @since HTTP/1.0
     */
    String MD_GET = "GET";

    /**
     * The POST method is used to request that the origin server accept the entity enclosed in the request as a new subordinate
     * of the resource identified by the Request-URI in the Request-Line. POST is designed to allow a uniform method to cover
     * the following functions:
     * <ul>
     * <li>Annotation of existing resources.</li>
     * <li>Posting a message to a bulletin board, newsgroup, mailing list, or similar group of articles.</li>
     * <li>Providing a block of data, such as the result of submitting a form, to a data-handling process.</li>
     * <li>Extending a database through an append operation.</li>
     * </ul>
     *
     * @since HTTP/1.0
     */
    String MD_POST = "POST";

    /**
     * The PUT method requests that the enclosed entity be stored under the supplied Request-URI. If the Request-URI refers to
     * an already existing resource, the enclosed entity SHOULD be considered as a modified version of the one residing on the
     * origin server. If the Request-URI does not point to an existing resource, and that URI is capable of being defined as a
     * new resource by the requesting user agent, the origin server can create the resource with that URI. If a new resource is
     * created, the origin server MUST inform the user agent via the 201 (Created) response. If an existing resource is
     * modified, either the 200 (OK) or 204 (No Content) response codes SHOULD be sent to indicate successful completion of the
     * request. If the resource could not be created or modified with the Request-URI, an appropriate error response SHOULD be
     * given that reflects the nature of the problem. The recipient of the entity MUST NOT ignore any Content-* (e.g.
     * Content-Range) headers that it does not understand or implement and MUST return a 501 (Not Implemented) response in such
     * cases.
     *
     * @since HTTP/1.1
     */
    String MD_PUT = "PUT";

    /**
     * The DELETE method requests that the origin server delete the resource identified by the Request-URI. This method MAY be
     * overridden by human intervention (or other means) on the origin server. The client cannot be guaranteed that the
     * operation has been carried out, even if the status code returned from the origin server indicates that the action has
     * been completed successfully. However, the server SHOULD NOT indicate success unless, at the time the response is given,
     * it intends to delete the resource or move it to an inaccessible location.
     *
     * @since HTTP/1.1
     */
    String MD_DELETE = "DELETE";

    /* HTTP General Headers */

    /**
     * The Cache-Control general-header field is used to specify directives that MUST be obeyed by all caching mechanisms along
     * the request/response chain. The directives specify behavior intended to prevent caches from adversely interfering with
     * the request or response. These directives typically override the default caching algorithms. Cache directives are
     * unidirectional in that the presence of a directive in a request does not imply that the same directive is to be given in
     * the response.
     * <p>
     * <code>Cache-Control   = "Cache-Control" ":" 1#cache-directive</code>
     * <p>
     * Example:
     * <p>
     * <code>Cache-Control: no-cache</code>
     *
     * @since HTTP/1.1
     */
    String HD_CACHE_CONTROL = "Cache-Control";

    /**
     * The Connection general-header field allows the sender to specify options that are desired for that particular connection
     * and MUST NOT be communicated by proxies over further connections.
     * <p>
     * <code>Connection = "Connection" ":" 1#(connection-token) connection-token = token</code>
     * <p>
     * Example:
     * <p>
     * <code>Connection: close</code>
     *
     * @since HTTP/1.1
     */
    String HD_CONNECTION = "Connection";

    /**
     * The Proxy-Connection general-header field allows the sender to specify options that are desired for that particular
     * connection and MUST NOT be communicated by proxies over further connections. This is not an official header from HTTP RFC
     * <p>
     * <code>Connection = "Connection" ":" 1#(connection-token) connection-token = token</code>
     * <p>
     * Example:
     * <p>
     * <code>Connection: close</code>
     */
    String HD_PROXY_CONNECTION = "Proxy-Connection";

    /**
     * The Date general-header field represents the date and time at which the message was originated, having the same semantics
     * as orig-date in RFC 822. The field value is an HTTP-date; it MUST be sent in RFC 1123 [8]-date format.
     * <p>
     * <code>Date = "Date" ":" HTTP-date</code>
     * <p>
     * Example:
     * <p>
     * <code>Date: Tue, 15 Nov 1994 08:12:31 GMT</code>
     * <p>
     * Not forwarded in SONG messages.
     *
     * @since HTTP/1.0
     */
    String HD_DATE = "Date";

    /**
     * The Pragma general-header field is used to include implementation- specific directives that might apply to any recipient
     * along the request/response chain. All pragma directives specify optional behavior from the viewpoint of the protocol;
     * however, some systems MAY require that behavior be consistent with the directives.
     * <p>
     * <code>Pragma = "Pragma" ":" 1#pragma-directive pragma-directive = "no-cache" | extension-pragma<br>
     * extension-pragma = token [ "="
     * ( token | quoted-string ) ]</code>
     * <p>
     * Example:
     * <p>
     * <code>Pragma: no-cache</code>
     *
     * @since HTTP/1.0
     */
    String HD_PRAGMA = "Pragma";

    /**
     * The Trailer general field value indicates that the given set of header fields is present in the trailer of a message
     * encoded with chunked transfer-coding.
     * <p>
     * <code>Trailer = "Trailer" ":" 1#field-name</code>
     *
     * @since HTTP/1.1
     */
    String HD_TRAILER = "Trailer";

    /**
     * The Transfer-Encoding general-header field indicates what (if any) type of transformation has been applied to the message
     * body in order to safely transfer it between the sender and the recipient. This differs from the content-coding in that
     * the transfer-coding is a property of the message, not of the entity.
     * <p>
     * <code>Transfer-Encoding = "Transfer-Encoding" ":" 1#transfer-coding</code>
     * <p>
     * Example:
     * <p>
     * <code>Transfer-Encoding: chunked</code>
     *
     * @since HTTP/1.1
     */
    String HD_TRANSFER_ENCODING = "Transfer-Encoding";

    /**
     * The Upgrade general-header allows the client to specify what additional communication protocols it supports and would
     * like to use if the server finds it appropriate to switch protocols. The server MUST use the Upgrade header field within a
     * 101 (Switching Protocols) response to indicate which protocol(s) are being switched.
     * <p>
     * <code>Upgrade = "Upgrade" ":" 1#product</code>
     * <p>
     * Example:
     * <p>
     * <code>Upgrade: HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11</code>
     *
     * @since HTTP/1.1
     */
    String HD_UPGRADE = "Upgrade";

    /**
     * The Via general-header field MUST be used by gateways and proxies to indicate the intermediate protocols and recipients
     * between the user agent and the server on requests, and between the origin server and the client on responses. It is
     * analogous to the "Received" field of RFC 822 [9] and is intended to be used for tracking message forwards, avoiding
     * request loops, and identifying the protocol capabilities of all senders along the request/response chain.
     * <p>
     * <code>Via = "Via" ":" 1#( received-protocol received-by [ comment ] )<br>
     * received-protocol = [ protocol-name "/" ] protocol-version<br>
     * protocol-name = token<br>
     * protocol-version = token<br>
     * received-by = ( host [ ":" port ] ) | pseudonym<br>
     * pseudonym = token</code>
     * <p>
     * <code>Via: 1.0 fred, 1.1 nowhere.com (Apache/1.1)</code>
     *
     * @since HTTP/1.1
     */
    String HD_VIA = "Via";

    /**
     * The Warning general-header field is used to carry additional information about the status or transformation of a message
     * which might not be reflected in the message. This information is typically used to warn about a possible lack of semantic
     * transparency from caching operations or transformations applied to the entity body of the message.
     * <p>
     * Warning headers are sent with responses using:
     * <p>
     * <code>Warning = "Warning" ":" 1#warning-value<br>
     * warning-value = warn-code SP warn-agent SP warn-text [SP warn-date]<br>
     * warn-code = 3DIGIT<br>
     * warn-agent = ( host [ ":" port ] ) | pseudonym ; the name or pseudonym of the server adding ; the
     * Warning header, for use in debugging<br>
     * warn-text = quoted-string<br>
     * warn-date = HTTP-date</code>
     *
     * @since HTTP/1.1
     */
    String HD_WARNING = "Warning";

    /* HTTP Request Headers */

    /**
     * The Accept request-header field can be used to specify certain media types which are acceptable for the response. Accept
     * headers can be used to indicate that the request is specifically limited to a small set of desired types, as in the case
     * of a request for an in-line image.
     * <p>
     * <code>Accept = "Accept" ":" #( media-range [ accept-params ] )
     * media-range = ( "*\/*" | ( type "/" "*" ) | ( type "/" subtype ) ) *( ";" parameter ) accept-params = ";" "q" "=" qvalue *(
     * accept-extension ) accept-extension = ";" token [ "=" ( token | quoted-string ) ]</code>
     * <p>
     * Example:
     * <p>
     * <code>Accept: audio/*; q=0.2, audio/basic</code>
     *
     * @since HTTP/1.1
     */
    String HD_ACCEPT = "Accept";

    /**
     * The Accept-Charset request-header field can be used to indicate what character sets are acceptable for the response. This
     * field allows clients capable of understanding more comprehensive or special- purpose character sets to signal that
     * capability to a server which is capable of representing documents in those character sets.
     * <p>
     * <code>Accept-Charset = "Accept-Charset" ":" 1#( ( charset | "*" )[ ";" "q" "=" qvalue ] )</code>
     * <p>
     * Example:
     * <p>
     * <code>Accept-Charset: iso-8859-5, unicode-1-1;q=0.8</code>
     *
     * @since HTTP/1.1
     */
    String HD_ACCEPT_CHARSET = "Accept-Charset";

    /**
     * The Accept-Encoding request-header field is similar to Accept, but restricts the content-codings (section 3.5) that are
     * acceptable in the response.
     * <p>
     * <code>Accept-Encoding = "Accept-Encoding" ":"
     *    1#( codings [ ";" "q" "=" qvalue ] )<br>
     * codings          = ( content-coding | "*" )</code>
     * <p>
     * Example: <code>Accept-Encoding: gzip;q=1.0, identity; q=0.5, *;q=0</code>
     *
     * @since HTTP/1.1
     */
    String HD_ACCEPT_ENCODING = "Accept-Encoding";

    /**
     * The Accept-Language request-header field is similar to Accept, but restricts the set of natural languages that are
     * preferred as a response to the request.
     * <p>
     * <code>Accept-Language = "Accept-Language" ":" 1#( language-range [ ";" "q" "=" qvalue ] ) language-range = ( ( 1*8ALPHA *( "-"
     * 1*8ALPHA ) ) | "*" )</code>
     * <p>
     * Example:
     * <p>
     * <code>Accept-Language: da, en-gb;q=0.8, en;q=0.7</code>
     *
     * @since HTTP/1.1
     */
    String HD_ACCEPT_LANGUAGE = "Accept-Language";

    /**
     * A user agent that wishes to authenticate itself with a server-- usually, but not necessarily, after receiving a 401
     * response--does so by including an Authorization request-header field with the request. The Authorization field value
     * consists of credentials containing the authentication information of the user agent for the realm of the resource being
     * requested.
     * <p>
     * <code>Authorization = "Authorization" ":" credentials</code>
     *
     * @since HTTP/1.0
     */
    String HD_AUTHORIZATION = "Authorization";

    /**
     * The Expect request-header field is used to indicate that particular server behaviors are required by the client.
     * <p>
     * <code>Expect = "Expect" ":" 1#expectation<br>
     * expectation = "100-continue" | expectation-extension<br>
     * expectation-extension = token [ "=" ( token | quoted-string )<br>
     * *expect-params ]<br>
     * expect-params = ";" token [ "=" ( token | quoted-string ) ]</code>
     *
     * @since HTTP/1.1
     */
    String HD_EXPECT = "Expect";

    /**
     * The From request-header field, if given, SHOULD contain an Internet e-mail address for the human user who controls the
     * requesting user agent. The address SHOULD be machine-usable, as defined by "mailbox" in RFC 822 [9] as updated by RFC
     * 1123 [8]:
     * <p>
     * <code>From = "From" ":" mailbox</code>
     * <p>
     * Example:
     * <p>
     * <code>From: webmaster@w3.org</code>
     *
     * @since HTTP/1.0
     */
    String HD_FROM = "From";

    /**
     * The Host request-header field specifies the Internet host and port number of the resource being requested, as obtained
     * from the original URI given by the user or referring resource (generally an HTTP URL,as described in section 3.2.2). The
     * Host field value MUST represent the naming authority of the origin server or gateway given by the original URL. This
     * allows the origin server or gateway to differentiate between internally-ambiguous URLs, such as the root "/" URL of a
     * server for multiple host names on a single IP address.
     * <p>
     * <code>Host = "Host" ":" host [ ":" port ]</code>
     * <p>
     * A "host" without any trailing port information implies the default port for the service requested (e.g., "80" for an HTTP
     * URL). For example, a request on the origin server for &lt;http://www.w3.org/pub/WWW/&gt; would properly include:
     * <p>
     * Example:
     * <p>
     * <code>GET /pub/WWW/ HTTP/1.1<br>
     * Host: www.w3.org</code>
     *
     * @since HTTP/1.1
     */
    String HD_HOST = "Host";

    /**
     * The If-Match request-header field is used with a method to make it conditional. A client that has one or more entities
     * previously obtained from the resource can verify that one of those entities is current by including a list of their
     * associated entity tags in the If-Match header field. Entity tags are defined in section 3.11. The purpose of this feature
     * is to allow efficient updates of cached information with a minimum amount of transaction overhead. It is also used, on
     * updating requests, to prevent inadvertent modification of the wrong version of a resource. As a special case, the value
     * "*" matches any current entity of the resource.
     * <p>
     * <code>If-Match = "If-Match" ":" ( "*" | 1#entity-tag )</code>
     * <p>
     * Example:
     * <p>
     * <code>If-Match: "xyzzy", "r2d2xxxx", "c3piozzzz"</code>
     *
     * @since HTTP/1.1
     */
    String HD_IF_MATCH = "If-Match";

    /**
     * The If-Modified-Since request-header field is used with a method to make it conditional: if the requested variant has not
     * been modified since the time specified in this field, an entity will not be returned from the server; instead, a 304 (not
     * modified) response will be returned without any message-body.
     * <p>
     * <code>If-Modified-Since = "If-Modified-Since" ":" HTTP-date</code>
     * <p>
     * Example:
     * <p>
     * <code>If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT</code>
     *
     * @since HTTP/1.0
     */
    String HD_IF_MODIFIED_SINCE = "If-Modified-Since";

    /**
     * The If-None-Match request-header field is used with a method to make it conditional. A client that has one or more
     * entities previously obtained from the resource can verify that none of those entities is current by including a list of
     * their associated entity tags in the If-None-Match header field. The purpose of this feature is to allow efficient updates
     * of cached information with a minimum amount of transaction overhead. It is also used to prevent a method (e.g. PUT) from
     * inadvertently modifying an existing resource when the client believes that the resource does not exist.
     * <p>
     * As a special case, the value "*" matches any current entity of the resource.
     * <p>
     * <code>If-None-Match = "If-None-Match" ":" ( "*" | 1#entity-tag )</code>
     * <p>
     * Example:
     * <p>
     * <code>If-None-Match: W/"xyzzy", W/"r2d2xxxx", W/"c3piozzzz"</code>
     *
     * @since HTTP/1.1
     */
    String HD_IF_NONE_MATCH = "If-None-Match";

    /**
     * If a client has a partial copy of an entity in its cache, and wishes to have an up-to-date copy of the entire entity in
     * its cache, it could use the Range request-header with a conditional GET (using either or both of If-Unmodified-Since and
     * If-Match.) However, if the condition fails because the entity has been modified, the client would then have to make a
     * second request to obtain the entire current entity-body.
     * <p>
     * The If-Range header allows a client to "short-circuit" the second request. Informally, its meaning is `if the entity is
     * unchanged, send me the part(s) that I am missing; otherwise, send me the entire new entity'.
     * <p>
     * <code>If-Range = "If-Range" ":" ( entity-tag | HTTP-date )</code>
     *
     * @since HTTP/1.1
     */
    String HD_IF_RANGE = "If-Range";

    /**
     * The If-Unmodified-Since request-header field is used with a method to make it conditional. If the requested resource has
     * not been modified since the time specified in this field, the server SHOULD perform the requested operation as if the
     * If-Unmodified-Since header were not present.
     * <p>
     * If the requested variant has been modified since the specified time, the server MUST NOT perform the requested operation,
     * and MUST return a 412 (Precondition Failed).
     * <p>
     * <code>If-Unmodified-Since = "If-Unmodified-Since" ":" HTTP-date</code>
     * <p>
     * Example:
     * <p>
     * <code>If-Unmodified-Since: Sat, 29 Oct 1994 19:43:31 GMT</code>
     *
     * @since HTTP/1.1
     */
    String HD_IF_UNMODIFIED_SINCE = "If-Unmodified-Since";

    /**
     * The Max-Forwards request-header field provides a mechanism with the TRACE (section 9.8) and OPTIONS (section 9.2) methods
     * to limit the number of proxies or gateways that can forward the request to the next inbound server. This can be useful
     * when the client is attempting to trace a request chain which appears to be failing or looping in mid-chain.
     * <p>
     * <code>Max-Forwards = "Max-Forwards" ":" 1*DIGIT</code>
     *
     * @since HTTP/1.1
     */
    String HD_MAX_FORWARDS = "Max-Forwards";

    /**
     * The Proxy-Authorization request-header field allows the client to identify itself (or its user) to a proxy which requires
     * authentication. The Proxy-Authorization field value consists of credentials containing the authentication information of
     * the user agent for the proxy and/or realm of the resource being requested.
     * <p>
     * <code>Proxy-Authorization = "Proxy-Authorization" ":" credentials</code>
     *
     * @since HTTP/1.1
     */
    String HD_PROXY_AUTHORIZATION = "Proxy-Authorization";

    /**
     * HTTP retrieval requests using conditional or unconditional GET methods MAY request one or more sub-ranges of the entity,
     * instead of the entire entity, using the Range request header, which applies to the entity returned as the result of the
     * request:
     * <p>
     * <code>Range = "Range" ":" ranges-specifier</code>
     *
     * @since HTTP/1.1
     */
    String HD_RANGE = "Range";

    /**
     * The Referer[sic] request-header field allows the client to specify, for the server's benefit, the address (URI) of the
     * resource from which the Request-URI was obtained (the "referrer", although the header field is misspelled.) The Referer
     * request-header allows a server to generate lists of back-links to resources for interest, logging, optimized caching,
     * etc. It also allows obsolete or mistyped links to be traced for maintenance. The Referer field MUST NOT be sent if the
     * Request-URI was obtained from a source that does not have its own URI, such as input from the user keyboard.
     * <p>
     * <code>Referer = "Referer" ":" ( absoluteURI | relativeURI )</code>
     * <p>
     * Example:
     * <p>
     * <code>Referer: http://www.w3.org/hypertext/DataSources/Overview.html</code>
     *
     * @since HTTP/1.0
     */
    String HD_REFERER = "Referer";

    /**
     * The TE request-header field indicates what extension transfer-codings it is willing to accept in the response and whether
     * or not it is willing to accept trailer fields in a chunked transfer-coding. Its value may consist of the keyword
     * "trailers" and/or a comma-separated list of extension transfer-coding names with optional accept parameters (as described
     * in section 3.6).
     * <p>
     * <code>TE = "TE" ":" #( t-codings ) t-codings = "trailers" | ( transfer-extension [ accept-params ] )</code>
     * <p>
     * Example:
     * <p>
     * <code>TE: trailers, deflate;q=0.5</code>
     *
     * @since HTTP/1.1
     */
    String HD_TE = "TE";

    /**
     * The User-Agent request-header field contains information about the user agent originating the request. This is for
     * statistical purposes, the tracing of protocol violations, and automated recognition of user agents for the sake of
     * tailoring responses to avoid particular user agent limitations. User agents SHOULD include this field with requests. The
     * field can contain multiple product tokens (section 3.8) and comments identifying the agent and any subproducts which form
     * a significant part of the user agent. By convention, the product tokens are listed in order of their significance for
     * identifying the application.
     * <p>
     * <code>User-Agent = "User-Agent" ":" 1*( product | comment )</code>
     * <p>
     * Example:
     * <p>
     * <code>User-Agent: CERN-LineMode/2.15 libwww/2.17b3</code>
     *
     * @since HTTP/1.0
     */
    String HD_USER_AGENT = "User-Agent";

    /* HTTP Response Headers */

    /**
     * The Accept-Ranges response-header field allows the server to indicate its acceptance of range requests for a resource:
     * <p>
     * <code>Accept-Ranges = "Accept-Ranges" ":" acceptable-ranges acceptable-ranges = 1#range-unit | "none"</code>
     * <p>
     * Example:
     * <p>
     * <code>Accept-Ranges: bytes</code>
     *
     * @since HTTP/1.1
     */
    String HD_ACCEPT_RANGES = "Accept-Ranges";

    /**
     * The Age response-header field conveys the sender's estimate of the amount of time since the response (or its
     * revalidation) was generated at the origin server. A cached response is "fresh" if its age does not exceed its freshness
     * lifetime.
     * <p>
     * <code>Age = "Age" ":" age-value age-value = delta-seconds</code>
     * <p>
     * Example:
     * <p>
     * <code>Age: 1000</code>
     *
     * @since HTTP/1.1
     */
    String HD_AGE = "Age";

    /**
     * The ETag response-header field provides the current value of the entity tag for the requested variant. The entity tag MAY
     * be used for comparison with other entities from the same resource.
     * <p>
     * <code>ETag = "ETag" ":" entity-tag</code>
     * <p>
     * Examples:
     * <p>
     * <code>ETag: W/"xyzzy"</code>
     *
     * @since HTTP/1.1
     */
    String HD_ETAG = "ETag";

    /**
     * The Location response-header field is used to redirect the recipient to a location other than the Request-URI for
     * completion of the request or identification of a new resource. For 201 (Created) responses, the Location is that of the
     * new resource which was created by the request. For 3xx responses, the location SHOULD indicate the server's preferred URI
     * for automatic redirection to the resource. The field value consists of a single absolute URI.
     * <p>
     * <code>Location = "Location" ":" absoluteURI</code>
     * <p>
     * Example:
     * <p>
     * <code>Location: http://www.w3.org/pub/WWW/People.html</code>
     *
     * @since HTTP/1.0
     */
    String HD_LOCATION = "Location";

    /**
     * The Proxy-Authenticate response-header field MUST be included as part of a 407 (Proxy Authentication Required) response.
     * The field value consists of a challenge that indicates the authentication scheme and parameters applicable to the proxy
     * for this Request-URI.
     * <p>
     * <code>Proxy-Authenticate = "Proxy-Authenticate" ":" 1#challenge</code>
     *
     * @since HTTP/1.1
     */
    String HD_PROXY_AUTHENTICATE = "Proxy-Authenticate";

    /**
     * The Retry-After response-header field can be used with a 503 (Service Unavailable) response to indicate how long the
     * service is expected to be unavailable to the requesting client. This field MAY also be used with any 3xx (Redirection)
     * response to indicate the minimum time the user-agent is asked wait before issuing the redirected request. The value of
     * this field can be either an HTTP-date or an integer number of seconds (in decimal) after the time of the response.
     * <p>
     * <code>Retry-After = "Retry-After" ":" ( HTTP-date | delta-seconds )</code>
     * <p>
     * Example:
     * <p>
     * <code>Retry-After: Fri, 31 Dec 1999 23:59:59 GMT<br>
     * Retry-After: 120</code>
     *
     * @since HTTP/1.1
     */
    String HD_RETRY_AFTER = "Retry-After";

    /**
     * The Server response-header field contains information about the software used by the origin server to handle the request.
     * The field can contain multiple product tokens (section 3.8) and comments identifying the server and any significant
     * subproducts. The product tokens are listed in order of their significance for identifying the application.
     * <p>
     * <code>Server = "Server" ":" 1*( product | comment )</code>
     * <p>
     * Example:
     * <p>
     * <code>Server: CERN/3.0 libwww/2.17</code>
     *
     * @since HTTP/1.0
     */
    String HD_SERVER = "Server";

    /**
     * The Vary field value indicates the set of request-header fields that fully determines, while the response is fresh,
     * whether a cache is permitted to use the response to reply to a subsequent request without revalidation. For uncacheable
     * or stale responses, the Vary field value advises the user agent about the criteria that were used to select the
     * representation. A Vary field value of "*" implies that a cache cannot determine from the request headers of a subsequent
     * request whether this response is the appropriate representation. See section 13.6 for use of the Vary header field by
     * caches.
     * <p>
     * <code>Vary = "Vary" ":" ( "*" | 1#field-name )</code>
     *
     * @since HTTP/1.1
     */
    String HD_VARY = "Vary";

    /**
     * The WWW-Authenticate response-header field MUST be included in 401 (Unauthorized) response messages. The field value
     * consists of at least one challenge that indicates the authentication scheme(s) and parameters applicable to the
     * Request-URI.
     * <p>
     * <code>WWW-Authenticate = "WWW-Authenticate" ":" 1#challenge</code>
     * <p>
     * Example: <code>WWW-Authenticate: Basic realm="WallyWorld"</code>
     *
     * @since HTTP/1.0
     */
    String HD_WWW_AUTHENTICATE = "WWW-Authenticate";

    /* HTTP Entity Headers */

    /**
     * The Allow entity-header field lists the set of methods supported by the resource identified by the Request-URI. The
     * purpose of this field is strictly to inform the recipient of valid methods associated with the resource. An Allow header
     * field MUST be present in a 405 (Method Not Allowed) response.
     * <p>
     * <code>Allow = "Allow" ":" #Method</code>
     * <p>
     * Example:
     * <p>
     * <code>Allow: GET, HEAD, PUT</code>
     *
     * @since HTTP/1.0
     */
    String HD_ALLOW = "Allow";

    /**
     * The Content-Encoding entity-header field is used as a modifier to the media-type. When present, its value indicates what
     * additional content codings have been applied to the entity-body, and thus what decoding mechanisms must be applied in
     * order to obtain the media-type referenced by the Content-Type header field. Content-Encoding is primarily used to allow a
     * document to be compressed without losing the identity of its underlying media type.
     * <p>
     * <code>Content-Encoding = "Content-Encoding" ":" 1#content-coding</code>
     * <p>
     * Example:
     * <p>
     * <code>Content-Encoding: gzip</code>
     *
     * @since HTTP/1.0
     */
    String HD_CONTENT_ENCODING = "Content-Encoding";

    /**
     * The Content-Language entity-header field describes the natural language(s) of the intended audience for the enclosed
     * entity. Note that this might not be equivalent to all the languages used within the entity-body.
     * <p>
     * <code>Content-Language = "Content-Language" ":" 1#language-tag</code>
     * <p>
     * Example:
     * <p>
     * <code>Content-Language: da</code>
     *
     * @since HTTP/1.1
     */
    String HD_CONTENT_LANGUAGE = "Content-Language";

    /**
     * The Content-Length entity-header field indicates the size of the entity-body, in decimal number of OCTETs, sent to the
     * recipient or, in the case of the HEAD method, the size of the entity-body that would have been sent had the request been
     * a GET.
     * <p>
     * <code>Content-Length = "Content-Length" ":" 1*DIGIT</code>
     * <p>
     * Example:
     * <p>
     * <code>Content-Length: 3495</code>
     *
     * @since HTTP/1.0
     */
    String HD_CONTENT_LENGTH = "Content-Length";

    /**
     * The Content-Location entity-header field MAY be used to supply the resource location for the entity enclosed in the
     * message when that entity is accessible from a location separate from the requested resource's URI. A server SHOULD
     * provide a Content-Location for the variant corresponding to the response entity; especially in the case where a resource
     * has multiple entities associated with it, and those entities actually have separate locations by which they might be
     * individually accessed, the server SHOULD provide a Content-Location for the particular variant which is returned.
     * <p>
     * <code>Content-Location = "Content-Location" ":" ( absoluteURI | relativeURI )</code>
     *
     * @since HTTP/1.1
     */
    String HD_CONTENT_LOCATION = "Content-Location";

    /**
     * The Content-MD5 entity-header field, as defined in RFC 1864 [23], is an MD5 digest of the entity-body for the purpose of
     * providing an end-to-end message integrity check (MIC) of the entity-body. (Note: a MIC is good for detecting accidental
     * modification of the entity-body in transit, but is not proof against malicious attacks.)
     * <p>
     * <code>Content-MD5 = "Content-MD5" ":" md5-digest<br>
     * md5-digest = &lt;base64 of 128 bit MD5 digest as per RFC 1864&gt;</code>
     *
     * @since HTTP/1.1
     */
    String HD_CONTENT_MD5 = "Content-MD5";

    /**
     * The Content-Range entity-header is sent with a partial entity-body to specify where in the full entity-body the partial
     * body should be applied. Range units are defined in section 3.12.
     * <p>
     * <code>Content-Range = "Content-Range" ":" content-range-spec<br>
     * content-range-spec = byte-content-range-spec<br>
     * byte-content-range-spec = bytes-unit SP<br>
     * byte-range-resp-spec "/"<br>
     * ( instance-length | "*" )<br>
     * byte-range-resp-spec = (first-byte-pos "-" last-byte-pos)<br>
     * | "*"<br>
     * instance-length = 1*DIGIT</code>
     *
     * @since HTTP/1.1
     */
    String HD_CONTENT_RANGE = "Content-Range";

    /**
     * The Content-Type entity-header field indicates the media type of the entity-body sent to the recipient or, in the case of
     * the HEAD method, the media type that would have been sent had the request been a GET.
     * <p>
     * <code>Content-Type = "Content-Type" ":" media-type</code>
     * <p>
     * Example:
     * <p>
     * <code>Content-Type: text/html; charset=ISO-8859-4</code>
     *
     * @since HTTP/1.0
     */
    String HD_CONTENT_TYPE = "Content-Type";

    /**
     * The Expires entity-header field gives the date/time after which the response is considered stale. A stale cache entry may
     * not normally be returned by a cache (either a proxy cache or a user agent cache) unless it is first validated with the
     * origin server (or with an intermediate cache that has a fresh copy of the entity).
     * <p>
     * The presence of an Expires field does not imply that the original resource will change or cease to exist at, before, or
     * after that time.
     * <p>
     * The format is an absolute date and time as defined by HTTP-date; it MUST be in RFC 1123 date format:
     * <p>
     * <code>Expires = "Expires" ":" HTTP-date</code>
     * <p>
     * Example:
     * <p>
     * <code>Expires: Thu, 01 Dec 1994 16:00:00 GMT</code>
     *
     * @since HTTP/1.0
     */
    String HD_EXPIRES = "Expires";

    /**
     * The Last-Modified entity-header field indicates the date and time at which the origin server believes the variant was
     * last modified.
     * <p>
     * <code>Last-Modified = "Last-Modified" ":" HTTP-date</code>
     * <p>
     * Example:
     * <p>
     * <code>Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT</code>
     *
     * @since HTTP/1.0
     */
    String HD_LAST_MODIFIED = "Last-Modified";

}
