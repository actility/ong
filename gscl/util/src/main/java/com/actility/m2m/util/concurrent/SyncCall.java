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
 * id $Id: SyncCall.java 6057 2013-10-14 11:50:13Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6057 $
 * lastrevision $Date: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 13:50:13 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.util.concurrent;

/**
 * Allows to synchronize two threads at a given point and to exchange an object.
 * <p>
 * The threads which waits the objetc to exchage is called the consumer thread while the other is the producer thread.
 */
public final class SyncCall {
    private final Object lock;
    private Object object;
    private boolean resumed = false;
    private boolean pending = false;

    /**
     * Builds a SyncCall which uses the current object as a lock.
     */
    public SyncCall() {
        lock = this;
    }

    /**
     * Builds a SyncCall which uses the given lock.
     *
     * @param lock The lock to use for the SynCall object
     */
    public SyncCall(Object lock) {
        this.lock = (lock == null) ? this : lock;
    }

    /**
     * Notify the cosumer thread that the producer thread has reached the meeting point and that the object to exchange is
     * available.
     */
    public void resume() {
        synchronized (lock) {
            resumed = true;
            lock.notify();
        }
    }

    /**
     * Resets the SyncCall for restartig the whole process.
     */
    public void reset() {
        synchronized (lock) {
            resumed = false;
            pending = false;
            lock.notify();
        }
    }

    /**
     * Suspends the consumer thread for the given amount of time or until the producer thread notifies it through
     * {@link #resume()}.
     *
     * @param timeout The maximum amout of time to wait for the producer thread to call the {@link #resume()} method
     * @return Whether the producer thread has called the {@link #resume()} method which means the object to exchange is
     *         available
     * @throws InterruptedException If the consumer thread is interrupted while waiting for the producer thread
     */
    public boolean suspend(long timeout) throws InterruptedException {
        synchronized (lock) {
            pending = true;
            boolean result;
            try {
                if (!resumed && timeout >= 0) {
                    if (timeout == 0) {
                        lock.wait();
                    } else if (timeout > 0) {
                        lock.wait(timeout);
                    }
                }
            } finally {
                result = resumed;
                resumed = false;
                pending = false;
            }

            return result;
        }
    }

    /**
     * Whether the consumer thread is waiting for the producer thread to send the object to exchange.
     *
     * @return Whether the consumer thread is waiting for the producer thread
     */
    public boolean isPending() {
        synchronized (lock) {
            return pending;
        }
    }

    /**
     * Whether the producer thread has sent the object to exchange to the consumer thread.
     *
     * @return Whether the producer thread has sent the object to exchange to the consumer thread
     */
    public boolean isResumed() {
        synchronized (lock) {
            return resumed;
        }
    }

    /**
     * Gets the object to exchange that the producer thread has sent or null if none.
     *
     * @return The object to exchange
     */
    public Object getObject() {
        return object;
    }

    /**
     * Sets the object to exchange that the producer thread wants to send.
     *
     * @param object The object to exchange
     */
    public void setObject(Object object) {
        this.object = object;
    }
}
