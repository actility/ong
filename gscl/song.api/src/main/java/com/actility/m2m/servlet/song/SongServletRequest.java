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
 * id $Id: SongServletRequest.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import javax.servlet.ServletRequest;

/**
 * Represents SONG request messages. When receiving an incoming SONG request the container creates a SongServletRequest and
 * passes it to the handling servlet. For outgoing, locally initiated requests, applications call
 * {@link SongFactory} to create a SongServletRequest that can then be modified and sent.
 */
public interface SongServletRequest extends SongServletMessage, ServletRequest {
    /**
     * Method used in SONG to retrieve a resource
     */
    String MD_RETRIEVE = "RETRIEVE";

    /**
     * Method used in SONG to update a resource
     */
    String MD_UPDATE = "UPDATE";

    /**
     * Method used in SONG to create a sub-resource on a collection resource
     */
    String MD_CREATE = "CREATE";

    /**
     * Method used in SONG to delete a resource
     */
    String MD_DELETE = "DELETE";

    /**
     * Creates a response for this request with the specifies status code.
     *
     * @param statuscode The status code of the response
     * @return The created response
     */
    SongServletResponse createResponse(int statuscode);

    /**
     * Creates a response for this request with the specifies status code and reason phrase.
     *
     * @param statusCode The status code of the response
     * @param reasonPhrase The reason phrase of the response
     * @return The created response
     */
    SongServletResponse createResponse(int statusCode, java.lang.String reasonPhrase);

    /**
     * Always returns null. SONG is not a content transfer protocol and having stream based content accessors is of little
     * utility. Message content can be retrieved using {@link SongServletMessage#getContent()} and
     * {@link SongServletMessage#getRawContent()}.
     */
    javax.servlet.ServletInputStream getInputStream() throws java.io.IOException;

    /**
     * Always returns null. SONG is not a content transfer protocol and having stream based content accessors is of little
     * utility. Message content can be retrieved using {@link SongServletMessage#getContent()} and
     * {@link SongServletMessage#getRawContent()}.
     */
    java.io.BufferedReader getReader() throws java.io.IOException;

    /**
     * Checks whether the current SongServletRequest is proxy request.
     *
     * @return Whether the current SongServletRequest is proxy request
     */
    boolean isProxy();

    /**
     * Sends this SongServletRequest and wait in worst case until the given time.
     *
     * @param millis Time to wait until considering that the transaction is expired. In that case, the request is answered with
     *            a 504 Gateway Timeout
     * @throws java.io.IOException If an IO problem occurs
     */
    void send(long millis) throws java.io.IOException;

    /**
     * Sends this SongServletRequest to the given point of contact.
     *
     * @param poc The point of contact to which the request must be sent
     * @throws java.io.IOException If an IO problem occurs
     */
    void send(SongURI poc) throws java.io.IOException;

    /**
     * Sends this SongServletRequest to the given point of contact and wait in worst case until the given time.
     *
     * @param millis Time to wait until considering that the transaction is expired. In that case, the request is answered with
     *            a 504 Gateway Timeout
     * @param poc The point of contact to which the request must be sent
     * @throws java.io.IOException If an IO problem occurs
     */
    void send(long millis, SongURI poc) throws java.io.IOException;

    /**
     * Gets the Proxy object related to the current request. This object allows to proxy a request to a remote server.
     *
     * @return The proxy object related to the current request
     */
    Proxy getProxy();

    /**
     * Gets the portion of the request URI that indicates the context of the request. The context path always comes first in a
     * request URI. The path starts with a '/' character but does not end with a '/' character. For servlets in the default
     * (root) context, this method returns ''.
     *
     * @return a <code>String</code> specifying the portion of the request URI that indicates the context of the request
     */
    String getContextPath();

    /**
     * Gets the path section that directly corresponds to the mapping which activated this request. This path starts with a '/'
     * character except in the case where the request matches the root servlet, in which case it is an empty string.
     *
     * @return The servlet path of the request
     */
    String getServletPath();

    /**
     * Gets the part of the request path that is not part of the Context Path or the Servlet Path. It is either null if there is
     * no extra path, or is a string with a leading ‘/’.
     *
     * @return The extra path after the servlet path that was matched
     */
    String getPathInfo();
}
