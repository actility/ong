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
 * id $Id: CircularArray.java 6057 2013-10-14 11:50:13Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6057 $
 * lastrevision $Date: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.util.concurrent;

/**
 * A circular array use in a producer / consumer pattern.
 */
public final class CircularArray {
    private final Object[] array;
    private int readIndex;
    private int writeIndex;

    /**
     * Builds the circular array with the given size. The size cannot be changed afterwards.
     *
     * @param capacity The capacity of the circular array
     */
    public CircularArray(int capacity) {
        if (capacity < 2) {
            throw new IllegalArgumentException("Capacity cannot be lower than 2");
        }
        array = new Object[capacity + 1];
        readIndex = 0;
        writeIndex = 0;
    }

    /**
     * Offers a data object to the circular array. Returns directly if the array is full.
     *
     * @param o The data to add to the circular array
     * @return Whether the data has been added
     */
    public synchronized boolean offer(Object o) {
        boolean inserted = false;
        int newIndex = (writeIndex + 1) % array.length;
        if (newIndex != readIndex) {
            array[writeIndex] = o;
            writeIndex = newIndex;
            notify();
            inserted = true;
        }
        return inserted;
    }

    /**
     * Takes a data object from the circular array. This call is blocking until a data gets available.
     *
     * @return The taken object
     * @throws InterruptedException If the thread is interrupted while waiting for the data object
     */
    public synchronized Object take() throws InterruptedException {
        Object result = null;
        while (true) {
            if (readIndex != writeIndex) {
                result = array[readIndex];
                array[readIndex] = null;
                readIndex = (readIndex + 1) % array.length;
                return result;
            }
            wait();
        }
    }

    /**
     * Takes a data object from the circular array. This call is blocking until a data gets available.
     *
     * @param timeout Maximum time to wait for a value
     * @return The taken object or null if no object is available in the given time
     * @throws InterruptedException If the thread is interrupted while waiting for the data object
     */
    public synchronized Object take(long timeout) throws InterruptedException {
        long startTime = System.currentTimeMillis();
        long waitTime = timeout;
        Object result = null;
        while (true) {
            if (readIndex != writeIndex) {
                result = array[readIndex];
                array[readIndex] = null;
                readIndex = (readIndex + 1) % array.length;
                return result;
            }
            if (waitTime > 0) {
                wait(waitTime);
                waitTime = timeout - (System.currentTimeMillis() - startTime);
            } else {
                return null;
            }
        }
    }

    /**
     * Poll a data object from the circular array. This call returns null if the queue is empty.
     *
     * @return The polled object or null if the queue is empty
     */
    public synchronized Object poll() {
        Object result = null;
        if (readIndex != writeIndex) {
            result = array[readIndex];
            array[readIndex] = null;
            readIndex = (readIndex + 1) % array.length;
        }
        return result;
    }

    /**
     * Returns the circular array capacity
     *
     * @return The circular array capacity
     */
    public int getSize() {
        return array.length - 1;
    }

    /**
     * Returns the circular array elements count
     *
     * @return The circular array elements count
     */
    public synchronized int getCount() {
        return (writeIndex - readIndex + array.length) % array.length;
    }
}
