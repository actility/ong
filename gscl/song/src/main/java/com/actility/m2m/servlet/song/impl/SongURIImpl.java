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
 * id $Id: SongURIImpl.java 7604 2014-02-06 13:01:34Z JReich $
 * author $Author: JReich $
 * version $Revision: 7604 $
 * lastrevision $Date: 2014-02-06 14:01:34 +0100 (Thu, 06 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-06 14:01:34 +0100 (Thu, 06 Feb 2014) $
 */

package com.actility.m2m.servlet.song.impl;

import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URLDecoder;
import java.text.ParseException;
import java.util.Iterator;
import java.util.Map;

import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.EqualsUtils;
import com.actility.m2m.util.MultiMap;
import com.actility.m2m.util.StringReader;
import com.actility.m2m.util.URIUtils;

public final class SongURIImpl implements SongURI {

    private static final char[] QUERY_DELIMITERS = "&=".toCharArray();

    private String rawUri;
    private URI uri;
    private MultiMap queryParams;
    private final String scheme;
    private String host;
    private int port;
    private String path;
    private String fragment;
    private boolean secure;

    public SongURIImpl(String uri, String containerHost, BindingNode defaultBinding) throws URISyntaxException {
        this(new URI(uri), containerHost, defaultBinding);
    }

    public SongURIImpl(URI uri, String containerHost, BindingNode defaultBinding) {
        if (uri.isOpaque()) {
            // TODO should support opaque URIs in the future
            throw new IllegalArgumentException("Opaque URIs are not supported");
        }
        if (uri.isAbsolute()) {
            this.scheme = uri.getScheme();
            this.secure = scheme.endsWith("s");
            this.host = uri.getHost();
            if (host == null) {
                throw new IllegalArgumentException("Host part contains an invalid character: " + uri.getAuthority());
            }
            this.port = uri.getPort();
            this.uri = uri;
        } else {
            if (uri.getPath() != null && (uri.getPath().length() == 0 || uri.getPath().charAt(0) != '/')) {
                throw new IllegalArgumentException(
                        "Song URI cannot be created from a relative URI with a path which does not start with a '/': " + uri);
            }
            this.scheme = defaultBinding.getServerScheme();
            this.secure = scheme.endsWith("s");
            this.host = containerHost;
            this.port = defaultBinding.getPort();
        }
        this.path = uri.getPath();
        this.fragment = uri.getFragment();
        if (uri.getRawQuery() != null) {
            try {
                queryParams = new MultiMap();
                StringReader reader = new StringReader(uri.getRawQuery());
                String queryParamName = null;
                String queryParamValue = null;
                while (!reader.isTerminated()) {
                    queryParamName = reader.readUntil(QUERY_DELIMITERS);
                    if (queryParamName.length() == 0) {
                        throw new IllegalArgumentException("Detected an empty query parameter name in the uri path: " + uri);
                    }
                    queryParamValue = queryParamName;
                    if (!reader.isTerminated() && '=' == reader.readCurrent()) {
                        reader.skipOffset(1);
                        queryParamValue = reader.readUntil('&');
                    }
                    queryParams.put(queryParamName, URLDecoder.decode(queryParamValue, "US-ASCII"));
                    if (!reader.isTerminated()) {
                        reader.skipOffset(1);
                    }
                }
            } catch (ParseException e) {
                throw new IllegalArgumentException("Parameter part of the path is not correct: " + uri);
            } catch (UnsupportedEncodingException e) {
                throw new IllegalArgumentException("US-ASCII character encoding is not supported");
            }
        }
    }

    public SongURIImpl(String scheme, boolean secure, String hostname, int port, String path) {
        this.scheme = scheme;
        this.secure = secure;
        this.host = hostname;
        this.port = port;
        this.path = path;
    }

    public SongURI copy() {

        SongURIImpl uri = new SongURIImpl(scheme, secure, host, port, path);
        if (queryParams != null) {
            Iterator it = queryParams.entrySet().iterator();
            Map.Entry entry = null;
            while (it.hasNext()) {
                entry = (Map.Entry) it.next();
                uri.addQueryParameter((String) entry.getKey(), (String) entry.getValue());
            }
        }
        uri.setFragment(fragment);
        return uri;
    }

    public String getHost() {
        return host;
    }

    public String getPath() {
        return path;
    }

    public int getPort() {
        return port;
    }

    public String getScheme() {
        return scheme;
    }

    public boolean isSecure() {
        return secure;
    }

    public void setHost(String host) {
        uri = null;
        rawUri = null;
        this.host = host;
    }

    public void setPath(String path) {
        uri = null;
        rawUri = null;
        this.path = path;
    }

    public void setPort(int port) {
        uri = null;
        rawUri = null;
        this.port = port;
    }

    public void setSecure(boolean b) {
        uri = null;
        rawUri = null;
        this.secure = b;
    }

    public Iterator getQueryParameters() {
        return (queryParams != null) ? queryParams.entrySet().iterator() : EmptyUtils.EMPTY_ITERATOR;
    }

    public String getQueryParameter(String key) {
        return (queryParams != null) ? (String) queryParams.get(key) : null;
    }

    public Iterator getQueryParameters(String key) {
        return (queryParams != null) ? queryParams.getAll(key).iterator() : EmptyUtils.EMPTY_ITERATOR;
    }

    public void removeQueryParameter(String name) {
        if (queryParams != null) {
            uri = null;
            queryParams.remove(name);
        }
    }

    public void removeQueryParameters(String name) {
        if (queryParams != null) {
            uri = null;
            rawUri = null;
            queryParams.removeAll(name);
        }
    }

    public void setQueryParameter(String name, String value) {
        uri = null;
        rawUri = null;
        if (queryParams != null) {
            queryParams.removeAndPut(name, value);
        } else {
            queryParams = new MultiMap();
            queryParams.put(name, value);
        }
    }

    public void addQueryParameter(String name, String value) {
        uri = null;
        rawUri = null;
        if (queryParams == null) {
            queryParams = new MultiMap();
        }
        queryParams.put(name, value);
    }

    public String getFragment() {
        return fragment;
    }

    public void setFragment(String fragment) {
        uri = null;
        rawUri = null;
        this.fragment = fragment;
    }

    public String absoluteURI() {
        if (rawUri == null) {
            StringBuffer buffer = new StringBuffer();
            buffer.append(scheme).append("://");
            boolean needBrackets = ((host.indexOf(':') >= 0) && !host.startsWith("[") && !host.endsWith("]"));
            if (needBrackets) {
                buffer.append('[');
            }
            buffer.append(host);
            if (needBrackets) {
                buffer.append(']');
            }
            if (port > 0) {
                buffer.append(':').append(port);
            }
            if (path != null) {
                buffer.append(URIUtils.encodePath(path));
            }
            if (queryParams != null) {
                buffer.append('?');
                boolean first = true;
                Iterator it = queryParams.entrySet().iterator();
                Map.Entry entry = null;
                while (it.hasNext()) {
                    entry = (Map.Entry) it.next();
                    if (first) {
                        first = false;
                    } else {
                        buffer.append('&');
                    }
                    buffer.append(URIUtils.encodeQuerySegment((String) entry.getKey()));
                    buffer.append('=');
                    buffer.append(URIUtils.encodeQuerySegment((String) entry.getValue()));
                }
            }
            if (fragment != null) {
                buffer.append('#').append(URIUtils.encodeFragment(fragment));
            }
            rawUri = buffer.toString();
        }
        return rawUri;
    }

    public int hashCode() {
        return absoluteURI().hashCode();
    }

    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (SongURIImpl.class != obj.getClass()) {
            return false;
        }
        SongURIImpl uri = (SongURIImpl) obj;
        if (EqualsUtils.isNotEqual(scheme, uri.scheme)) {
            return false;
        } else if (EqualsUtils.isNotEqual(host, uri.host)) {
            return false;
        } else if (EqualsUtils.isNotEqual(path, uri.path)) {
            return false;
        } else if (EqualsUtils.isNotEqual(fragment, uri.fragment)) {
            return false;
        } else if (port != uri.port) {
            return false;
        } else if (secure != uri.secure) {
            return false;
        } else if ((queryParams == null || uri.queryParams == null) && queryParams != uri.queryParams) {
            return false;
        } else if (queryParams != null && queryParams.size() != uri.queryParams.size()) {
            return false;
        } else {
            if (queryParams != null) {
                Iterator it = queryParams.keySet().iterator();
                Map.Entry entry = null;
                String headerName = null;
                String headerValue = null;
                String uriHeaderValue = null;
                while (it.hasNext()) {
                    entry = (Map.Entry) it.next();
                    headerName = (String) entry.getKey();
                    headerValue = (String) entry.getValue();
                    uriHeaderValue = (String) uri.queryParams.get(headerName);
                    if (uriHeaderValue == null || !headerValue.equals(uriHeaderValue)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    public URI toURI() {
        if (uri == null) {
            try {
                uri = new URI(absoluteURI());
            } catch (URISyntaxException e) {
                IllegalArgumentException y = new IllegalArgumentException();
                y.initCause(e);
                throw y;
            }
        }
        return uri;
    }
}
