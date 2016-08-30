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

import java.util.AbstractMap;
import java.util.AbstractSet;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;

/**
 * Builds a Map that can have multiple values for a single key.
 *
 */
public final class MultiMap extends AbstractMap {

    private int size;
    private SortedMap map;

    /**
     * Builds a MultiMap.
     */
    public MultiMap() {
        this((Comparator) null);
    }

    /**
     * Builds a MultiMap using the given comparator to compare keys.
     *
     * @param c The comparator used to compare the multimap key
     */
    public MultiMap(Comparator c) {
        map = new TreeMap(c);
    }

    /**
     * Builds a MultiMap by copying the given MultiMap using the same comparator.
     *
     * @param copy The MultiMap to copy to build this MultiMap
     */
    public MultiMap(MultiMap copy) {
        map = new TreeMap(copy.comparator());
        map.putAll(copy.map);
    }

    /**
     * Returns the number of key-value mappings in this map.
     *
     * @return the number of key-value mappings in this map.
     */
    public int size() {
        return size;
    }

    /**
     * Returns <tt>true</tt> if this map contains a mapping for the specified key.
     *
     * @param key key whose presence in this map is to be tested.
     *
     * @return <tt>true</tt> if this map contains a mapping for the specified key.
     * @throws ClassCastException if the key cannot be compared with the keys currently in the map.
     * @throws NullPointerException key is <tt>null</tt> and this map uses natural ordering, or its comparator does not tolerate
     *             <tt>null</tt> keys.
     */
    public boolean containsKey(Object key) {
        return map.containsKey(key);
    }

    /**
     * Returns <tt>true</tt> if this map maps one or more keys to the specified value. More formally, returns <tt>true</tt> if
     * and only if this map contains at least one mapping to a value <tt>v</tt> such that
     * <tt>(value==null ? v==null : value.equals(v))</tt>. This operation will probably require time linear in the Map size for
     * most implementations of Map.
     *
     * @param value value whose presence in this Map is to be tested.
     * @return <tt>true</tt> if a mapping to <tt>value</tt> exists; <tt>false</tt> otherwise.
     * @since 1.2
     */
    public boolean containsValue(Object value) {
        Iterator iter = map.entrySet().iterator();
        boolean found = false;
        Entry entry = null;
        List values = null;
        while (iter.hasNext() && !found) {
           entry = (Entry) iter.next();
            if (ArrayList.class == entry.getValue().getClass()) {
                values = (List) entry.getValue();
                found = values.contains(value);
            } else {
                found = entry.getValue().equals(value);
            }
        }
        return found;
    }

    /**
     * Gets the first value corresponding to the given key.
     *
     * @return The first value corresponding to the given key
     */
    public Object get(Object key) {
        Object value = map.get(key);
        if (value != null && ArrayList.class == value.getClass()) {
            value = ((List) value).get(0);
        }
        return value;
    }

    /**
     * Gets all values corresponding to the given key.
     *
     * @param key The key from which to retrieve the list of values.
     * @return All values corresponding to the given key
     */
    public List getAll(Object key) {
        Object value = map.get(key);
        if (value == null) {
            return null;
        } else if (ArrayList.class == value.getClass()) {
            return (List) value;
        } else {
            return Arrays.asList(new Object[] { value });
        }
    }

    /**
     * Returns the comparator used to order this map, or <tt>null</tt> if this map uses its keys' natural order.
     *
     * @return the comparator associated with this sorted map, or <tt>null</tt> if it uses its keys' natural sort method.
     */
    public Comparator comparator() {
        return map.comparator();
    }

    /**
     * Removes and put the given the given value associated to the given key.
     * <p>
     * This method does erase already registered keys.
     *
     * @param key key with which the specified value is to be associated.
     * @param value value to be associated with the specified key.
     */
    public void removeAndPut(Object key, Object value) {
        Object original = map.put(key, value);
        if (original == null) {
            ++size;
        } else if (ArrayList.class == original.getClass()) {
            List values = (List) original;
            size -= values.size();
        }
    }

    /**
     * Put the given the given value associated to the given key.
     * <p>
     * This method does not erase already registered keys.
     *
     * @param key key with which the specified value is to be associated.
     * @param value value to be associated with the specified key.
     * @return The last value registered for the given key
     */
    public Object put(Object key, Object value) {
        return putLast(key, value);
    }

    /**
     * Put the given the given value associated to the given key at the first index.
     * <p>
     * This method does not erase already registered keys.
     *
     * @param key key with which the specified value is to be associated.
     * @param value value to be associated with the specified key.
     * @return The previous first value registered for the given key
     */
    public Object putFirst(Object key, Object value) {
        Object original = map.get(key);
        ++size;
        if (original == null) {
            return map.put(key, value);
        } else if (ArrayList.class == original.getClass()) {
            List values = (List) original;
            original = values.get(0);
            values.add(0, value);
            return original;
        } else {
            List values = new ArrayList();
            values.add(value);
            values.add(original);
            map.put(key, values);
            return original;
        }
    }

