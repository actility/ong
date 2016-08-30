/*******************************************************************************
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
 *******************************************************************************/

package com.actility.m2m.util;

/**
 * Useful methods to use URIs
 */
public final class URIUtils {

    private URIUtils() {
        // Static class
    }

    /**
     * Low bitmask for dash character:
     * <p>
     * dash = "-"
     */
    public static final long L_DASH = CharacterUtils.lowMask("-");
    /**
     * High bitmask for dash character:
     * <p>
     * dash = "-"
     */
    public static final long H_DASH = CharacterUtils.highMask("-");

    /**
     * Low bitmask for dash character:
     * <p>
     * leftbracket = "-"
     */
    public static final long L_LEFT_BRACKET = CharacterUtils.lowMask("[");
    /**
     * High bitmask for dash character:
     * <p>
     * leftbracket = "-"
     */
    public static final long H_LEFT_BRACKET = CharacterUtils.highMask("[");

    /**
     * Low bitmask for mark characters:
     * <p>
     * mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
     */
    public static final long L_MARK = CharacterUtils.lowMask("-_.!~*'()");
    /**
     * High bitmask for mark characters:
     * <p>
     * mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
     */
    public static final long H_MARK = CharacterUtils.highMask("-_.!~*'()");

    /**
     * Low bitmask for URL unreserved characters:
     * <p>
     * unreserved = alphanum | mark
     */
    public static final long L_UNRESERVED = CharacterUtils.L_ALPHANUM | L_MARK;
    /**
     * High bitmask for URL unreserved characters:
     * <p>
     * unreserved = alphanum | mark
     */
    public static final long H_UNRESERVED = CharacterUtils.H_ALPHANUM | H_MARK;

    /**
     * Low bitmask for URL reserved characters:
     * <p>
     * reserved = ";" | "/" | "?" | ":" | "@" | "&amp;" | "=" | "+" | "$" | "," | "[" | "]"
     */
    public static final long L_RESERVED = CharacterUtils.lowMask(";/?:@&=+$,[]");
    /**
     * High bitmask for URL reserved characters:
     * <p>
     * reserved = ";" | "/" | "?" | ":" | "@" | "&amp;" | "=" | "+" | "$" | "," | "[" | "]"
     */
    public static final long H_RESERVED = CharacterUtils.highMask(";/?:@&=+$,[]");

    /**
     * Low bitmask for URI characters:
     * <p>
     * uric = reserved | unreserved | escaped
     */
    public static final long L_URIC = L_RESERVED | L_UNRESERVED | CharacterUtils.L_ESCAPED;
    /**
     * High bitmask for URI characters:
     * <p>
     * uric = reserved | unreserved | escaped
     */
    public static final long H_URIC = H_RESERVED | H_UNRESERVED | CharacterUtils.H_ESCAPED;

    /**
     * Low bitmask for URI characters without '/':
     * <p>
     * uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" | "&amp;" | "=" | "+" | "$" | ","
     */
    public static final long L_URIC_NO_SLASH = L_UNRESERVED | CharacterUtils.L_ESCAPED | CharacterUtils.lowMask(";?:@&=+$,");
    /**
     * High bitmask for URI characters without '/':
     * <p>
     * uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" | "&amp;" | "=" | "+" | "$" | ","
     */
    public static final long H_URIC_NO_SLASH = H_UNRESERVED | CharacterUtils.H_ESCAPED | CharacterUtils.highMask(";?:@&=+$,");

    /**
     * Low bitmask for scheme characters:
     * <p>
     * scheme = alpha *( alpha | digit | "+" | "-" | "." )
     */
    public static final long L_SCHEME = CharacterUtils.L_ALPHA | CharacterUtils.L_DIGIT | CharacterUtils.lowMask("+-.");
    /**
     * High bitmask for scheme characters:
     * <p>
     * scheme = alpha *( alpha | digit | "+" | "-" | "." )
     */
    public static final long H_SCHEME = CharacterUtils.H_ALPHA | CharacterUtils.H_DIGIT | CharacterUtils.highMask("+-.");

    /**
     * Low bitmask for registration based authority characters:
     * <p>
     * reg_name = 1*( unreserved | escaped | "$" | "," | ";" | ":" | "@" | "&amp;" | "=" | "+" )
     */
    public static final long L_REG_NAME = L_UNRESERVED | CharacterUtils.L_ESCAPED | CharacterUtils.lowMask("$,;:@&=+");
    /**
     * High bitmask for registration based authority characters:
     * <p>
     * reg_name = 1*( unreserved | escaped | "$" | "," | ";" | ":" | "@" | "&amp;" | "=" | "+" )
     */
    public static final long H_REG_NAME = H_UNRESERVED | CharacterUtils.H_ESCAPED | CharacterUtils.highMask("$,;:@&=+");

    /**
     * Low bitmask for user-info characters:
     * <p>
     * userinfo = *( unreserved | escaped | ";" | ":" | "&amp;" | "=" | "+" | "$" | "," )
     */
    public static final long L_USERINFO = L_UNRESERVED | CharacterUtils.L_ESCAPED | CharacterUtils.lowMask(";:&=+$,");
    /**
     * High bitmask for user-info characters:
     * <p>
     * userinfo = *( unreserved | escaped | ";" | ":" | "&amp;" | "=" | "+" | "$" | "," )
     */
    public static final long H_USERINFO = H_UNRESERVED | CharacterUtils.H_ESCAPED | CharacterUtils.highMask(";:&=+$,");

