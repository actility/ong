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
 * id $Id: SongBindingFactory.java 9044 2014-07-03 15:50:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 9044 $
 * lastrevision $Date: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 */

package com.actility.m2m.servlet.song.binding;

import java.net.InetAddress;

import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;

/**
 * A {@link SongFactory} for a SONG binding.
 * <p>
 * Defines specific methods to inject SONG messages from other protocols.
 */
public interface SongBindingFactory extends SongFactory {
    /**
     * Attribute to set the long poll URI of a SONG request, this request requires to be resolved in the external aliases route
     * table.
     * @deprecated
     */
    String AT_LONG_POLL_URI = "com.actility.servlet.song.LongPollURI";

    /**
     * Attribute to get the point of contact URI. When present this is the URI to which the SONG request must be sent on the IP
     * network even if the targetID is still the requested URI. This is in case the targetID is not routable.
     */
    String AT_POC_URI = "com.actility.servlet.song.PocURI";

    /**
     * Attribute to get the expiration time of a request to send over the network.
     */
    String AT_EXPIRATION_TIME = "com.actility.servlet.song.ExpirationTime";

    /**
     * Creates SONG request from the given data.
     *
     * @param protocol The SONG request protocol
     * @param method The SONG request method
     * @param requestingEntity The SONG request requestingEntity
     * @param targetID The SONG request targetID
     * @param localAddress The destination IP address of the connection on which the request was received
     * @param localPort The SONG request localPort which is port of the server that received the request
     * @param remoteAddress The source IP address of the connection on which the request was received
     * @param remotePort The SONG request remotePort which is port of the client that sent the request
     * @param proxyRequest Whether this is a proxu request
     * @return The built SONG request
     */
    SongServletRequest createRequest(String protocol, String method, SongURI requestingEntity, SongURI targetID,
            InetAddress localAddress, int localPort, InetAddress remoteAddress, int remotePort, boolean proxyRequest);

    /**
     * Creates SONG request from the given data.
     *
     * @param protocol The SONG request protocol
     * @param method The SONG request method
     * @param requestingEntity The SONG request requestingEntity
     * @param targetID The SONG request targetID
     * @param localAddress The destination IP address of the connection on which the request was received
     * @param localPort The SONG request localPort which is port of the server that received the request
     * @param remoteAddress The source IP address of the connection on which the request was received
     * @param remotePort The SONG request remotePort which is port of the client that sent the request
     * @param proxyRequest Whether this is a proxu request
     * @param id associated to the request
     * @return The built SONG request
     */
    SongServletRequest createRequest(String protocol, String method, SongURI requestingEntity, SongURI targetID,
            InetAddress localAddress, int localPort, InetAddress remoteAddress, int remotePort, boolean proxyRequest, String id);

    /**
     * Creates SONG response from the given data.
     *
     * @param request The corresponding SONG request
     * @param statusCode The SONG response status code
     * @param reasonPhrase The SONG response reason phrase
     * @param localAddress The destination IP address of the connection on which the response was received
     * @param localPort The SONG response localPort which is port of the client that received the response
     * @param remoteAddress The source IP address of the connection on which the response was received
     * @param remotePort The SONG response remotePort which is port of the server that sent the response
     * @return The built SONG response
     */
    SongServletResponse createResponse(SongServletRequest request, int statusCode, String reasonPhrase,
            InetAddress localAddress, int localPort, InetAddress remoteAddress, int remotePort);
}
