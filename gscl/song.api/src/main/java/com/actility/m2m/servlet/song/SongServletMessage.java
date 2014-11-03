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
 * id $Id: SongServletMessage.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import java.io.UnsupportedEncodingException;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.ListIterator;

import com.actility.m2m.servlet.ApplicationSession;

/**
 * Defines common aspects of SONG requests and responses. The Servlet API is defined with an implicit assumption that servlets
 * receives requests from clients, inspects various aspects of the corresponding ServletRequest object, and generates a response
 * by setting various attributes of a ServletResponse object. This model fits HTTP well, because HTTP servlets always execute
 * origin servers; they execute only to process incoming requests and never initiates HTTP requests of their own. SONG services,
 * on the other hand, does need to be able to initiate requests of their own. This implies that SONG request and response
 * classes are more symmetric, that is, requests must be writable as well as readable, and likewise, responses must be readable
 * as well as writable. The SongServletMessage interface defines a number of methods which are common to SongServletRequest and
 * SongServletResponse, for example setters and getters for message headers and content.
 */
public interface SongServletMessage {

    /* SONG Request Headers */

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
     * <code>Accept: application/*; q=0.2, application/xml</code>
     *
     * @since SONG/1.0
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
     * @since SONG/1.0
     */
    String HD_ACCEPT_CHARSET = "Accept-Charset";

    /**
     * The Accept-Encoding request-header field is similar to Accept, but restricts the content-codings that are acceptable in
     * the response.
     * <p>
     * <code>Accept-Encoding = "Accept-Encoding" ":"
     *    1#( codings [ ";" "q" "=" qvalue ] )<br />
     * codings          = ( content-coding | "*" )</code>
     * <p>
     * Example: <code>Accept-Encoding: gzip;q=1.0, identity; q=0.5, *;q=0</code>
     *
     * @since SONG/1.0
     */
    String HD_ACCEPT_ENCODING = "Accept-Encoding";

    /**
     * The If-Modified-Since request-header field is used with a method to make it conditional: if the requested variant has not
     * been modified since the time specified in this field, an entity will not be returned from the server; instead, a 304 (not
     * modified) response will be returned without any message-body.
     * <p>
     * <code>If-Modified-Since = "If-Modified-Since" ":" SONG-date</code>
     * <p>
     * Example:
     * <p>
     * <code>If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT</code>
     *
     * @since SONG/1.0
     */
    String HD_IF_MODIFIED_SINCE = "If-Modified-Since";

    /**
     * The If-Unmodified-Since request-header field is used with a method to make it conditional. If the requested resource has
     * not been modified since the time specified in this field, the server SHOULD perform the requested operation as if the
     * If-Unmodified-Since header were not present.
     * <p>
     * If the requested variant has been modified since the specified time, the server MUST NOT perform the requested operation,
     * and MUST return a 412 (Precondition Failed).
     * <p>
     * <code>If-Unmodified-Since = "If-Unmodified-Since" ":" SONG-date</code>
     * <p>
     * Example:
     * <p>
     * <code>If-Unmodified-Since: Sat, 29 Oct 1994 19:43:31 GMT</code>
     *
     * @since SONG/1.0
     */
    String HD_IF_UNMODIFIED_SINCE = "If-Unmodified-Since";

    /**
     * The If-Match request-header field is used with a method to make it conditional. A client that has one or more entities
     * previously obtained from the resource can verify that one of those entities is current by including a list of their
     * associated entity tags in the If-Match header field. The purpose of this feature is to allow efficient updates of cached
     * information with a minimum amount of transaction overhead. It is also used, on updating requests, to prevent inadvertent
     * modification of the wrong version of a resource. As a special case, the value "*" matches any current entity of the
     * resource.
     * <p>
     * <code>If-Match = "If-Match" ":" ( "*" | 1#entity-tag )</code>
     * <p>
     * Example:
     * <p>
     * <code>If-Match: eTag1, eTag2</code>
     *
     * @since SONG/3.0
     */
    String HD_IF_MATCH = "If-Match";

