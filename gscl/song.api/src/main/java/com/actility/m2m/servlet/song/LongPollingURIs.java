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
 * id $Id: LongPollingURIs.java 9044 2014-07-03 15:50:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 9044 $
 * lastrevision $Date: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 */

package com.actility.m2m.servlet.song;

/**
 * Util class container to gather the contact and long poll URIs used during the long poll connection creation.
 * <p>
 * The contact URI is a public URI that could be used to contact the long poll connection while the long poll URI is an internal
 * URI used between the client and server.
 *
 */
public class LongPollingURIs {
    private SongURI contactURI;
    private SongURI longPollURI;

    /**
     * Builds a {@link LongPollingURIs} from the given contact and long poll URI
     *
     * @param contactURI The contact URI of the long poll connection
     * @param longPollURI The long poll URI of the long poll connection
     */
    public LongPollingURIs(SongURI contactURI, SongURI longPollURI) {
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
    public SongURI getLongPollingURI() {
        return longPollURI;
    }
}
