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
 * id $Id: XoList.java 6092 2013-10-15 14:36:59Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6092 $
 * lastrevision $Date: 2013-10-15 16:36:59 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:36:59 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.xo.impl;

import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.NoSuchElementException;

import com.actility.m2m.xo.ni.NiXoService;

public final class XoList implements List, Iterator {
    public static final int TYPE_XO_OBJECT = 0;
    public static final int TYPE_STRING = 1;

    private final long handle;
    private final String attributeName;
    private final int ofType;
    private final boolean obix;
    private int currentIndex;
    private NiXoService xoNativeService;

    public XoList(NiXoService xoNativeService, long handle, boolean obix, String attributeName, int ofType) {
        this.xoNativeService = xoNativeService;
        this.handle = handle;
        this.obix = obix;
        this.attributeName = attributeName;
        this.ofType = ofType;
    }

    public boolean add(Object value) {
        if (value.getClass() == XoObjectImpl.class) {
            xoNativeService.xoObjectNmAddInAttr(handle, attributeName, ((XoObjectImpl) value).getHandle());
        } else if (value.getClass() == String.class) {
            xoNativeService.xoStringNmAddInAttr(handle, attributeName, (String) value);
        } else {
            throw new UnsupportedOperationException("Only XoObject and String are allowed in Xo Lists: " + value.getClass());
        }
        return true;
    }

    public boolean addAll(Collection col) {
        if (col != null) {
            Iterator it = col.iterator();
            while (it.hasNext()) {
                add(it.next());
            }
        }
        return true;
    }

    public void clear() {
        xoNativeService.xoNmDropList(handle, attributeName);
    }

    public boolean contains(Object value) {
        int index = -1;
        if (ofType == TYPE_XO_OBJECT) {
            if (value.getClass() == XoObjectImpl.class) {
                index = xoNativeService.xoObjectNmIndexOf(handle, attributeName, ((XoObjectImpl) value).getHandle());
            }
        } else if (value.getClass() == String.class) {
            index = xoNativeService.xoStringNmIndexOf(handle, attributeName, (String) value);
        }
        return index >= 0;
    }

    public Object get(int index) {
        String attributePath = attributeName + "[" + index + "]";
        Object result = null;
        if (ofType == TYPE_XO_OBJECT) {
            long objectHandle = xoNativeService.xoObjectNmGetAttr(handle, attributePath);
            if (objectHandle != 0) {
                result = new XoObjectImpl(xoNativeService, objectHandle, obix);
            }
        } else {
            result = xoNativeService.xoStringNmGetAttr(handle, attributePath);
        }
        if (result == null) {
            throw new IndexOutOfBoundsException("Index " + index + " is out of bound in the " + attributeName + " list object");
        }
        return result;
    }

    public int indexOf(Object value) {
        int index = 0;
        if (ofType == TYPE_XO_OBJECT) {
            if (value.getClass() == XoObjectImpl.class) {
                index = xoNativeService.xoObjectNmIndexOf(handle, attributeName, ((XoObjectImpl) value).getHandle());
            }
        } else if (value.getClass() == String.class) {
            index = xoNativeService.xoStringNmIndexOf(handle, attributeName, (String) value);
        }
        return (index >= 0) ? index : -1;
    }

    public boolean isEmpty() {
        return xoNativeService.xoNmNbInAttr(handle, attributeName) == 0;
    }

    public Iterator iterator() {
        currentIndex = xoNativeService.xoNmFirstAttr(handle, attributeName);
        return this;
    }

    public boolean remove(Object value) {
        int index = indexOf(value);
        if (index >= 0) {
            return remove(index) != null;
        }
        return false;
    }

    public Object remove(int index) {
        xoNativeService.xoNmSupInAttr(handle, attributeName + "[" + index + "]");
        return null;
    }

    public int size() {
        return xoNativeService.xoNmNbInAttr(handle, attributeName);
    }

    public boolean hasNext() {
        return currentIndex > 0;
    }

    public Object next() {
        Object result = null;
        if (ofType == TYPE_XO_OBJECT) {
            long objectHandle = xoNativeService.xoObjectNmGetAttr(handle, attributeName);
            if (objectHandle != 0) {
                result = new XoObjectImpl(xoNativeService, objectHandle, obix);
            }
        } else {
            result = xoNativeService.xoStringNmGetAttr(handle, attributeName);
        }
        currentIndex = xoNativeService.xoNmNextAttr(handle, attributeName);
        if (result == null) {
            throw new NoSuchElementException();
        }
        return result;
    }

    public void add(int arg0, Object arg1) {
        throw new UnsupportedOperationException();
    }

    public boolean addAll(int arg0, Collection arg1) {
        throw new UnsupportedOperationException();
    }

    public boolean containsAll(Collection arg0) {
        throw new UnsupportedOperationException();
    }

    public int lastIndexOf(Object arg0) {
        throw new UnsupportedOperationException();
    }

    public ListIterator listIterator() {
        throw new UnsupportedOperationException();
    }

    public ListIterator listIterator(int arg0) {
        throw new UnsupportedOperationException();
    }

    public boolean removeAll(Collection arg0) {
        throw new UnsupportedOperationException();
    }

    public boolean retainAll(Collection arg0) {
        throw new UnsupportedOperationException();
    }

    public Object set(int arg0, Object arg1) {
        throw new UnsupportedOperationException();
    }

    public List subList(int arg0, int arg1) {
        throw new UnsupportedOperationException();
    }

    public Object[] toArray() {
        throw new UnsupportedOperationException();
    }

    public Object[] toArray(Object[] arg0) {
        throw new UnsupportedOperationException();
    }

    public void remove() {
        throw new UnsupportedOperationException();
    }

}