    /**
     * Low bitmask for server based hostname part characters:
     * <p>
     * hostname = *( domainlabel "." ) toplabel [ "." ] domainlabel = alphanum | alphanum *( alphanum | "-" ) alphanum toplabel
     * = alpha | alpha *( alphanum | "-" ) alphanum
     */
    public static final long L_HOSTNAME_PART = CharacterUtils.L_ALPHANUM | L_DASH;
    /**
     * High bitmask for server based hostname part characters:
     * <p>
     * hostname = *( domainlabel "." ) toplabel [ "." ] domainlabel = alphanum | alphanum *( alphanum | "-" ) alphanum toplabel
     * = alpha | alpha *( alphanum | "-" ) alphanum
     */
    public static final long H_HOSTNAME_PART = CharacterUtils.H_ALPHANUM | H_DASH;

    /**
     * Low bitmask for path segment characters:
     * <p>
     * pchar = unreserved | escaped | ":" | "@" | "&amp;" | "=" | "+" | "$" | ","
     */
    public static final long L_PCHAR = L_UNRESERVED | CharacterUtils.L_ESCAPED | CharacterUtils.lowMask(":@&=+$,");
    /**
     * High bitmask for path segment characters:
     * <p>
     * pchar = unreserved | escaped | ":" | "@" | "&amp;" | "=" | "+" | "$" | ","
     */
    public static final long H_PCHAR = H_UNRESERVED | CharacterUtils.H_ESCAPED | CharacterUtils.highMask(":@&=+$,");

    /**
     * Low bitmask for URL query segment characters:
     * <p>
     * query = unreserved | escaped | ";" | "/" | "?" | ":" | "@" | "+" | "$" | "," | "[" | "]"
     */
    public static final long L_QUERY = L_UNRESERVED | CharacterUtils.L_ESCAPED | CharacterUtils.lowMask(";/?:@+$,[]");
    /**
     * High bitmask for URL query segment characters:
     * <p>
     * query = unreserved | escaped | ";" | "/" | "?" | ":" | "@" | "+" | "$" | "," | "[" | "]"
     */
    public static final long H_QUERY = H_UNRESERVED | CharacterUtils.H_ESCAPED | CharacterUtils.highMask(";/?:@+$,[]");

    // Aggregated rules
    /**
     * Low bitmask for path characters:
     * <p>
     * path = pchar | ";" | "/"
     */
    public static final long L_PATH = L_PCHAR | CharacterUtils.lowMask(";/");
    /**
     * High bitmask for path characters:
     * <p>
     * path = pchar | ";" | "/"
     */
    public static final long H_PATH = H_PCHAR | CharacterUtils.highMask(";/");

    /**
     * Low bitmask for host characters:
     * <p>
     * host = hostname | IPv4address | IPv6Address
     */
    public static final long L_HOST = L_HOSTNAME_PART | CharacterUtils.L_HEX | CharacterUtils.lowMask("%.:[]");
    /**
     * Low bitmask for host characters:
     * <p>
     * host = hostname | IPv4address | IPv6Address
     */
    public static final long H_HOST = H_HOSTNAME_PART | CharacterUtils.H_HEX | CharacterUtils.highMask("%.:[]");

    /**
     * Encodes a string in the opaque part of a URI.
     * <p>
     * scheme:opaque-part
     *
     * @param value The opaque part
     * @return The encoded opaque part
     */
    public static String encodeOpaquePart(String value) {
        return StringUtils.quote(value, L_URIC, H_URIC);
    }

    /**
     * Encodes a string in the registry part of a hierarchical URI.
     * <p>
     * scheme://registry-part
     *
     * @param value The registry part
     * @return The encoded registry part
     */
    public static String encodeRegistry(String value) {
        return StringUtils.quote(value, L_REG_NAME, H_REG_NAME);
    }

    /**
     * Encodes a string in the user-info part of a server based URI.
     * <p>
     * scheme://userinfo@server:port
     *
     * @param value The user-info part
     * @return The encoded user-info part part
     */
    public static String encodeUserInfo(String value) {
        return StringUtils.quote(value, L_USERINFO, H_USERINFO);
    }

    /**
     * Encodes a string in the path part of a hierarchical URI.
     * <p>
     * scheme://[authority]path
     *
     * @param value The path part
     * @return The encoded path part
     */
    public static String encodePath(String value) {
        return StringUtils.quote(value, L_PATH, H_PATH);
    }

    /**
     * Encodes a string in the path segment part of a path.
     * <p>
     * scheme://[authority]/path-segment/path-segment...
     *
     * @param value The path-segment part
     * @return The encoded path-segment part
     */
    public static String encodePathSegment(String value) {
        return StringUtils.quote(value, L_PCHAR, H_PCHAR);
    }

    /**
     * Encodes a string in the query part of a hierarchical URI.
     * <p>
     * scheme://[authority]?query
     *
     * @param value The query part
     * @return The encoded query part
     */
    public static String encodeQuery(String value) {
        return StringUtils.quote(value, L_URIC, H_URIC);
    }

    /**
     * Encodes a string in the query-segment part of a query.
     * <p>
     * scheme://[authority]?query-segment=query-segment&amp;query-segment=query-segment...
     *
     * @param value The query-segment part
     * @return The encoded query-segment part
     */
    public static String encodeQuerySegment(String value) {
        return StringUtils.quote(value, L_QUERY, H_QUERY);
    }

    /**
     * Encodes a string in the fragment part of a URI.
     * <p>
     * [ absoluteURI | relativeURI ] [ "#" fragment ]
     *
     * @param value The fragment part
     * @return The encoded fragment part
     */
    public static String encodeFragment(String value) {
        return StringUtils.quote(value, L_URIC, H_URIC);
    }
}
