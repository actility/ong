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

package com.actility.m2m.cm;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Vector;

import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.util.IteratorEnum;

public final class ConfigurationDictionary extends Dictionary {

    /**
     * * Allowed object types.
     */

    private final static Set allowedSimpleTypes = new HashSet();
    static {
        allowedSimpleTypes.add(String.class);
        allowedSimpleTypes.add(Integer.class);
        allowedSimpleTypes.add(Long.class);
        allowedSimpleTypes.add(Float.class);
        allowedSimpleTypes.add(Double.class);
        allowedSimpleTypes.add(Byte.class);
        allowedSimpleTypes.add(Short.class);
        allowedSimpleTypes.add(Character.class);
        allowedSimpleTypes.add(Boolean.class);
    }

    /**
     * * Allowed primitive types in arrays.
     */
    private final static Set allowedArrayTypes = new HashSet();

    static {
        allowedArrayTypes.add(long[].class);
        allowedArrayTypes.add(int[].class);
        allowedArrayTypes.add(short[].class);
        allowedArrayTypes.add(char[].class);
        allowedArrayTypes.add(byte[].class);
        allowedArrayTypes.add(double[].class);
        allowedArrayTypes.add(float[].class);
        allowedArrayTypes.add(boolean[].class);
        allowedArrayTypes.add(String[].class);
        allowedArrayTypes.add(Integer[].class);
        allowedArrayTypes.add(Long[].class);
        allowedArrayTypes.add(Float[].class);
        allowedArrayTypes.add(Double[].class);
        allowedArrayTypes.add(Byte[].class);
        allowedArrayTypes.add(Short[].class);
        allowedArrayTypes.add(Character[].class);
        allowedArrayTypes.add(Boolean[].class);
    }

    private final SortedMap dictionary;
    private int nbUserProps;
    private String location;
    private boolean dynamic;

    /**
     * * Construct a ConfigurationDictionary given an ordinary Dictionary. * *
     */
    public ConfigurationDictionary() {
        this.dictionary = new TreeMap(String.CASE_INSENSITIVE_ORDER);
        this.nbUserProps = 0;
        this.location = null;
        this.dynamic = false;
    }

    /**
     * * Construct a ConfigurationDictionary given an ordinary Dictionary. * *
     */
    public ConfigurationDictionary(ConfigurationDictionary toCopy) {
        this.dictionary = new TreeMap(String.CASE_INSENSITIVE_ORDER);
        this.nbUserProps = 0;
        copyDictionary(toCopy.dictionary);
        this.location = toCopy.location;
        this.dynamic = toCopy.dynamic;
    }

    public String getLocation() {
        return location;
    }

    public void setLocation(String location) {
        this.location = location;
    }

    public boolean isDynamic() {
        return dynamic;
    }

    public void setDynamic(boolean dynamic) {
        this.dynamic = dynamic;
    }

    public int getNbUserProps() {
        return nbUserProps;
    }

    public boolean isEmpty() {
        return dictionary.isEmpty();
    }

    public Enumeration elements() {
        return new IteratorEnum(dictionary.values().iterator());
    }

    public Enumeration keys() {
        return new IteratorEnum(dictionary.keySet().iterator());
    }

    public Object get(Object key) {
        Property property = (Property) dictionary.get(key);
        if (property != null) {
            return property.getValue();
        }
        return null;
    }

    public Object putEtc(Object key, Object value) {
        if (key.getClass() != String.class) {
            throw new IllegalArgumentException("The key " + key + " is not of type java.lang.String.");
        }
        checkValue(value);
        Property property = (Property) dictionary.put(key, new Property(value, false));
        return (property != null) ? property.getValue() : null;
    }

    public Object put(Object key, Object value) {
        if (key.getClass() != String.class) {
            throw new IllegalArgumentException("The key " + key + " is not of type java.lang.String.");
        }
        checkValue(value);
        ++nbUserProps;
        Property property = (Property) dictionary.put(key, new Property(value, true));
        return (property != null) ? property.getValue() : null;
    }

    public Object remove(Object key) {
        Property property = (Property) dictionary.remove(key);
        if (property != null && property.isUserValue()) {
            --nbUserProps;
        }
        return property;
    }

    public int size() {
        return dictionary.size();
    }

