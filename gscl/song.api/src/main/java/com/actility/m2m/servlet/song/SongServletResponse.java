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
 * id $Id: SongServletResponse.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import javax.servlet.ServletResponse;

/**
 * Represents SONG responses. Instances of this class are passed to servlets when the container receives incoming SONG responses
 * and also, servlets acting as servers generates SONG responses of their own by creating SongServletResponses. SONG responses
 * has a three-digit status code that indicates the outcome of the corresponding request. 2xx and 3xx responses indicate a
 * successful outcome while 4xx-5xx indicate a variety of non-success conditions. The summary of status codes is:
 * <ul>
 * <li>2xx - 3xx: Success -- the action was successfully received, understood, and accepted</li>
 * <li>4xx: Client Error -- the request contains bad syntax or cannot be fulfilled at this server</li>
 * <li>5xx: Server Error -- the server failed to fulfill an apparently valid request</li>
 * </ul>
 * Note: Because of a change in Servlet spec 2.4 the setCharacterEncoding() does NOT throw the
 * java.io.UnsupportedEncodingException as derived from SongServletMessage.setCharacterEncoding(String) but inherits a more
 * generic setCharacterEncoding() method from the javax.servlet.ServletResponse.
 */
public interface SongServletResponse extends SongServletMessage, ServletResponse {
    /**
     * Status code (200) indicating the request succeeded normally.
     */
    int SC_OK = 200;

    /**
     * Status code (201) indicating the request has been fulfilled and resulted in a new resource being created. The newly
     * created resource can be referenced by the URI(s) returned in the entity of the response, with the most specific URI for
     * the resource given by a Content-Location header field. The entity format is specified by the media type given in the
     * Content-Type header field. The origin server MUST create the resource before returning the 201 status code. If the action
     * cannot be carried out immediately, the server SHOULD respond with 202 (Accepted) response instead.
     */
    int SC_CREATED = 201;

    /**
     * Status code (202) indicating that the request has been accepted for processing, but the processing has not been
     * completed. The request will be responded later with a final response using semi-asynchronous mechanism.
     */
    int SC_ACCEPTED = 202;

    /**
     * Status code (204) indicating the server has fulfilled the request but does not need to return an entity-body, and might
     * want to return updated metainformation. The response MAY include new or updated metainformation in the form of
     * entity-headers, which if present SHOULD be associated with the requested variant.
     */
    int SC_NO_CONTENT = 204;

    /**
     * Status code (304) indicating if the client has performed a conditional RETRIEVE request and access is allowed, but the
     * document has not been modified, the server SHOULD respond with this status code. The 304 response MUST NOT contain a
     * message-body.
     */
    int SC_NOT_MODIFIED = 304;

    /**
     * Status code (400) indicating the request could not be understood by the server due to malformed syntax. The client SHOULD
     * NOT repeat the request without modifications.
     */
    int SC_BAD_REQUEST = 400;

    /**
     * Status code (401) indicating the requestEntity in not authorized to perform the request.
     */
    int SC_PERMISSION_DENIED = 401;

    /**
     * Status code (403) indicating the server understood the request, but is refusing to fulfill it.
     */
    int SC_FORBIDDEN = 403;

    /**
     * Status code (404) indicating the server has not found anything matching the Request-URI. No indication is given of
     * whether the condition is temporary or permanent. The 410 (Deleted) status code SHOULD be used if the server knows,
     * through some internally configurable mechanism, that an old resource is permanently unavailable and has no forwarding
     * address. This status code is commonly used when the server does not wish to reveal exactly why the request has been
     * refused, or when no other response is applicable.
     */
    int SC_NOT_FOUND = 404;

    /**
     * Status code (405) indicating the method specified in the Request-Line is not allowed for the resource identified by the
     * Request-URI. The response MUST include an Allow header containing a list of valid methods for the requested resource.
     */
    int SC_METHOD_NOT_ALLOWED = 405;

    /**
     * Status code (406) indicating the resource identified by the request is only capable of generating response entities which
     * have content characteristics not acceptable according to the accept headers sent in the request.
     */
    int SC_NOT_ACCEPTABLE = 406;

    /**
     * Status code (408) indicating the client did not produce a request within the time that the server was prepared to wait.
     * The client MAY repeat the request without modifications at any later time.
     */
    int SC_REQUEST_TIMEOUT = 408;

    /**
     * Status code (409) indicating that the request could not be completed due to a conflict with the current state of the
     * resource. This code is only allowed in situations where it is expected that the user might be able to resolve the
     * conflict and resubmit the request.
     */
    int SC_CONFLICT = 409;