    /**
     * The If-None-Match request-header field is used with a method to make it conditional. A client that has one or more
     * entities previously obtained from the resource can verify that none of those entities is current by including a list of
     * their associated entity tags in the If-None-Match header field. The purpose of this feature is to allow efficient updates
     * of cached information with a minimum amount of transaction overhead. It is also used to prevent a method (e.g. PUT) from
     * inadvertently modifying an existing resource when the client believes that the resource does not exist. As a special
     * case, the value "*" matches any current entity of the resource.
     * <p>
     * <code>If-None-Match = "If-None-Match" ":" ( "*" | 1#entity-tag )</code>
     * <p>
     * Example:
     * <p>
     * <code>If-None-Match: eTag1, eTag2</code>
     *
     * @since SONG/3.0
     */
    String HD_IF_NONE_MATCH = "If-None-Match";

    /**
     * The Max-Forwards request-header field provides a mechanism to limit the number of proxies or gateways that can forward
     * the request to the next inbound server.
     * <p>
     * <code>Max-Forwards = "Max-Forwards" ":" 1*DIGIT</code>
     *
     * @since SONG/1.0
     */
    String HD_MAX_FORWARDS = "Max-Forwards";

    /**
     * The User-Agent request-header field contains information about the user agent originating the request. This is for
     * statistical purposes, the tracing of protocol violations, and automated recognition of user agents for the sake of
     * tailoring responses to avoid particular user agent limitations. User agents SHOULD include this field with requests. The
     * field can contain multiple product tokens and comments identifying the agent and any subproducts which form a significant
     * part of the user agent. By convention, the product tokens are listed in order of their significance for identifying the
     * application.
     * <p>
     * <code>User-Agent = "User-Agent" ":" 1*( product | comment )</code>
     * <p>
     * Example:
     * <p>
     * <code>User-Agent: M2M GSCL/2.0</code>
     *
     * @since SONG/1.0
     */
    String HD_USER_AGENT = "User-Agent";

    String HD_X_ETSI_CORRELATION_ID = "X-etsi-correlationID";

    String HD_X_ETSI_CONTACT_URI = "X-etsi-contactURI";

    /* SONG Response Headers */

    /**
     * The ETag response-header field provides the current value of the entity tag for the requested variant. The entity tag MAY
     * be used for comparison with other entities from the same resource.
     * <p>
     * <code>ETag = "ETag" ":" entity-tag</code>
     * <p>
     * Example:
     * <p>
     * <code>ETag: "xyzzy"</code>
     *
     * @since SONG/3.0
     */
    String HD_ETAG = "ETag";

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
     * @since SONG/3.0
     */
    String HD_LAST_MODIFIED = "Last-Modified";

    /**
     * The Retry-After response-header field can be used with a 503 (Service Unavailable) response to indicate how long the
     * service is expected to be unavailable to the requesting client. This field MAY also be used with any 3xx (Redirection)
     * response to indicate the minimum time the user-agent is asked wait before issuing the redirected request. The value of
     * this field can be either an SONG-date or an integer number of seconds (in decimal) after the time of the response.
     * <p>
     * <code>Retry-After = "Retry-After" ":" ( SONG-date | delta-seconds )</code>
     * <p>
     * Example:
     * <p>
     * <code>Retry-After: Fri, 31 Dec 1999 23:59:59 GMT<br />
     * Retry-After: 120</code>
     *
     * @since SONG/1.0
     */
    String HD_RETRY_AFTER = "Retry-After";

    /**
     * The Server response-header field contains information about the software used by the origin server to handle the request.
     * The field can contain multiple product tokens and comments identifying the server and any significant subproducts. The
     * product tokens are listed in order of their significance for identifying the application.
     * <p>
     * <code>Server = "Server" ":" 1*( product | comment )</code>
     * <p>
     * Example:
     * <p>
     * <code>Server: M2M GSCL/2.0</code>
     *
     * @since SONG/1.0
     */
    String HD_SERVER = "Server";

    /* SONG Entity Headers */

    /**
     * The Content-Location entity-header field MAY be used to supply the resource location for the entity enclosed in the
     * message when that entity is accessible from a location separate from the requested resource's URI.
     * <p>
     * <code>Content-Location = "Content-Location" ":" absoluteURI</code>
     * <p>
     * Example:
     * <p>
     * <code>Location: http://gsc1.actility.com/apps/app1/</code>
     *
     * @since SONG/3.0
     */
    String HD_CONTENT_LOCATION = "Content-Location";

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
     * @since SONG/1.0
     */
    String HD_CONTENT_ENCODING = "Content-Encoding";