    public Map getDictionary() {
        return dictionary;
    }

    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((dictionary == null) ? 0 : dictionary.hashCode());
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
        ConfigurationDictionary other = (ConfigurationDictionary) obj;
        if (dictionary == null) {
            if (other.dictionary != null) {
                return false;
            }
        } else if (!dictEquals(other.dictionary)) {
            return false;
        }
        return true;
    }

    private boolean dictEquals(SortedMap other) {
        if (dictionary.size() != other.size()) {
            return false;
        }

        boolean result = true;
        Iterator/* <Entry<String, Object>> */entries = dictionary.entrySet().iterator();
        while (result && entries.hasNext()) {
            Entry/* <String, Object> */entry = (Entry) entries.next();
            Object v1 = entry.getValue();
            Object v2 = other.get(entry.getKey());
            result = valuesAreEqual(v1, v2);
        }
        return result;
    }

    private boolean valuesAreEqual(Object first, Object second) {
        if (first == null && second == null) {
            return true;
        }
        if (first == null || second == null) {
            return false;
        }
        if (!first.getClass().equals(second.getClass())) {
            return false;
        }

        if (first.getClass().isArray()) {
            // Check all types of arrays
            Class componentClass = first.getClass();
            if (String[].class == componentClass) {
                return arraysAreEqual((String[]) first, (String[]) second);
            } else if (char[].class == componentClass) {
                return arraysAreEqual((char[]) first, (char[]) second);
            } else if (Character[].class == componentClass) {
                return arraysAreEqual((Character[]) first, (Character[]) second);
            } else if (boolean[].class == componentClass) {
                return arraysAreEqual((boolean[]) first, (boolean[]) second);
            } else if (Boolean[].class == componentClass) {
                return arraysAreEqual((Boolean[]) first, (Boolean[]) second);
            } else if (byte[].class == componentClass) {
                return arraysAreEqual((byte[]) first, (byte[]) second);
            } else if (Byte[].class == componentClass) {
                return arraysAreEqual((Byte[]) first, (Byte[]) second);
            } else if (int[].class == componentClass) {
                return arraysAreEqual((int[]) first, (int[]) second);
            } else if (Integer[].class == componentClass) {
                return arraysAreEqual((Integer[]) first, (Integer[]) second);
            } else if (short[].class == componentClass) {
                return arraysAreEqual((short[]) first, (short[]) second);
            } else if (Short[].class == componentClass) {
                return arraysAreEqual((Short[]) first, (Short[]) second);
            } else if (long[].class == componentClass) {
                return arraysAreEqual((long[]) first, (long[]) second);
            } else if (Long[].class == componentClass) {
                return arraysAreEqual((Long[]) first, (Long[]) second);
            } else if (float[].class == componentClass) {
                return arraysAreEqual((float[]) first, (float[]) second);
            } else if (Float[].class == componentClass) {
                return arraysAreEqual((Float[]) first, (Float[]) second);
            } else if (double[].class == componentClass) {
                return arraysAreEqual((double[]) first, (double[]) second);
            } else if (Double[].class == componentClass) {
                return arraysAreEqual((Double[]) first, (Double[]) second);
            }
            return false;
        } else if (first instanceof List) {
            return listsAreEqual((List) first, (List) second);
        } else if (first instanceof Vector) {
            return vectorsAreEqual((Vector) first, (Vector) second);
        } else {
            return first.equals(second);
        }
    }

    private boolean listsAreEqual(List first, List second) {
        if (first.size() != second.size()) {
            return false;
        }
        boolean result = true;
        for (int i = first.size(); result && i < first.size(); ++i) {
            result = valuesAreEqual(first.get(i), second.get(i));
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean vectorsAreEqual(Vector first, Vector second) {
        if (first.size() != second.size()) {
            return false;
        }
        boolean result = true;
        for (int i = first.size(); result && i < first.size(); ++i) {
            result = valuesAreEqual(first.elementAt(i), second.elementAt(i));
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(String[] first, String[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(char[] first, char[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Character[] first, Character[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(boolean[] first, boolean[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Boolean[] first, Boolean[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(byte[] first, byte[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Byte[] first, Byte[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(int[] first, int[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Integer[] first, Integer[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(short[] first, short[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Short[] first, Short[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(long[] first, long[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Long[] first, Long[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(float[] first, float[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Float[] first, Float[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(double[] first, double[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = first[i] != second[i];
            if (!result) {
                break;
            }
        }
        return result;
    }

    private boolean arraysAreEqual(Double[] first, Double[] second) {
        if (first.length != second.length) {
            return false;
        }
        boolean result = true;
        for (int i = 0; result && i < first.length; ++i) {
            result = valuesAreEqual(first[i], second[i]);
            if (!result) {
                break;
            }
        }
        return result;
    }

    private void copyDictionary(SortedMap in) {
        Iterator entries = in.entrySet().iterator();
        while (entries.hasNext()) {
            Entry entry = (Entry) entries.next();
            Property property = (Property) entry.getValue();
            if (property.isUserValue()) {
                ++nbUserProps;
            }
            dictionary.put(entry.getKey(), new Property(copyValue(property.getValue()), property.isUserValue()));
        }
    }

    private Object copyValue(Object in) {
        if (in == null) {
            return null;
        }
        if (in.getClass().isArray()) {
            // Check all types of arrays
            Class componentClass = in.getClass();
            if (String[].class == componentClass) {
                return copyArray((String[]) in);
            } else if (char[].class == componentClass) {
                return copyArray((char[]) in);
            } else if (Character[].class == componentClass) {
                return copyArray((Character[]) in);
            } else if (boolean[].class == componentClass) {
                return copyArray((boolean[]) in);
            } else if (Boolean[].class == componentClass) {
                return copyArray((Boolean[]) in);
            } else if (byte[].class == componentClass) {
                return copyArray((byte[]) in);
            } else if (Byte[].class == componentClass) {
                return copyArray((Byte[]) in);
            } else if (int[].class == componentClass) {
                return copyArray((int[]) in);
            } else if (Integer[].class == componentClass) {
                return copyArray((Integer[]) in);
            } else if (short[].class == componentClass) {
                return copyArray((short[]) in);
            } else if (Short[].class == componentClass) {
                return copyArray((Short[]) in);
            } else if (long[].class == componentClass) {
                return copyArray((long[]) in);
            } else if (Long[].class == componentClass) {
                return copyArray((Long[]) in);
            } else if (float[].class == componentClass) {
                return copyArray((float[]) in);
            } else if (Float[].class == componentClass) {
                return copyArray((Float[]) in);
            } else if (double[].class == componentClass) {
                return copyArray((double[]) in);
            } else if (Double[].class == componentClass) {
                return copyArray((Double[]) in);
            }
            return in;
        } else if (in instanceof Collection) {
            return copyCollection((Collection) in);
        } else {
            return in;
        }
    }

    private Collection copyCollection(Collection in) {
        if (in == null) {
            return null;
        }
        List out = new ArrayList();
        Iterator i = in.iterator();
        while (i.hasNext()) {
            out.add(copyValue(i.next()));
        }
        return out;
    }

    private Object copyArray(String[] in) {
        String[] out = new String[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(char[] in) {
        char[] out = new char[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Character[] in) {
        Character[] out = new Character[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(boolean[] in) {
        boolean[] out = new boolean[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Boolean[] in) {
        Boolean[] out = new Boolean[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(byte[] in) {
        byte[] out = new byte[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Byte[] in) {
        Byte[] out = new Byte[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(int[] in) {
        int[] out = new int[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Integer[] in) {
        Integer[] out = new Integer[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(short[] in) {
        short[] out = new short[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Short[] in) {
        Short[] out = new Short[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(long[] in) {
        long[] out = new long[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Long[] in) {
        Long[] out = new Long[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(float[] in) {
        float[] out = new float[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Float[] in) {
        Float[] out = new Float[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(double[] in) {
        double[] out = new double[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private Object copyArray(Double[] in) {
        Double[] out = new Double[in.length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    private void checkValue(Object value) throws IllegalArgumentException {
        if (value == null) {
            return;
        }

        Class valueClass = value.getClass();
        if (valueClass.isArray()) {
            checkArray(value);
        } else if (value instanceof Collection) {
            checkCollection((Collection) value);
        } else {
            checkSimple(value);
        }
    }

    private void checkArray(Object array) {
        if (!allowedArrayTypes.contains(array.getClass())) {
            throw new IllegalArgumentException("Illegal class type for arrays: " + array.getClass().toString());
        }
    }

    private void checkCollection(Collection collection) {
        Iterator i = collection.iterator();
        while (i.hasNext()) {
            checkSimple(i.next());
        }
    }

    private void checkSimple(Object value) {
        Class valueClass = value.getClass();
        if (!allowedSimpleTypes.contains(valueClass)) {
            throw new IllegalArgumentException(valueClass.toString() + " is not an allowed type.");
        }
    }

    public Dictionary toSearchableProperties() {
        Dictionary properties = new Hashtable();
        Iterator entries = dictionary.entrySet().iterator();
        while (entries.hasNext()) {
            Entry entry = (Entry) entries.next();
            properties.put(entry.getKey(), ((Property) entry.getValue()).getValue());
        }
        properties.put(ConfigurationAdmin.SERVICE_BUNDLELOCATION, location);
        properties.put(CMConstants.SERVICE_DYNAMICLOCATION, new Boolean(dynamic));
        return properties;
    }
}