    /**
     * Status code (410) indicating the requested resource is no longer available at the server and no forwarding address is
     * known. This condition is expected to be considered permanent. Clients with link editing capabilities SHOULD delete
     * references to the Request-URI after user approval. If the server does not know, or has no facility to determine, whether
     * or not the condition is permanent, the status code 404 (Not Found) SHOULD be used instead.
     */
    int SC_DELETED = 410;

    /**
     * Status code (412) indicating the precondition given in one or more of the request-header fields evaluated to false when
     * it was tested on the server. This response code allows the client to place preconditions on the current resource
     * metainformation (header field data) and thus prevent the requested method from being applied to a resource other than the
     * one intended.
     */
    int SC_PRECONDITION_FAILED = 412;

    /**
     * Status code (415) indicating the server is refusing to service the request because the entity of the request is in a
     * format not supported by the requested resource for the requested method.
     */
    int SC_UNSUPPORTED_MEDIA_TYPE = 415;

    /**
     * Status code (500) indicating the server encountered an unexpected condition that prevented it from fulfilling the
     * request.
     */
    int SC_INTERNAL_SERVER_ERROR = 500;

    /**
     * Status code (501) indicating the server does not support the functionality required to fulfill the request.
     */
    int SC_NOT_IMPLEMENTED = 501;

    /**
     * Status code (502) indicating the server, while acting as a gateway or proxy, received an invalid response from the
     * upstream server it accessed in attempting to fulfill the request.
     */
    int SC_BAD_GATEWAY = 502;

    /**
     * Status code (503) indicating the server is currently unable to handle the request due to a temporary overloading or
     * maintenance of the server. The implication is that this is a temporary condition which will be alleviated after some
     * delay. If known, the length of the delay MAY be indicated in a Retry-After header. If no Retry-After is given, the client
     * SHOULD handle the response as it would for a 500 response.
     */
    int SC_SERVICE_UNAVAILABLE = 503;

    /**
     * Status code (504) indicating the server, while acting as a gateway or proxy, did not receive a timely response from the
     * upstream server specified by the URI (e.g. HTTP, FTP, LDAP) or some other auxiliary server (e.g. DNS) it needed to access
     * in attempting to complete the request.
     */
    int SC_GATEWAY_TIMEOUT = 504;

    /**
     * Reason phrase for status code (200) indicating the request succeeded normally.
     */
    String RP_OK = "OK";

    /**
     * Reason phrase for status code (201) indicating the request has been fulfilled and resulted in a new resource being
     * created. The newly created resource can be referenced by the URI(s) returned in the entity of the response, with the most
     * specific URI for the resource given by a Content-Location header field. The entity format is specified by the media type
     * given in the Content-Type header field. The origin server MUST create the resource before returning the 201 status code.
     * If the action cannot be carried out immediately, the server SHOULD respond with 202 (Accepted) response instead.
     */
    String RP_CREATED = "Created";

    /**
     * Reason phrase for status code (202) indicating that the request has been accepted for processing, but the processing has
     * not been completed. The request will be responded later with a final response using semi-asynchronous mechanism.
     */
    String RP_ACCEPTED = "Accepted";

    /**
     * Reason phrase for status code (204) indicating the server has fulfilled the request but does not need to return an
     * entity-body, and might want to return updated metainformation. The response MAY include new or updated metainformation in
     * the form of entity-headers, which if present SHOULD be associated with the requested variant.
     */
    String RP_NO_CONTENT = "No Content";

    /**
     * Reason phrase for status code (304) indicating if the client has performed a conditional RETRIEVE request and access is
     * allowed, but the document has not been modified, the server SHOULD respond with this status code. The 304 response MUST
     * NOT contain a message-body.
     */
    String RP_NOT_MODIFIED = "Not Modified";

    /**
     * Reason phrase for status code (400) indicating the request could not be understood by the server due to malformed syntax.
     * The client SHOULD NOT repeat the request without modifications.
     */
    String RP_BAD_REQUEST = "Bad Request";

    /**
     * Reason phrase for status code (401) indicating the requestEntity in not authorized to perform the request.
     */
    String RP_PERMISSION_DENIED = "Permission Denied";

    /**
     * Reason phrase for status code (403) indicating the server understood the request, but is refusing to fulfill it.
     */
    String RP_FORBIDDEN = "Forbidden";

    /**
     * Reason phrase for status code (404) indicating the server has not found anything matching the Request-URI. No indication
     * is given of whether the condition is temporary or permanent. The 410 (Deleted) status code SHOULD be used if the server
     * knows, through some internally configurable mechanism, that an old resource is permanently unavailable and has no
     * forwarding address. This status code is commonly used when the server does not wish to reveal exactly why the request has
     * been refused, or when no other response is applicable.
     */
    String RP_NOT_FOUND = "Not Found";