    /**
     * The Content-Length entity-header field indicates the size of the entity-body, in decimal number of OCTETs, sent to the
     * recipient.
     * <p>
     * <code>Content-Length = "Content-Length" ":" 1*DIGIT</code>
     * <p>
     * Example:
     * <p>
     * <code>Content-Length: 3495</code>
     *
     * @since SONG/1.0
     */
    String HD_CONTENT_LENGTH = "Content-Length";

    /**
     * The Content-MD5 entity-header field, as defined in RFC 1864, is an MD5 digest of the entity-body for the purpose of
     * providing an end-to-end message integrity check (MIC) of the entity-body. (Note: a MIC is good for detecting accidental
     * modification of the entity-body in transit, but is not proof against malicious attacks.)
     * <p>
     * <code>Content-MD5 = "Content-MD5" ":" md5-digest<br />
     * md5-digest = &lt;base64 of 128 bit MD5 digest as per RFC 1864></code>
     *
     * @since SONG/1.0
     */
    String HD_CONTENT_MD5 = "Content-MD5";

    /**
     * The Content-Type entity-header field indicates the media type of the entity-body sent to the recipient.
     * <p>
     * <code>Content-Type = "Content-Type" ":" media-type</code>
     * <p>
     * Example:
     * <p>
     * <code>Content-Type: application/xml; charset=utf-8</code>
     *
     * @since SONG/1.0
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
     * The format is an absolute date and time as defined by SONG-date; it MUST be in RFC 1123 date format:
     * <p>
     * <code>Expires = "Expires" ":" SONG-date</code>
     * <p>
     * Example:
     * <p>
     * <code>Expires: Thu, 01 Dec 1994 16:00:00 GMT</code>
     *
     * @since SONG/1.0
     */
    String HD_EXPIRES = "Expires";

    /**
     * Adds a header with the given name and value. This method allows headers to have multiple values. The container MAY check
     * that the specified header field can legally appear in the this message. Note: applications should never attempt to set
     * system headers.
     *
     * @param name The name of the header to add
     * @param value The value of the header to add
     */
    void addHeader(java.lang.String name, java.lang.String value);

    /**
     * Adds the specified Parameterable as a new value of the named header field. The parameterable is added as the last header
     * field value. This method can be used with headers which are defined to contain one or more entries matching field-value
     * *(;parameter-name=parameter-value).
     *
     * @param name The name of the parameterable header to add
     * @param param The parameterable header to add
     * @param first Whether to add this header first in the list of the multi-valued header
     */
    void addParameterableHeader(java.lang.String name, Parameterable param, boolean first);

    /**
     * Returns the application session to which this message belongs. If the session doesn't already exist it is created.
     *
     * @return The application session to which the message belongs
     */
    ApplicationSession getApplicationSession();

    /**
     * Returns the application session to which this message belongs.
     *
     * @param create Whether to create the application session if it does not exist
     * @return The application session to which the message belongs or null if the session is not created and
     *         <code>create</code> is <code>false</code>
     */
    ApplicationSession getApplicationSession(boolean create);

    /**
     * Returns the value of the named attribute as an Object, or null if no attribute of the given name exists. Attributes can
     * be set two ways. The servlet container may set attributes to make available custom information about a request or a
     * response. For example, for requests made using HTTPS, the attribute javax.servlet.request.X509Certificate can be used to
     * retrieve information on the certificate of the client. Attributes can also be set programatically using
     * setAttribute(String, Object). This allows information to be embedded into a request or response before a
     * RequestDispatcher call. Attribute names should follow the same conventions as package names. Names beginning with
     * com.actility.servlet.song. are reserved for definition by the SONG Servlet API.
     *
     * @param name The name of the attribute to get
     * @return The attribute or null if it does not exist
     */
    java.lang.Object getAttribute(java.lang.String name);

    /**
     * Returns an Enumeration containing the names of the attributes available to this message object. This method returns an
     * empty Enumeration if the request has no attributes available to it.
     *
     * @return An Enumeration containing the names of the attributes available to this message
     */
    Enumeration getAttributeNames();