    /**
     * Put the given the given value associated to the given key at the last index.
     * <p>
     * This method does not erase already registered keys.
     *
     * @param key key with which the specified value is to be associated.
     * @param value value to be associated with the specified key.
     * @return The last value registered for the given key
     */
    public Object putLast(Object key, Object value) {
        Object original = map.get(key);
        ++size;
        if (original == null) {
            return map.put(key, value);
        } else if (ArrayList.class == original.getClass()) {
            List values = (List) original;
            original = values.get(values.size() - 1);
            values.add(value);
            return original;
        } else {
            List values = new ArrayList();
            values.add(original);
            values.add(value);
            map.put(key, values);
            return original;
        }
    }

    /**
     * Removes the value corresponding to the given key.
     *
     * @return The last value corresponding to the given key
     */
    public Object remove(Object key) {
        Object original = map.get(key);
        if (original != null && ArrayList.class == original.getClass()) {
            List values = (List) original;
            original = values.remove(0);
            if (values.size() == 0) {
                map.remove(key);
            }
        } else {
            map.remove(key);
        }
        --size;
        return original;
    }

    /**
     * Removes all values corresponding to the given key.
     *
     * @param key The key to remove
     * @return The last value corresponding to the given key
     */
    public Object removeAll(Object key) {
        Object original = map.remove(key);
        if (original != null && ArrayList.class == original.getClass()) {
            List values = (List) original;
            original = values.get(values.size() - 1);
            size -= values.size();
        } else {
            --size;
        }
        return original;
    }

    /**
     * Removes all mappings from this MultiMap.
     */
    public void clear() {
        size = 0;
        map.clear();
    }

    public void putAll(Map t) {
        size += t.size();
        super.putAll(t);
    }

    /**
     * Returns a Set view of the keys contained in this map. The set's iterator will return the keys in ascending order. The map
     * is backed by this <tt>TreeMap</tt> instance, so changes to this map are reflected in the Set, and vice-versa. The Set
     * supports element removal, which removes the corresponding mapping from the map, via the <tt>Iterator.remove</tt>,
     * <tt>Set.remove</tt>, <tt>removeAll</tt>, <tt>retainAll</tt>, and <tt>clear</tt> operations. It does not support the
     * <tt>add</tt> or <tt>addAll</tt> operations.
     *
     * @return a set view of the keys contained in this MultiMap.
     */
    public Set keySet() {
        return map.keySet();
    }

    public Set entrySet() {
        final Iterator entries = map.entrySet().iterator();

        return new AbstractSet() {
            public Iterator iterator() {
                return new Iterator() {
                    private int pos = 0;
                    private Map.Entry entry;
                    private Iterator values;

                    public void remove() {
                        throw new UnsupportedOperationException();
                    }

                    public boolean hasNext() {
                        return pos != MultiMap.this.size();
                    }

                    public Object next() {
                        Object key = null;
                        Object value = null;
                        while (true) {
                            if (entry == null || values == null) {
                                entry = (Map.Entry) entries.next();
                                values = null;
                                if (ArrayList.class == entry.getValue().getClass()) {
                                    values = ((List) entry.getValue()).iterator();
                                } else {
                                    pos++;
                                    return entry;
                                }
                            }
                            key = entry.getKey();
                            if (values.hasNext()) {
                                value = values.next();
                                pos++;
                                return new Entry(key, value);
                            }
                            entry = null;
                        }
                    }
                };
            }

            public int size() {
                return MultiMap.this.size();
            }
        };
    }

    public String toString() {
        StringBuffer buffer = new StringBuffer();
        buffer.append('{');
        Iterator keys = map.keySet().iterator();
        Object key = null;
        Object value = null;
        List values = null;
        Iterator itValues = null;
        Object realValue = null;
        while (keys.hasNext()) {
            key = keys.next();
            value = map.get(key);
            if (ArrayList.class == value.getClass()) {
                values = (List) value;
                itValues = values.iterator();
                while (itValues.hasNext()) {
                    realValue = itValues.next();
                    buffer.append(key);
                    buffer.append(": ");
                    buffer.append(realValue);
                    buffer.append(", ");
                }
            } else {
                buffer.append(key);
                buffer.append(": ");
                buffer.append(value);
                buffer.append(", ");
            }
        }
        if (!map.isEmpty()) {
            buffer.setLength(buffer.length() - 2);
        }
        buffer.append('}');
        return buffer.toString();
    }

    private static final class Entry implements Map.Entry {
        private final Object key;
        private Object value;

        public Entry(Object key, Object value) {
            this.key = key;
            this.value = value;
        }

        public Object getKey() {
            return key;
        }

        public Object getValue() {
            return value;
        }

        public Object setValue(Object value) {
            Object oldValue = this.value;
            this.value = value;
            return oldValue;
        }

        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || !(o instanceof Map.Entry)) {
                return false;
            }
            Map.Entry e = (Map.Entry) o;
            return ((key == null) ? e.getKey() == null : key.equals(e.getKey()))
                    && ((value == null) ? e.getValue() == null : value.equals(e.getValue()));
        }

        public int hashCode() {
            return (value == null) ? 0 : value.hashCode();
        }

        public String toString() {
            return key + "=" + value;
        }
    }
}
