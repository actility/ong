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
 * id $Id: Pair.java 6057 2013-10-14 11:50:13Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6057 $
 * lastrevision $Date: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.util;

/**
 * Utils class to embeds two objects into one.
 */
public final class Pair {
    private Object first;
    private Object second;

    /**
     * Builds a Pair from the given objects.
     *
     * @param first The first object to bind to the pair
     * @param second The second object to bind to the pair
     */
    public Pair(Object first, Object second) {
        this.first = first;
        this.second = second;
    }

    /**
     * Gets the first object bound to the Pair.
     *
     * @return The first object
     */
    public Object getFirst() {
        return first;
    }

    /**
     * Sets the first object bound to the Pair.
     *
     * @param first The first object
     */
    public void setFirst(Object first) {
        this.first = first;
    }

    /**
     * Gets the second object bound to the Pair.
     *
     * @return The second object
     */
    public Object getSecond() {
        return second;
    }

    /**
     * Gets the second object bound to the Pair.
     *
     * @param second The second object
     */
    public void setSecond(Object second) {
        this.second = second;
    }
}