    /**
     * Returns the name of the charset used for the MIME body sent in this message. This method returns null if the message does
     * not specify a character encoding. The message character encoding is used when converting between bytes and characters. If
     * the character encoding hasn't been set explicitly UTF-8 will be used for this purpose. For more information about
     * character encodings and MIME see RFC 2045 (http://www.ietf.org/rfc/rfc2045.txt).
     *
     * @return The character encoding or null if it is not specified
     */
    java.lang.String getCharacterEncoding();

    /**
     * Returns the content as a Java object. The actual type of the returned object depends on the MIME type of the content
     * itself (the Content-Type). Containers are required to return a String object for MIME type text/plain as for other text/*
     * MIME types for which the container doesn't have specific knowledge. It is encouraged that the object returned for
     * "multipart" MIME content is a javax.mail.Multipart object. A byte array is returned for content-types that are unknown to
     * the container. The message's character encoding is used when the MIME type indicates that the content consists of
     * character data. Note: This method, together with setContent, is modelled over similar methods in the JavaMail API.
     * Whereas the JavaMail API mandates the use of the Java Activation Framework (JAF) as the underlying data handling system,
     * the SONG servlet API doesn't currently require JAF.
     *
     * @return The content of the message as an Object
     * @throws java.io.IOException If an IO problem occurs
     * @throws java.io.UnsupportedEncodingException If the character encoding is not supported
     */
    java.lang.Object getContent() throws java.io.IOException, java.io.UnsupportedEncodingException;

    /**
     * Returns the length in number of bytes of the content part of this message. This directly reflects the value of the
     * Content-Length header field.
     *
     * @return The length in number of bytes of the content part
     */
    int getContentLength();

    /**
     * Returns the value of the Content-Type header field.
     *
     * @return The content type of the message
     */
    java.lang.String getContentType();

    /**
     * Returns the value of the specified header as a String. If the request did not include a header of the specified name,
     * this method returns null. If multiple headers exist, the first one is returned. The header name is case insensitive.
     *
     * @param name The name of the header to get
     * @return The header or null if it does not exist
     */
    java.lang.String getHeader(java.lang.String name);

    /**
     * Returns an Iterator over all the header names this message contains. If the message has no headers, this method returns
     * an empty Iterator. Note: This is a fail-fast iterator and can throw ConcurrentModificationException if the underlying
     * implementation does not allow modification after the iterator is created. Some servlet containers do not allow servlets
     * to access headers using this method, in which case this method returns null.
     *
     * @return An Iterator over all the header names this message contains
     */
    Iterator getHeaderNames();

    /**
     * Returns all the values of the specified request header as a ListIterator over a number of String objects. The values
     * returned by the Iterator follow the order in which they appear in the message header. Some headers, such as
     * Accept-Language can be sent by clients as several headers each with a different value rather than sending the header as a
     * comma separated list. If the request did not include any headers of the specified name, this method returns an empty
     * Iterator. The header name is case insensitive. Note: This is a fail-fast iterator and can throw
     * ConcurrentModificationException if the underlying implementation does not allow modification after the iterator is
     * created. Attempts to modify the specified header field through the returned list iterator must fail with an
     * IllegalStateException if the header field is a system header.
     *
     * @param name The name of the header for which the list iterator is built
     * @return The list iterator over the name header
     */
    ListIterator getHeaders(java.lang.String name);

    /**
     * Returns the IP address of the interface this message was received on.
     *
     * @return The IP address of the interface this message was received on
     */
    java.lang.String getLocalAddr();

    /**
     * Returns the local port this message was received on.
     *
     * @return The local port this message was received on
     */
    int getLocalPort();

    /**
     * Returns the method of this message. This is a token consisting of all upper-case letters, for example "RETRIEVE". For
     * requests, the method is in the request line while for responses it may be deducted from its request.
     *
     * @return The method of the message
     */
    String getMethod();

