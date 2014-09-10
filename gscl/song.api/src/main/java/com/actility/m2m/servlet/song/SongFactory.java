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
 * id $Id: SongFactory.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.ApplicationSession;

/**
 * Factory interface for a variety of SONG Servlet API abstractions.
 * <p>
 * SONG servlet containers are required to make a SongFactory instance available to applications through a ServletContext
 * attribute with name com.actility.servlet.song.SongFactory.
 */
public interface SongFactory {

    /**
     * Returns a new ApplicationSession. This is useful, for example, when an application is being initialized and wishes to
     * perform some signaling action.
     *
     * @return The created ApplicationSession
     */
    ApplicationSession createApplicationSession();

    /**
     * Returns a new request object which is a copy of the given request with the specified requestingEntity, and targetID. This
     * method is used by servlets acting as SONG clients in order to send a request. This method makes a copy of the
     * requestingEntity and targetID arguments and associates them with the new SongServletRequest.
     *
     * @param request The request to copy in the new request
     * @param requestingEntity The new requestingEntity URI of the request
     * @param targetID The new targetID URI of the request
     * @return The created request
     * @throws IOException If a problem occurs while setting the content of the created request
     */
    SongServletRequest createRequestFrom(SongServletRequest request, SongURI requestingEntity, SongURI targetID)
            throws IOException;

    /**
     * Returns a new request object which is a copy of the given request with the specified requestingEntity, and targetID. This
     * method is used by servlets acting as SONG clients in order to send a request.
     *
     * @param request The request to copy in the new request
     * @param requestingEntity The new requestingEntity URI of the request
     * @param targetID The new targetID URI of the request
     * @return The created request
     * @throws IOException If a problem occurs while setting the content of the created request
     */
    SongServletRequest createRequestFrom(SongServletRequest request, URI requestingEntity, URI targetID) throws IOException;

    /**
     * Returns a new request object which is a copy of the given request with the specified requestingEntity, and targetID. This
     * method is used by servlets acting as SONG clients in order to send a request.
     *
     * @param request The request to copy in the new request
     * @param requestingEntity The new requestingEntity URI of the request
     * @param targetID The new targetID URI of the request
     * @return The created request
     * @throws URISyntaxException If one of the given URIs is invalid
     * @throws IOException If a problem occurs while setting the content of the created request
     */
    SongServletRequest createRequestFrom(SongServletRequest request, String requestingEntity, String targetID)
            throws URISyntaxException, IOException;

    /**
     * Returns a new response object which is a copy of the given response with the specified statusCode. This
     * method is used by servlets acting as SONG servers in order to send a response.
     *
     * @param request The request from which the response is created
     * @param response The response to copy in the new response
     * @param statusCode The new statusCode of the response
     * @return The created response
     * @throws IOException If a problem occurs while setting the content of the created request
     */
    SongServletResponse createResponseFrom(SongServletRequest request, SongServletResponse response, int statusCode)
            throws IOException;

    /**
     * Returns a new response object which is a copy of the given response with the specified statusCode. This
     * method is used by servlets acting as SONG servers in order to send a response.
     *
     * @param request The request from which the response is created
     * @param response The response to copy in the new response
     * @param statusCode The new statusCode of the response
     * @param reasonPhrase The new reason phrase of the response
     * @return The created response
     * @throws IOException If a problem occurs while setting the content of the created request
     */
    SongServletResponse createResponseFrom(SongServletRequest request, SongServletResponse response, int statusCode,
            String reasonPhrase) throws IOException;

    /**
     * Returns a new request object with the specified request method, requestingEntity, and targetID. This method is used by
     * servlets acting as SONG clients in order to send a request. This method makes a copy of the requestingEntity and targetID
     * arguments and associates them with the new SongServletRequest.
     *
     * @param method The method of the request, e.g. "RETRIEVE", "UPDATE" ...
     * @param requestingEntity The requestingEntity URI of the request
     * @param targetID The targetID URI of the request
     * @return The created request
     */
    SongServletRequest createRequest(String method, SongURI requestingEntity, SongURI targetID);

    /**
     * Returns a new request object which is a copy of the given request with the specified requestingEntity, and targetID. This
     * method is used by servlets acting as SONG clients in order to send a request.
     *
     * @param method The method of the request, e.g. "RETRIEVE", "UPDATE" ...
     * @param requestingEntity The requestingEntity URI of the request
     * @param targetID The targetID URI of the request
     * @return The created request
     */
    SongServletRequest createRequest(String method, URI requestingEntity, URI targetID);

    /**
     * Returns a new request object which is a copy of the given request with the specified requestingEntity, and targetID. This
     * method is used by servlets acting as SONG clients in order to send a request.
     *
     * @param method The method of the request, e.g. "RETRIEVE", "UPDATE" ...
     * @param requestingEntity The requestingEntity URI of the request
     * @param targetID The targetID URI of the request
     * @return The created request
     * @throws URISyntaxException If one of the given URIs is invalid
     */
    SongServletRequest createRequest(String method, String requestingEntity, String targetID) throws URISyntaxException;

