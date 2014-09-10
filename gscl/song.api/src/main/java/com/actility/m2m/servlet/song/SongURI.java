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
 * id $Id: SongURI.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import java.util.Iterator;

/**
 * Represents a protocol URIs. URIs may contain parameters and headers (although headers are not legal in all contexts)
 *
 * @see SongFactory
 */
public interface SongURI {

    /**
     * Compares the given URI with this URI. The comparison rules to be followed shall depend upon the underlying URI scheme
     * being used. For general purpose URIs RFC 2396 should be consulted for equality. If the URIs are of scheme for which
     * comparison rules are further specified in their specifications, then they must be used for any comparison.
     */
    boolean equals(java.lang.Object o);

    /**
     * Returns the scheme of this URI, for example "http", "https", "sip", "sips", "song", "songs" ...
     *
     * @return The scheme of the URI
     */
    String getScheme();

    /**
     * Returns the host part of this URI.
     *
     * @return The host part of this URI
     */
    java.lang.String getHost();

    /**
     * Returns the value of the path of this URI.
     *
     * @return The value of the path of this URI
     */
    java.lang.String getPath();

    /**
     * Returns the port number of this URI, or -1 if this is not set.
     *
     * @return The port number of this URI, or -1 if this is not set
     */
    int getPort();

    /**
     * Returns true if this URI is secure, that ends with an "s".
     * <p>
     * Examples: https, sips, songs...
     *
     * @return Whether the URI is secured
     */
    boolean isSecure();

    /**
     * Sets the host part of this URI. This should be a fully qualified domain name or a numeric IP address.
     *
     * @param host The host part of this URI
     */
    void setHost(java.lang.String host);

    /**
     * Sets the value of the path of this URI.
     *
     * @param path The path to set in the URI
     */
    void setPath(String path);

    /**
     * Sets the port number of this URI.
     *
     * @param port The port number of this URI
     */
    void setPort(int port);

    /**
     * Sets the scheme of this URI.
     *
     * @param b The secure flag
     */
    void setSecure(boolean b);

    Iterator getQueryParameters();

    /**
     * Returns the value of the named parameter, or null if it is not set. A zero-length String indicates flag parameter.
     *
     * @param key The key of the parameter to get
     * @return The value of the named parameter, or null if it is not set. A zero-length String indicates flag parameter
     */
    String getQueryParameter(java.lang.String key);

    /**
     * Returns the value of the named parameter, or null if it is not set. A zero-length String indicates flag parameter.
     *
     * @param key The key of the parameter to get
     * @return The value of the named parameter, or null if it is not set. A zero-length String indicates flag parameter
     */
    Iterator getQueryParameters(java.lang.String key);

    /**
     * Removes the named parameter from this URL. Nothing is done if the URL did not already contain the specific parameter.
     *
     * @param name The name of the parameter to remove
     */
    void removeQueryParameter(java.lang.String name);

    /**
     * Removes the named parameter from this URL. Nothing is done if the URL did not already contain the specific parameter.
     *
     * @param name The name of the parameter to remove
     */
    void removeQueryParameters(java.lang.String name);

    /**
     * Sets the value of the named parameter. If this URL previously contained a value for the given parameter name, then the
     * old value is replaced by the specified value. The setting of a flag parameter is indicated by specifying a zero-length
     * String for the parameter value.
     *
     * @param name The name of the parameter to set
     * @param value The value of the parameter to set
     */
    void setQueryParameter(java.lang.String name, java.lang.String value);

    /**
     * Sets the value of the named parameter. If this URL previously contained a value for the given parameter name, then the
     * old value is replaced by the specified value. The setting of a flag parameter is indicated by specifying a zero-length
     * String for the parameter value.
     *
     * @param name The name of the parameter to set
     * @param value The value of the parameter to set
     */
    void addQueryParameter(java.lang.String name, java.lang.String value);

    String getFragment();

    void setFragment(String fragment);

    /**
     * Returns the absolute representation of this URI. Any reserved characters will be properly escaped according to RFC2396.
     *
     * @return The absolute URI as a string
     */
    java.lang.String absoluteURI();

    /**
     * Returns the Song URI as a native java {@link java.net.URI}.
     *
     * @return The Song URI as a native java URI
     */
    java.net.URI toURI();

    /**
     * Returns a copy of this URI
     *
     * @return the copied created
     */
    SongURI copy();
}