    /**
     * Returns the value of the specified header field as a Parameterable object. This method can be used with headers which are
     * defined to contain one or more entries matching field-value *(;parameter-name=parameter-value). If there is more than one
     * header field value the first is returned.
     *
     * @param name The name of the Parameterable header to get
     * @return The Parameterable header or null if it does not exist
     * @throws ServletParseException If the header cannot be parsed as a Parameterable
     */
    Parameterable getParameterableHeader(java.lang.String name) throws ServletParseException;

    /**
     * Returns a ListIterator over all Parameterable header field values for the specified header name. The values returned by
     * the Iterator follow the order in which they appear in the message header. This method can be used with headers which are
     * defined to contain one or more entries matching field-value *(;parameter-name=parameter-value). Attempts to modify the
     * specified header field through the returned list iterator must fail with an IllegalStateException if the header field is
     * a system header. Note: This is a fail-fast iterator and can throw ConcurrentModificationException if the underlying
     * implementation does not allow modification after the iterator is created.
     *
     * @param name The name of the Parameterable header to get
     * @return The list iterator over the name Parameterable header
     * @throws ServletParseException If at least one of the header cannot be parsed as a Parameterable
     */
    ListIterator getParameterableHeaders(java.lang.String name) throws ServletParseException;

    /**
     * Returns the name and version of the protocol of this message. This is in the form {protocol} "/" {major-version-number}
     * "." {minor-version-number}, for example "SONG/1.0".
     *
     * @return The protocol of this message
     */
    String getProtocol();

    /**
     * Returns message content as a byte array. The reference is returned if the application wants to re-use the content for
     * another message it should make a copy.
     *
     * @return The message content as a byte array
     * @throws java.io.IOException If an IO problem occurs
     */
    byte[] getRawContent() throws java.io.IOException;

    /**
     * Returns the IP address of the next upstream/downstream hop from which this message was received.
     *
     * <br/>
     * If the message was internally routed (from one application to the next within the same container), then this method
     * returns the address of the container's protocol interface.
     *
     * @return The remote IP address of the message
     */
    java.lang.String getRemoteAddr();

    /**
     * Returns the port number of the next upstream/downstream hop from which this message was received. <br/>
     * If the message was internally routed (from one application to the next within the same container), then this method
     * returns a valid port number chosen by the container or the host TCP/IP stack.
     *
     * @return The remote port number of the message
     */
    int getRemotePort();

    /**
     * Returns the login of the user sending this message, if the user has been authenticated, or null if the user has not been
     * authenticated.
     *
     * @return The login of the authenticated user or null
     */
    java.lang.String getRemoteUser();

    /**
     * Returns the name of the protocol with which this message was received, e.g. "UDP", "TCP", "TLS", or "SCTP".
     *
     * @return The name of the underlying protocol
     */
    java.lang.String getTransport();

    /**
     * Returns true if this message is committed, that is, if one of the following conditions is true: This message is an
     * incoming request for which a final response has already been generated. This message is an outgoing request which has
     * already been sent. This message is an incoming response received by a servlet acting as a UAC. This message is a response
     * which has already been forwarded upstream.
     *
     * @return Whether the message is committed
     */
    boolean isCommitted();

    /**
     * Returns a boolean indicating whether this message was received over a secure channel, such as TLS.
     *
     * @return Whether this message was received over a secure channel
     */
    boolean isSecure();

    /**
     * Returns a boolean indicating whether the authenticated user is included in the specified logical "role". Roles and role
     * membership can be defined using deployment descriptors. If the user has not been authenticated, the method returns false.
     *
     * @param role The role to check for the authenticated user
     * @return Whether the authenticated user owns the given role
     */
    boolean isUserInRole(java.lang.String role);

    /**
     * Removes the named attribute from this message. Nothing is done if the message did not already contain the specified
     * attribute.
     *
     * Attribute names should follow the same conventions as package names. Names beginning with com.actility.servlet.song.* are
     * reserved for definition by the SONG Servlet API.
     *
     * @param name a String specifying the name of the attribute
     * @throws NullPointerException if name is null.
     */
    void removeAttribute(java.lang.String name);

    /**
     * Removes the specified header. If multiple headers exists with the given name, they're all removed.
     *
     * @param name The name of the header to remove
     */
    void removeHeader(java.lang.String name);

    /**
     * Sends this SongServletMessage.
     *
     * @throws java.io.IOException If an IO problem occurs
     */
    void send() throws java.io.IOException;

