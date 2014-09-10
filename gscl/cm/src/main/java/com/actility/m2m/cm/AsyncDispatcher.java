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
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.cm;

import org.apache.log4j.Logger;

import com.actility.m2m.cm.log.BundleLogger;
import com.actility.m2m.util.concurrent.CircularArray;
import com.actility.m2m.util.log.OSGiLogger;

public final class AsyncDispatcher implements Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(AsyncDispatcher.class, BundleLogger.getStaticLogger());

    /**
     * * The queue of updates.
     */
    private final CircularArray/* <Runnable> */queue;

    private boolean end;

    /**
     * * The thread running this object.
     */
    private Thread thread;

    public AsyncDispatcher() {
        this.end = false;
        this.thread = null;
        this.queue = new CircularArray/* <Runnable> */(32);
    }

    /**
     * Override of Thread.run().
     */
    public void run() {
        boolean localEnd = false;
        while (!end && !localEnd) {
            try {
                Runnable runnable = (Runnable) queue.take(5000L);
                if (runnable == null) {
                    detachCurrentThread();
                    localEnd = true;
                } else {
                    runnable.run();
                }
            } catch (RuntimeException e) {
                LOG.error("Error while delivering async message", e);
            } catch (InterruptedException e) {
                LOG.error("Error while delivering async message", e);
            }
        }
    }

    public void send(Runnable runnable) {
        queue.offer(runnable);
        attachNewThreadIfNeccesary();
    }

    public synchronized void stop() {
        end = true;
        if (thread != null) {
            thread.interrupt();
            try {
                thread.join();
            } catch (InterruptedException e) {
                LOG.error("Received an interrupted exception while waiting for async thread completion");
            }
        }
    }

    private synchronized void attachNewThreadIfNeccesary() {
        if (thread == null) {
            thread = new Thread(this);
            thread.start();
        }
    }

    private synchronized void detachCurrentThread() {
        thread = null;
    }
}
