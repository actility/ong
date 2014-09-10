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
 * id $Id: LongPollURIs.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

/**
 * Util class container to gather the contact and long poll URIs used during the long poll connection creation.
 * <p>
 * The contact URI is a public URI that could be used to contact the long poll connection while the long poll URI is an internal
 * URI used between the client and server.
 *
 */
public class LongPollURIs {
    private SongURI contactURI;
    private SongURI longPollURI;

    /**
     * Builds a {@link LongPollURIs} from the given contact and long poll URI
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    public LongPollURIs(SongURI contactURI, SongURI longPollURI) {
        this.contactURI = contactURI;
        this.longPollURI = longPollURI;
    }

    /**
     * Gets the contact URI of the long poll connection.
     *
     * @return The contact URI
     */
    public SongURI getContactURI() {
        return contactURI;
    }

    /**
     * Gets the long poll URI of the long poll connection.
     *
     * @return The long poll URI
     */
    public SongURI getLongPollURI() {
        return longPollURI;
    }
}
