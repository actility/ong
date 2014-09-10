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
 * id $Id: SongBindingFacade.java 6084 2013-10-15 13:54:23Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6084 $
 * lastrevision $Date: 2013-10-15 15:54:23 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:54:23 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import javax.servlet.ServletException;

/**
 * Interface that a SONG binding MUST provide to the SONG container in order to manage messages.
 */
public interface SongBindingFacade {

    /**
     * Creates a server long poll connection with the given URI.
     * <p>
     * The method is in charge to generate a contact and long poll URI for the created connection.
     * 
     * @param serverURI The URI that is the base to build the server long poll connection and which allows to build the
     *            {@link LongPollURIs}
     * @return The contact and long poll URI of the long poll connection
     */
    LongPollURIs createServerLongPoll(SongURI serverURI);

    /**
     * Creates a server long poll connection with the given contact and long poll URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     * 
     * @param contactURI The contact URI to use in the long poll connection
     * @param longPollURI The long poll URI to use in the long poll connection
     * @throws ServletException If the binding cannot manage the given contact and long poll URIs
     */
    void createServerLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException;

    /**
     * Deletes a server long poll connection given its contact and long poll URI.
     * 
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    void deleteServerLongPoll(SongURI contactURI, SongURI longPollURI);

    /**
     * Creates a client long poll connection from the given contact and long poll URIs.
     * <p>
     * This will trigger the connection creation on the long poll URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     * 
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     * @throws ServletException If a long poll connection if already opened for the given contact and long poll URIs
     */
    void createClientLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException;

    /**
     * Deletes a client long poll connection given its contact and long poll URI.
     * 
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    void deleteClientLongPoll(SongURI contactURI, SongURI longPollURI);
}
