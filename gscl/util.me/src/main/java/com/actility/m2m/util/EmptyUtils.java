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

import java.util.Collections;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;

/**
 * Utils class to access empty iterators, enumerations, collections and arrays.
 */
public final class EmptyUtils {

    private EmptyUtils() {
        // Static class
    }

    private static final class EmptyEnumeration implements Enumeration {

        public boolean hasMoreElements() {
            return false;
        }

        public Object nextElement() {
            throw new NoSuchElementException();
        }
    }

    private static class EmptyIterator implements Iterator {

        public boolean hasNext() {
            return false;
        }

        public Object next() {
            throw new NoSuchElementException();
        }

        public void remove() {
            throw new UnsupportedOperationException();
        }
    }

    private static final class EmptyListIterator extends EmptyIterator implements ListIterator {

        public void add(Object o) {
            throw new UnsupportedOperationException();
        }

        public boolean hasPrevious() {
            return false;
        }

        public int nextIndex() {
            return 0;
        }

        public Object previous() {
            return null;
        }

        public int previousIndex() {
            return 0;
        }

        public void set(Object o) {
            throw new UnsupportedOperationException();
        }

    }

    /**
     * Empty Map.
     */
    public static final Map EMPTY_MAP = Collections.EMPTY_MAP;
    /**
     * Empty List.
     */
    public static final List EMPTY_LIST = Collections.EMPTY_LIST;
    /**
     * Empty Set.
     */
    public static final Set EMPTY_SET = Collections.EMPTY_SET;
    /**
     * Empty array of objects (Object[]).
     */
    public static final Object[] EMPTY_OBJECTS_ARRAY = new Object[0];
    /**
     * Empty array of bytes (byte[]).
     */
    public static final byte[] EMPTY_BYTES_ARRAY = new byte[0];
    /**
     * Empty array of strings (String[]).
     */
    public static final String[] EMPTY_STRINGS_ARRAY = new String[0];
    /**
     * Empty enumeration.
     */
    public static final Enumeration EMPTY_ENUMERATION = new EmptyEnumeration();
    /**
     * Empty iterator.
     */
    public static final Iterator EMPTY_ITERATOR = new EmptyIterator();
    /**
     * Empty list iterator.
     */
    public static final ListIterator EMPTY_LIST_ITERATOR = new EmptyListIterator();
}