    /**
     * Returns a new request object with the specified request method, requestingEntity, and targetID. The returned request
     * object exists in the specified ApplicationSession. This method is used by servlets acting as SONG clients in order to
     * send a request. This method makes a copy of the requestingEntity and targetID arguments and associates them with the new
     * SongServletRequest.
     *
     * @param appSession The application to which the SongServletRequest belongs
     * @param method The method of the request, e.g. "RETRIEVE", "UPDATE" ...
     * @param requestingEntity The requestingEntity URI of the request
     * @param targetID The targetID URI of the request
     * @return The created request
     */
    SongServletRequest createRequest(ApplicationSession appSession, String method, SongURI requestingEntity, SongURI targetID);

    /**
     * Returns a new request object with the specified request method, requestingEntity, and targetID. The returned request
     * object exists in the specified ApplicationSession. This method is used by servlets acting as SONG clients in order to
     * send a request.
     *
     * @param appSession The application to which the SongServletRequest belongs
     * @param method The method of the request, e.g. "RETRIEVE", "UPDATE" ...
     * @param requestingEntity The requestingEntity URI of the request
     * @param targetID The targetID URI of the request
     * @return The created request
     */
    SongServletRequest createRequest(ApplicationSession appSession, String method, URI requestingEntity, URI targetID);

    /**
     * Returns a new request object with the specified request method, requestingEntity, and targetID. The returned request
     * object exists in the specified ApplicationSession. This method is used by servlets acting as SONG clients in order to
     * send a request. This method makes a copy of the requestingEntity and targetID arguments and associates them with the new
     * SongServletRequest.
     *
     * @param appSession The application to which the SongServletRequest belongs
     * @param method The method of the request, e.g. "RETRIEVE", "UPDATE" ...
     * @param requestingEntity The requestingEntity URI of the request
     * @param targetID The targetID URI of the request
     * @return The created request
     * @throws URISyntaxException If one of the given URIs is invalid
     */
    SongServletRequest createRequest(ApplicationSession appSession, String method, String requestingEntity, String targetID)
            throws URISyntaxException;

    /**
     * Returns a URI object corresponding to the specified string. The URI may then be used as a requestingEntity or targetID
     * URI in SONG requests. Implementations must be able to represent URIs of any scheme. If the specified URI string contains
     * any reserved characters, then they must be escaped.
     *
     * @param uri The string URI to parse
     * @return The created URI
     * @throws URISyntaxException If the given URI is malformed
     */
    SongURI createURI(String uri) throws URISyntaxException;

    /**
     * Returns a URI object corresponding to the specified one. The URI may then be used as a requestingEntity or targetID URI
     * in SONG requests. Implementations must be able to represent URIs of any scheme. If the specified URI string contains any
     * reserved characters, then they must be escaped.
     *
     * @param uri The URI to use to build the Song URI
     * @return The created URI
     */
    SongURI createURI(URI uri);

    /**
     * Returns a URI object to the given scheme, hostname, port and path. The URI may then be used as a requestingEntity or
     * targetID URI in SONG requests. Implementations must be able to represent URIs of any scheme. If the specified URI string
     * contains any reserved characters, then they must be escaped.
     *
     * @param scheme The scheme of the URI
     * @param hostname The hostname of the URI
     * @param port The port of the URI
     * @param path The path of the URI
     * @return The created URI
     */
    SongURI createURI(String scheme, String hostname, int port, String path);

    /**
     * Builds a URI which could be used to access the current SONG application from the given URI.
     * <p>
     * This method takes into consideration the scheme of the given URI and the binding server connection available for it
     *
     * @param reference The URI the must be used to compute the current SONG application URI.
     * @param path An optional path appended to the end of the generated URI
     * @return The built SONG URI for the current SONG application
     * @throws URISyntaxException If the given URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URI scheme
     */
    SongURI createLocalURIFrom(String reference, String path) throws URISyntaxException, ServletException;

    /**
     * Builds a URI which could be used to access the current SONG application from the given URI.
     * <p>
     * This method takes into consideration the scheme of the given URI and the binding server connection available for it
     *
     * @param reference The URI the must be used to compute the current SONG application URI.
     * @param path An optional path appended to the end of the generated URI
     * @return The built SONG URI for the current SONG application
     * @throws ServletException If no binding exists for the given URI scheme
     */
    SongURI createLocalURIFrom(URI reference, String path) throws ServletException;

    /**
     * Builds a URI which could be used to access the current SONG application from the given URI.
     * <p>
     * This method takes into consideration the scheme of the given URI and the binding server connection available for it
     *
     * @param reference The URI the must be used to compute the current SONG application URI.
     * @param path An optional path appended to the end of the generated URI
     * @return The built SONG URI for the current SONG application
     * @throws ServletException If no binding exists for the given URI scheme
     */
    SongURI createLocalURIFrom(SongURI reference, String path) throws ServletException;
}
