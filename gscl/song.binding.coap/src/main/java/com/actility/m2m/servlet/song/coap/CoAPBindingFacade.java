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
 * id $Id: CoAPBindingFacade.java 9044 2014-07-03 15:50:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 9044 $
 * lastrevision $Date: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 */

package com.actility.m2m.servlet.song.coap;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.song.ChannelClientListener;
import com.actility.m2m.servlet.song.ChannelServerListener;
import com.actility.m2m.servlet.song.LongPollingURIs;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.binding.SongBindingFacade;

/**
 * Implementation for this binding of the {@link SongBindingFacade} needed to register a SONG binding in the SONG container.
 * <p>
 * It allows the interaction between the SONG container and the CoAP binding.
 *
 */
public final class CoAPBindingFacade implements SongBindingFacade {

    public LongPollingURIs createServerNotificationChannel(SongURI serverURI, ChannelServerListener listener) {
        throw new UnsupportedOperationException();
    }

    public void createServerNotificationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException {
        throw new UnsupportedOperationException();
    }

    public void deleteServerNotificationChannel(SongURI contactURI, SongURI longPollingURI) {
        throw new UnsupportedOperationException();
    }

    public void createClientNotificationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            SongURI relatedRequestingEntity, SongURI relatedTargetID, ChannelClientListener listener) throws ServletException {
        throw new UnsupportedOperationException();
    }

    public void deleteClientNotificationChannel(SongURI contactURI, SongURI longPollingURI) {
        throw new UnsupportedOperationException();
    }

    public LongPollingURIs createServerCommunicationChannel(SongURI serverURI, ChannelServerListener listener) {
        throw new UnsupportedOperationException();
    }

    public void createServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException {
        throw new UnsupportedOperationException();
    }

    public void deleteServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI) {
        throw new UnsupportedOperationException();
    }

    public void createClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            ChannelClientListener listener) throws ServletException {
        throw new UnsupportedOperationException();
    }

    public void deleteClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI) {
        throw new UnsupportedOperationException();
    }

}
