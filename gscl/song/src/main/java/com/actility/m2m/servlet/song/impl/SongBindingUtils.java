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
 * id $Id: SongBindingUtils.java 9834 2014-10-31 16:26:14Z JReich $
 * author $Author: JReich $
 * version $Revision: 9834 $
 * lastrevision $Date: 2014-10-31 17:26:14 +0100 (Fri, 31 Oct 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-10-31 17:26:14 +0100 (Fri, 31 Oct 2014) $
 */

package com.actility.m2m.servlet.song.impl;

import java.net.InetAddress;

import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.binding.SongBindingFactory;
import com.actility.m2m.servlet.song.message.LocalWrappedRequest;
import com.actility.m2m.servlet.song.message.RemoteRequest;
import com.actility.m2m.servlet.song.node.BindingEndpointNode;

public final class SongBindingUtils extends SongServletUtils implements SongBindingFactory {

    private final BindingEndpointNode bindingNode;

    public SongBindingUtils(SongContainer container, ExtServletContext servletContext, BindingEndpointNode bindingNode) {
        super(container, servletContext);
        this.bindingNode = bindingNode;
    }

    public SongServletRequest createRequest(String protocol, String method, SongURI from, SongURI to, InetAddress localAddress,
            int localPort, InetAddress remoteAddress, int remotePort, boolean proxy) {
        return new RemoteRequest(bindingNode, method, protocol, from, to, localAddress, localPort, remoteAddress, remotePort,
                proxy, null);
    }

    public SongServletRequest createRequest(String protocol, String method, SongURI from, SongURI to, InetAddress localAddress,
            int localPort, InetAddress remoteAddress, int remotePort, boolean proxy, String id) {
        return new RemoteRequest(bindingNode, method, protocol, from, to, localAddress, localPort, remoteAddress, remotePort,
                proxy, id);
    }

    public SongServletResponse createResponse(SongServletRequest request, int statusCode, String reasonPhrase,
            InetAddress localAddress, int localPort, InetAddress remoteAddress, int remotePort) {
        if (LocalWrappedRequest.class == request.getClass()) {
            LocalWrappedRequest realRequest = (LocalWrappedRequest) request;
            return realRequest.createResponse(statusCode, reasonPhrase, localAddress, localPort, remoteAddress, remotePort);
        }

        throw new IllegalArgumentException("Unknown request, cannot create its response");
    }
}
