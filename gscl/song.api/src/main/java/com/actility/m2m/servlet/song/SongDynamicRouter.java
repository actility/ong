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
 * id $Id: SongDynamicRouter.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import java.net.URISyntaxException;
import java.util.Map;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.NamespaceException;

/**
 * Utility class that allows to dynamically add paths to a SONG application. Thsi also allows to manage long poll connections
 * creation and deletion.
 * <p>
 * SONG servlet containers are required to make a SongDynamicRouter instance available to applications through a ServletContext
 * attribute with name com.actility.servlet.song.SongDynamicRouter.
 */
public interface SongDynamicRouter {
    /**
     * Adds dynamically a path to a SONG servlet.
     * <p>
     * The path must begin with '/' and must not end with slash '/' or contains any wildcarded '*' with the exception that a
     * path of the form '/' is used to denote the root path. The path is registered with an implicit wildcarded suffix, this
     * means all paths that have the same prefix as the one registered will be sent to the current Servlet. The matching process
     * gives a higher priority to the longest prefix found.
     *
     * @param path The path to add
     * @param configuration Configuration of the registration
     * @throws ServletException If a problem occurs during the adding of the path
     * @throws NamespaceException If the path is already used
     */
    void addPath(String path, Map configuration) throws ServletException, NamespaceException;

    /**
     * Adds a path associated to a SONG session to a SONG servlet.
     * <p>
     * Any request matching this path will be sent to the current Servlet and will embed the given Application session.
     * <p>
     * The path must begin with '/' and must not end with slash '/' or contains any wildcarded '*' with the exception that a
     * path of the form '/' is used to denote the root path. The path is registered with an implicit wildcarded suffix, this
     * means all paths that have the same prefix as the one registered will be sent to the current Servlet. The matching process
     * gives a higher priority to the longest prefix found.
     *
     * @param path The path to add
     * @param appSession The application session associated to the given path
     * @param configuration Configuration of the registration
     * @throws ServletException If a problem occurs during the adding of the path
     * @throws NamespaceException If the path is already used
     */
    void addPath(String path, ApplicationSession appSession, Map configuration) throws ServletException, NamespaceException;

    /**
     * Removes a path from a SONG servlet.
     *
     * @param path The path to remove
     */
    void removePath(String path);

    /**
     * Creates a server long poll connection with the given URI.
     * <p>
     * The method is in charge to generate a contact and long poll URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long poll connection and which allows to build the
     *            {@link LongPollURIs}
     * @return The contact and long poll URI of the long poll connection
     * @throws ServletException If no binding exists for the given URI scheme
     */
    LongPollURIs createServerLongPoll(SongURI serverURI) throws ServletException;

    /**
     * Creates a server long poll connection on the given URI.
     * <p>
     * The method is in charge to generate a contact and long poll URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long poll connection and which allows to build the
     *            {@link LongPollURIs}
     * @return The contact and long poll URI of the long poll connection
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URI scheme
     */
    LongPollURIs createServerLongPoll(String serverURI) throws URISyntaxException, ServletException;

    /**
     * Creates a server long poll connection with the given contact and long poll URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long poll connection
     * @param longPollURI The long poll URI to use in the long poll connection
     * @throws ServletException If no binding exists for the given URIs scheme or the container rejects the given URIs
     */
    void createServerLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException;

    /**
     * Creates a server long poll connection with the given contact and long poll URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long poll connection
     * @param longPollURI The long poll URI to use in the long poll connection
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URIs scheme or the container rejects the given URIs
     */
    void createServerLongPoll(String contactURI, String longPollURI) throws URISyntaxException, ServletException;

    /**
     * Deletes a server long poll connection given its contact and long poll URI.
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    void deleteServerLongPoll(SongURI contactURI, SongURI longPollURI);

    /**
     * Deletes a server long poll connection given its contact and long poll URI.
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    void deleteServerLongPoll(String contactURI, String longPollURI);

    /**
     * Creates a client long poll connection from the given contact and long poll URIs.
     * <p>
     * This will trigger the connection creation on the long poll URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     * @throws ServletException If no binding exists for the given URIs scheme or if a long poll connection if already opened
     *             for the given contact and long poll URIs
     */
    void createClientLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException;

    /**
     * Creates a client long poll connection from the given contact and long poll URIs.
     * <p>
     * This will trigger the connection creation on the long poll URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URIs scheme or if a long poll connection if already opened
     *             for the given contact and long poll URIs
     */
    void createClientLongPoll(String contactURI, String longPollURI) throws URISyntaxException, ServletException;

    /**
     * Deletes a client long poll connection given its contact and long poll URI.
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    void deleteClientLongPoll(SongURI contactURI, SongURI longPollURI);

    /**
     * Deletes a client long poll connection given its contact and long poll URI.
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    void deleteClientLongPoll(String contactURI, String longPollURI);

    /**
     * Check whether the current application can receive incoming connections from the given URI.
     * <p>
     * This takes into consideration the protocol used between the remote and local application which may support bi-directional
     * communication.
     *
     * @param targetedURI The URI to use to check whether the current application can be server
     * @return Whether the current application can be server from the given URI
     * @throws ServletException If no binding exists for the given URIs scheme
     */
    boolean canBeServerFrom(SongURI targetedURI) throws ServletException;

    /**
     * Check whether the current application can receive incoming connections from the given URI.
     * <p>
     * This takes into consideration the protocol used between the remote and local application which may support bi-directional
     * communication.
     *
     * @param targetedURI The URI to use to check whether the current application can be server
     * @return Whether the current application can be server from the given URI
     * @throws URISyntaxException If the given URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URIs scheme
     */
    boolean canBeServerFrom(String targetedURI) throws URISyntaxException, ServletException;
}
