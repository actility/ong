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
 * id $Id: SongBindingFacade.java 9044 2014-07-03 15:50:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 9044 $
 * lastrevision $Date: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 */

package com.actility.m2m.servlet.song.binding;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.song.ChannelClientListener;
import com.actility.m2m.servlet.song.ChannelServerListener;
import com.actility.m2m.servlet.song.LongPollingURIs;
import com.actility.m2m.servlet.song.SongURI;

/**
 * Interface that a SONG binding MUST provide to the SONG container in order to manage messages.
 */
public interface SongBindingFacade {

    /**
     * Creates a server &lt;notificationChannel&gt; connection with the given URI.
     * <p>
     * The method is in charge to generate a contact and long polling URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long polling connection and which allows to build the
     *            {@link LongPollingURIs}
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @return The contact and long polling URI of the long polling connection
     */
    LongPollingURIs createServerNotificationChannel(SongURI serverURI, ChannelServerListener listener);

    /**
     * Creates a server &lt;notificationChannel&gt; connection with the given contact and long polling URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long polling connection
     * @param longPollingURI The long polling URI to use in the long polling connection
     * @param listener A listener that will receive events related to the &lt;notificationChannel&gt;
     * @throws ServletException If the binding cannot manage the given contact and long polling URIs
     */
    void createServerNotificationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException;

    /**
     * Deletes a server &lt;notificationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteServerNotificationChannel(SongURI contactURI, SongURI longPollingURI);

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
     * @throws ServletException If a long polling connection if already opened for the given contact and long polling URIs
     */
    void createClientNotificationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            SongURI relatedRequestingEntity, SongURI relatedTargetID, ChannelClientListener listener) throws ServletException;

    /**
     * Deletes a client &lt;notificationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteClientNotificationChannel(SongURI contactURI, SongURI longPollingURI);

    /**
     * Creates a server &lt;communicationChannel&gt; connection with the given URI.
     * <p>
     * The method is in charge to generate a contact and long polling URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long polling connection and which allows to build the
     *            {@link LongPollingURIs}
     * @param listener A listener that will receive events related to the &lt;communicationChannel&gt;
     * @return The contact and long polling URI of the long polling connection
     */
    LongPollingURIs createServerCommunicationChannel(SongURI serverURI, ChannelServerListener listener);

    /**
     * Creates a server &lt;communicationChannel&gt; connection with the given contact and long polling URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long polling connection
     * @param longPollingURI The long polling URI to use in the long polling connection
     * @param listener A listener that will receive events related to the &lt;communicationChannel&gt;
     * @throws ServletException If the binding cannot manage the given contact and long polling URIs
     */
    void createServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException;

    /**
     * Deletes a server &lt;comunicationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI);

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
     * @throws ServletException If a long polling connection if already opened for the given contact and long polling URIs
     */
    void createClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            ChannelClientListener listener) throws ServletException;

    /**
     * Deletes a client &lt;communicationChannel&gt; connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    void deleteClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI);
}
