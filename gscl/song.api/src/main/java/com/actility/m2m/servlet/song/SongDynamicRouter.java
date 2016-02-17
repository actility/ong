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
 * id $Id: SongDynamicRouter.java 9044 2014-07-03 15:50:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 9044 $
 * lastrevision $Date: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 */

package com.actility.m2m.servlet.song;

import java.net.URISyntaxException;
import java.util.Map;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.NamespaceException;

/**
 * Utility class that allows to dynamically add paths to a SONG application. This also allows to manage long polling connections
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
     * Creates a server &lt;notificationChannel&gt; connection with the given URI.
     * <p>
     * The method is in charge to generate a contact and long polling URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long polling connection and which allows to build the
     *            {@link LongPollingURIs}
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @return The contact and long polling URI of the long polling connection
     * @throws ServletException If no binding exists for the given URI scheme
     */
    LongPollingURIs createServerNotificationChannel(SongURI serverURI, ChannelServerListener listener) throws ServletException;

    /**
     * Creates a server &lt;notificationChannel&gt; connection on the given URI.
     * <p>
     * The method is in charge to generate a contact and long polling URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long polling connection and which allows to build the
     *            {@link LongPollingURIs}
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @return The contact and long polling URI of the long polling connection
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URI scheme
     */
    LongPollingURIs createServerNotificationChannel(String serverURI, ChannelServerListener listener)
            throws URISyntaxException, ServletException;

    /**
     * Creates a server &lt;notificationChannel&gt; connection with the given contact and long polling URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long polling connection
     * @param longPollingURI The long polling URI to use in the long polling connection
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @throws ServletException If no binding exists for the given URIs scheme or the container rejects the given URIs
     */
    void createServerNotificationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException;

    /**
     * Creates a server &lt;notificationChannel&gt; connection with the given contact and long polling URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long polling connection
     * @param longPollingURI The long polling URI to use in the long polling connection
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URIs scheme or the container rejects the given URIs
     */
    void createServerNotificationChannel(String contactURI, String longPollingURI, ChannelServerListener listener)
            throws URISyntaxException, ServletException;

    /**
     * Deletes a server &lt;notificationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteServerNotificationChannel(SongURI contactURI, SongURI longPollingURI);

    /**
     * Deletes a server &lt;notificationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteServerNotificationChannel(String contactURI, String longPollingURI);

    /**
     * Creates a client &lt;notificationChannel&gt; connection from the given contact and long polling URIs.
     * <p>
     * This will trigger the connection creation on the long polling URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     * @param requestingEntity The requesting entity to use for the &lt;notificationChannel&gt; requests
     * @param relatedRequestingEntity The requesting entity to use for &lt;notification&gt; received in the channel
     * @param relatedTargetID The target ID to use for &lt;notification&gt; received in the channel
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @throws ServletException If no binding exists for the given URIs scheme or if a long polling connection if already opened
     *             for the given contact and long polling URIs
     */
    void createClientNotificationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            SongURI relatedRequestingEntity, SongURI relatedTargetID, ChannelClientListener listener) throws ServletException;

    /**
     * Creates a client &lt;notificationChannel&gt; connection from the given contact and long polling URIs.
     * <p>
     * This will trigger the connection creation on the long polling URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     * @param requestingEntity The requesting entity to use for the &lt;notificationChannel&gt; requests
     * @param relatedRequestingEntity The requesting entity to use for &lt;notification&gt; received in the channel
     * @param relatedTargetID The target ID to use for &lt;notification&gt; received in the channel
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URIs scheme or if a long polling connection if already opened
     *             for the given contact and long polling URIs
     */
    void createClientNotificationChannel(String contactURI, String longPollingURI, String requestingEntity,
            String relatedRequestingEntity, String relatedTargetID, ChannelClientListener listener) throws URISyntaxException,
            ServletException;

    /**
     * Deletes a client &lt;notificationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteClientNotificationChannel(SongURI contactURI, SongURI longPollingURI);

    /**
     * Deletes a client &lt;notificationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteClientNotificationChannel(String contactURI, String longPollingURI);

    /**
     * Creates a server &lt;communicationChannel&gt; connection with the given URI.
     * <p>
     * The method is in charge to generate a contact and long polling URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long polling connection and which allows to build the
     *            {@link LongPollingURIs}
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @return The contact and long polling URI of the long polling connection
     * @throws ServletException If no binding exists for the given URI scheme
     */
    LongPollingURIs createServerCommunicationChannel(SongURI serverURI, ChannelServerListener listener) throws ServletException;

    /**
     * Creates a server &lt;communicationChannel&gt; connection on the given URI.
     * <p>
     * The method is in charge to generate a contact and long polling URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long polling connection and which allows to build the
     *            {@link LongPollingURIs}
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @return The contact and long polling URI of the long polling connection
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URI scheme
     */
    LongPollingURIs createServerCommunicationChannel(String serverURI, ChannelServerListener listener)
            throws URISyntaxException, ServletException;

    /**
     * Creates a server &lt;communicationChannel&gt; connection with the given contact and long polling URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long polling connection
     * @param longPollingURI The long polling URI to use in the long polling connection
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @throws ServletException If no binding exists for the given URIs scheme or the container rejects the given URIs
     */
    void createServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException;

    /**
     * Creates a server &lt;communicationChannel&gt; connection with the given contact and long polling URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long polling connection
     * @param longPollingURI The long polling URI to use in the long polling connection
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URIs scheme or the container rejects the given URIs
     */
    void createServerCommunicationChannel(String contactURI, String longPollingURI, ChannelServerListener listener)
            throws URISyntaxException, ServletException;

    /**
     * Deletes a server &lt;communicationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI);

    /**
     * Deletes a server &lt;communicationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteServerCommunicationChannel(String contactURI, String longPollingURI);

    /**
     * Creates a client &lt;communicationChannel&gt; connection from the given contact and long polling URIs.
     * <p>
     * This will trigger the connection creation on the long polling URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     * @param requestingEntity The requesting entity to use for the &lt;communicationChannel&gt; requests
     * @param listener A listener that will receive events related to the &lt;communicationChannel&gt;
     * @throws ServletException If no binding exists for the given URIs scheme or if a long polling connection if already opened
     *             for the given contact and long polling URIs
     */
    void createClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            ChannelClientListener listener) throws ServletException;

    /**
     * Creates a client &lt;communicationChannel&gt; connection from the given contact and long polling URIs.
     * <p>
     * This will trigger the connection creation on the long polling URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     * @param requestingEntity The requesting entity to use for the &lt;communicationChannel&gt; requests
     * @param listener A listener that will receive events related to the &lt;communicationChannel&gt;
     * @throws URISyntaxException If the given server URI has an incorrect syntax
     * @throws ServletException If no binding exists for the given URIs scheme or if a long polling connection if already opened
     *             for the given contact and long polling URIs
     */
    void createClientCommunicationChannel(String contactURI, String longPollingURI, String requestingEntity,
            ChannelClientListener listener) throws URISyntaxException, ServletException;

    /**
     * Deletes a client &lt;communicationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI);

    /**
     * Deletes a client &lt;communicationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteClientCommunicationChannel(String contactURI, String longPollingURI);

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