    /**
     * Stores an attribute in this message. Attributes are reset between messages. This method is most often used in conjunction
     * with RequestDispatcher. Attribute names should follow the same conventions as package names. Names beginning with
     * com.actility.servlet.song.* are reserved for definition by the SONG Servlet API.
     *
     * @param name The name of the attribute to set
     * @param o The attribute to set
     */
    void setAttribute(java.lang.String name, java.lang.Object o);

    /**
     * Overrides the name of the character encoding that will be used to convert the body of this message from bytes to
     * characters or vice versa. Explicitly setting a message's character encoding potentially affects the behavior of
     * subsequent calls to getContent() and setContent(java.lang.Object, java.lang.String). This method must be called prior to
     * calling either of those methods.
     *
     * @param enc The new encoding to use to parse the content message
     * @throws UnsupportedEncodingException If the specified encoding is not supported
     */
    void setCharacterEncoding(java.lang.String enc) throws UnsupportedEncodingException;

    /**
     * Sets the content of this message to the specified Object. This method only works if the implementation "knows about" the
     * specified object and MIME type. Containers are required to handle byte[] content with any MIME type. Furthermore,
     * containers are required to handle String content when used with a text/* content type. When invoked with non-String
     * objects and a text/* content type, containers may invoke toString() on the content Object in order to obtain the body's
     * character data. It is also recommended that implementations know how to handle javax.mail.Multipart content when used
     * together with "multipart" MIME types. When converting String content, this method may use the the message's character
     * encoding (as set by setCharacterEncoding(java.lang.String), setContentType(java.lang.String) or
     * setContentLanguage(java.util.Locale)) to map the String to a byte array. Note: This method, together with getContent(),
     * is modelled over a similar method in the JavaMail API. Whereas the JavaMail API mandates the use of the Java Activation
     * Framework (JAF) as the underlying data handling system, the SONG servlet API doesn't currently require JAF.
     *
     * @param content The content to set
     * @param contentType The content type to use for the given message content
     * @throws UnsupportedEncodingException If the character encoding extracted from the specified content type is unsupported
     */
    void setContent(java.lang.Object content, java.lang.String contentType) throws UnsupportedEncodingException;

    /**
     * Sets the value of the Content-Length header. Applications are discouraged from setting the Content-Length directly using
     * this method; they should instead use the setContent methods which guarantees that the Content-Length is computed and set
     * correctly.
     *
     * @param len The length to set for the content length
     */
    void setContentLength(int len);

    /**
     * Sets the content type of the response being sent to the client. The content type may include the type of character
     * encoding used, for example, text/html; charset=UTF-8. This will cause the message's current character encoding to be set.
     * If obtaining a PrintWriter or calling setContent, this method should be called first.
     *
     * @param type The content type to use for the message content
     */
    void setContentType(java.lang.String type);

    /**
     * Sets a header with the given name and value. If the header had already been set, the new value overwrites the previous
     * one.
     *
     * @param name The name of the header to set
     * @param value The value of the header to set
     */
    void setHeader(java.lang.String name, java.lang.String value);

    /**
     * Sets the header with the specified name to have the value specified by the address argument. This method can be used with
     * headers which are defined to contain one or more entries matching field-value *(;parameter-name=parameter-value) as
     * defined in RFC 3261.
     *
     * <p>
     * If the message did not include any headers of the specified name, this method returns an empty Iterator. If the message
     * included headers of the specified name with no values, this method returns an Iterator over empty <code>String</code>s.
     *
     * @param name The name of the Parameterable header to set
     * @param param The Parameterable header to set
     * @throws IllegalArgumentException if the specified header isn't defined to hold address values or if the specified header
     *             field is a system header
     */
    void setParameterableHeader(java.lang.String name, Parameterable param);

    /**
     * Returns the requestingEntity URI of this request.
     *
     * @return The requestingEntity URI of the request
     */
    SongURI getRequestingEntity();

    /**
     * Returns the targetID URI of this request.
     *
     * @return The targetID URI of the request
     */
    SongURI getTargetID();

    /**
     * Returns the ID associated to the SONG transaction.
     *
     * @return The ID associated to the SONG transaction
     */
    String getId();
}
