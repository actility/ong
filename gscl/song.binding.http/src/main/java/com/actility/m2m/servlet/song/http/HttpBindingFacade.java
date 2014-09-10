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
 * id $Id: HttpBindingFacade.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.servlet.song.http;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.song.LongPollURIs;
import com.actility.m2m.servlet.song.SongBindingFacade;
import com.actility.m2m.servlet.song.SongURI;


/**
 * Implementation for this binding of the {@link SongBindingFacade} needed to register a SONG binding in the SONG container.
 * <p>
 * It allows the interaction between the SONG container and the HTTP binding.
 *
 */
public final class HttpBindingFacade implements SongBindingFacade {

    private final SongHttpBinding songHttpBinding;

    /**
     * Builds the Song binding facade for HTTP.
     *
     * @param songHttpBinding The {@link SongHttpBinding} which will manage some of the operations of the song binding facade
     */
    public HttpBindingFacade(SongHttpBinding songHttpBinding) {
        this.songHttpBinding = songHttpBinding;
    }

    public LongPollURIs createServerLongPoll(SongURI serverURI) {
        return songHttpBinding.createServerLongPolling(serverURI);
    }

    public void createServerLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException {
        songHttpBinding.createServerLongPolling(contactURI, longPollURI);
    }

    public void deleteServerLongPoll(SongURI contactURI, SongURI longPollURI) {
        songHttpBinding.deleteServerLongPolling(contactURI, longPollURI);
    }

    public void createClientLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException {
        songHttpBinding.createClientLongPolling(contactURI, longPollURI);
    }

    public void deleteClientLongPoll(SongURI contactURI, SongURI longPollURI) {
        songHttpBinding.deleteClientLongPolling(contactURI, longPollURI);
    }

}
