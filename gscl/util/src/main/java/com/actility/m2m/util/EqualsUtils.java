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
 * id $Id: EqualsUtils.java 6057 2013-10-14 11:50:13Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6057 $
 * lastrevision $Date: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.util;

/**
 * Utils class for equality tests.
 */
public final class EqualsUtils {

    private EqualsUtils() {
        // Static class
    }

    /**
     * Checks whether the given objects are equals.
     * This methods also works if object are null.
     *
     * @param obj1 The first object to check for equality
     * @param obj2 The second object to check for equality
     * @return Whether the given objects are equals
     */
    public static boolean isEqual(Object obj1, Object obj2) {
        return obj1 == obj2 || (obj1 != null && obj1.equals(obj2));
    }

    /**
     * Checks whether the given objects are not equals.
     * This methods also works if object are null.
     *
     * @param obj1 The first object to check
     * @param obj2 The second object to check
     * @return Whether the given objects are not equals
     */
    public static boolean isNotEqual(Object obj1, Object obj2) {
        return obj1 != obj2 && (obj1 == null || !obj1.equals(obj2));
    }
}
