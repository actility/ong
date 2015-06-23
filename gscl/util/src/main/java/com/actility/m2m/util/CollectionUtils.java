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
 * id $Id: CollectionUtils.java 6057 2013-10-14 11:50:13Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6057 $
 * lastrevision $Date: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.util;

import java.util.Collection;
import java.util.Iterator;

/**
 * Utils class to manage collections.
 */
public final class CollectionUtils {

    private CollectionUtils() {
        // Static class
    }

    /**
     * Generate a string from a Collection. Useful to generate logs.
     * <p>
     * Example:
     * <p>
     * { 10, 5, 3 } with prefix "[", suffix "]" and separator "-" : [10-5-3]
     *
     * @param collection The collection from which to generate a string
     * @param prefix The prefix of the generated string
     * @param suffix The suffix of the generated string
     * @param separator Separator to use between each element
     * @return The generated string.
     */
    public static String toString(Collection collection, String prefix, String suffix, String separator) {
        StringBuffer buffer = new StringBuffer();
        if (prefix != null) {
            buffer.append(prefix);
        }
        if (!collection.isEmpty()) {
            Iterator it = collection.iterator();
            buffer.append(it.next());
            while (it.hasNext()) {
                buffer.append(separator);
                buffer.append(it.next());
            }
        }
        if (suffix != null) {
            buffer.append(suffix);
        }
        return buffer.toString();
    }
}
