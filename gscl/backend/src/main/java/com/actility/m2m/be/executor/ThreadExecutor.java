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
 * id $Id: ThreadExecutor.java 8754 2014-05-21 15:23:48Z JReich $
 * author $Author: JReich $
 * version $Revision: 8754 $
 * lastrevision $Date: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.be.executor;

import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Implementation of a thread for {@link ThreadPoolExecutor}. It reads message exchanges from the global thread pool queue and
 * send it to the targeted {@link BackendEndpoint}.
 *
 */
public final class ThreadExecutor extends Thread {
    private static final Logger LOG = OSGiLogger.getLogger(ThreadExecutor.class, BundleLogger.getStaticLogger());

    private boolean end;
    private final long id;
    private final ThreadPoolExecutor threadPoolExecutor;

    /**
     * Builds a {@link ThreadExecutor}.
     *
     * @param id The unique identifier of the thread within the thread pool
     * @param threadPoolExecutor The thread pool in which the thread will reside
     */
    public ThreadExecutor(long id, ThreadPoolExecutor threadPoolExecutor) {
        super("BPool-" + id);
        this.threadPoolExecutor = threadPoolExecutor;
        this.id = id;
        this.end = false;
    }

    public void stopExecutor() {
        end = true;
        interrupt();
        try {
            join();
        } catch (InterruptedException e) {
            LOG.error("Thread interupted while joining the thread " + id + " from the thread pool");
        }
    }

    public void run() {
        BackendRunnable runnable = null;
        while (!end) {
            try {
                // Wait task
                runnable = threadPoolExecutor.getRunnable(getName());
                if (runnable != null) {
                    runnable.run();
                    threadPoolExecutor.putRunnable(runnable);
                } else if (threadPoolExecutor.removeThread(this)) {
                    end = true;
                }
            } catch (InterruptedException e) {
                end = true;
            } catch (RuntimeException e) {
                LOG.error("RuntimeException while executing task", e);
            }
        }
    }
}
