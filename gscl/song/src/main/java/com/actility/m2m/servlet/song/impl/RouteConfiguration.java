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
 * id $Id: RouteConfiguration.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.impl;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.UnknownHostException;

public final class RouteConfiguration {

    private final String subnet;
    private final long minIp;
    private final long maxIp;
    private final boolean serverCapable;

    public RouteConfiguration(String subnet, int cidr, boolean serverCapable) {
        if (cidr > 32) {
            throw new IllegalArgumentException("Cannot create a route configuration with a cidr strictly greater than 32");
        }
        try {
            InetAddress subnetInet = InetAddress.getByName(subnet);

            if (Inet4Address.class != subnetInet.getClass()) {
                throw new IllegalArgumentException("Cannot use an IPv6 address for route configuration");
            }
            long subnetAddress = getAddressFromInetAddress(subnetInet);
            int mask = 0;
            for (int i = 0; i < cidr; ++i) {
                mask |= (1 << (31 - i));
            }
            int rMask = ~mask;
            this.minIp = subnetAddress & (mask & 0xffffffffL);
            this.maxIp = (this.minIp | (rMask & 0xffffffffL));
            this.subnet = subnet + "/" + String.valueOf(cidr);
            this.serverCapable = serverCapable;
        } catch (UnknownHostException e) {
            throw new IllegalArgumentException("Bad subnet address " + subnet);
        }
    }

    private long getAddressFromInetAddress(InetAddress address) {
        byte[] rawAddress = address.getAddress();
        long result = 0;
        result = (result << 8) + (rawAddress[0] & 0xff);
        result = (result << 8) + (rawAddress[1] & 0xff);
        result = (result << 8) + (rawAddress[2] & 0xff);
        result = (result << 8) + (rawAddress[3] & 0xff);
        return result;
    }

    public boolean isInRoute(InetAddress address) {
        if (Inet4Address.class != address.getClass()) {
            return false;
        }
        long ipAddress = getAddressFromInetAddress(address);
        return (ipAddress >= minIp) && (ipAddress <= maxIp);
    }

    public String getSubnet() {
        return subnet;
    }

    public boolean isServerCapable() {
        return serverCapable;
    }

    public String toString() {
        return subnet + ":" + serverCapable;
    }

    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + (int) (maxIp ^ (maxIp >>> 32));
        result = prime * result + (int) (minIp ^ (minIp >>> 32));
        result = prime * result + (serverCapable ? 1231 : 1237);
        return result;
    }

    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        RouteConfiguration other = (RouteConfiguration) obj;
        if (maxIp != other.maxIp) {
            return false;
        }
        if (minIp != other.minIp) {
            return false;
        }
        if (serverCapable != other.serverCapable) {
            return false;
        }
        return true;
    }
}