    /**
     * Reason phrase for status code (405) indicating the method specified in the Request-Line is not allowed for the resource
     * identified by the Request-URI. The response MUST include an Allow header containing a list of valid methods for the
     * requested resource.
     */
    String RP_METHOD_NOT_ALLOWED = "Method Not Allowed";

    /**
     * Reason phrase for status code (406) indicating the resource identified by the request is only capable of generating
     * response entities which have content characteristics not acceptable according to the accept headers sent in the request.
     */
    String RP_NOT_ACCEPTABLE = "Not Acceptable";

    /**
     * Reason phrase for status code (408) indicating the client did not produce a request within the time that the server was
     * prepared to wait. The client MAY repeat the request without modifications at any later time.
     */
    String RP_REQUEST_TIMEOUT = "Request Timeout";

    /**
     * Reason phrase for status code (409) indicating that the request could not be completed due to a conflict with the current
     * state of the resource. This code is only allowed in situations where it is expected that the user might be able to
     * resolve the conflict and resubmit the request.
     */
    String RP_CONFLICT = "Conflict";

    /**
     * Reason phrase for status code (410) indicating the requested resource is no longer available at the server and no
     * forwarding address is known. This condition is expected to be considered permanent. Clients with link editing
     * capabilities SHOULD delete references to the Request-URI after user approval. If the server does not know, or has no
     * facility to determine, whether or not the condition is permanent, the status code 404 (Not Found) SHOULD be used instead.
     */
    String RP_DELETED = "Deleted";

    /**
     * Reason phrase for status code (412) indicating the precondition given in one or more of the request-header fields
     * evaluated to false when it was tested on the server. This response code allows the client to place preconditions on the
     * current resource metainformation (header field data) and thus prevent the requested method from being applied to a
     * resource other than the one intended.
     */
    String RP_PRECONDITION_FAILED = "Precondition Failed";

    /**
     * Reason phrase for status code (415) indicating the server is refusing to service the request because the entity of the
     * request is in a format not supported by the requested resource for the requested method.
     */
    String RP_UNSUPPORTED_MEDIA_TYPE = "Unsupported Media Type";

    /**
     * Reason phrase for status code (500) indicating the server encountered an unexpected condition that prevented it from
     * fulfilling the request.
     */
    String RP_INTERNAL_SERVER_ERROR = "Internal Server Error";

    /**
     * Reason phrase for status code (501) indicating the server does not support the functionality required to fulfill the
     * request.
     */
    String RP_NOT_IMPLEMENTED = "Not Implemented";

    /**
     * Reason phrase for status code (502) indicating the server, while acting as a gateway or proxy, received an invalid
     * response from the upstream server it accessed in attempting to fulfill the request.
     */
    String RP_BAD_GATEWAY = "Bad Gateway";

    /**
     * Reason phrase for status code (503) indicating the server is currently unable to handle the request due to a temporary
     * overloading or maintenance of the server. The implication is that this is a temporary condition which will be alleviated
     * after some delay. If known, the length of the delay MAY be indicated in a Retry-After header. If no Retry-After is given,
     * the client SHOULD handle the response as it would for a 500 response.
     */
    String RP_SERVICE_UNAVAILABLE = "Service Unavailable";

    /**
     * Reason phrase for status code (504) indicating the server, while acting as a gateway or proxy, did not receive a timely
     * response from the upstream server specified by the URI (e.g. HTTP, FTP, LDAP) or some other auxiliary server (e.g. DNS)
     * it needed to access in attempting to complete the request.
     */
    String RP_GATEWAY_TIMEOUT = "Gateway Timeout";

    /**
     * Always returns null. SONG is not a content transfer protocol and having stream based content accessors is of little
     * utility. Message content can be set using the SONGServletMessage.setContent(java.lang.Object, java.lang.String) method.
     *
     * @return null
     */
    javax.servlet.ServletOutputStream getOutputStream() throws java.io.IOException;

    /**
     * Returns the request associated with this response.
     *
     * @return The request corresponding the current response
     */
    SongServletRequest getRequest();

    /**
     * Returns the status code of this response object. The Status-Code is a 3-digit integer result code that indicates the
     * outcome of the attempt to understand and satisfy the request.
     *
     * @return The status code of the response
     */
    int getStatus();

    /**
     * Gets the reason phrase for this response object.
     *
     * @return The reason phrase for this response object
     */
    String getReasonPhrase();

    /**
     * Always returns null. SONG is not a content transfer protocol and having stream based content accessors is of little
     * utility. Message content can be set using the SongServletMessage.setContent(java.lang.Object, java.lang.String) method.
     *
     * @return null
     */
    java.io.PrintWriter getWriter() throws java.io.IOException;

    /**
     * Sets the status code of this response object.
     *
     * @param statusCode The status code to set
     */
    void setStatus(int statusCode);
}
