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

package com.actility.m2m.be.executor;

import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.BackendExecutor;
import com.actility.m2m.be.log.BundleLogger;
import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.util.concurrent.ExternalCircularArray;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Executor that starts a pool of Threads which treat message exchanges in parallel. The thread poll executor also holds a
 * global queue where exchanges are stored in case no thread is available for execution.
 *
 */
public final class ThreadPoolExecutor extends Thread implements BackendExecutor {
    private static final Logger LOG = OSGiLogger.getLogger(ThreadPoolExecutor.class, BundleLogger.getStaticLogger());

    private static final int CRITICAL = 0;
    private static final int IMPORTANT = 1;
    private static final int NORMAL = 2;

    private static long NEXT_THREAD_ID = 0;

    private boolean closed;
    private final ThreadExecutor[] threadPool;
    private final BackendRunnable[] runnablePool;

    private final ExternalCircularArray[] runnablesQueues;

    private int threadCount;
    private int threadWaiting;
    private int runnableCount;
    private int minThreadCount;
    private long expiration;

    public ThreadPoolExecutor(int queueSize, int minimumSize, int maximumSize, long expiration) {
        super("BPoolCleaner");
        this.closed = false;
        this.expiration = expiration;

        threadPool = new ThreadExecutor[maximumSize];
        runnablePool = new BackendRunnable[queueSize * 3 + maximumSize];
        runnablesQueues = new ExternalCircularArray[3];
        runnablesQueues[CRITICAL] = new ExternalCircularArray(queueSize, this);
        runnablesQueues[IMPORTANT] = new ExternalCircularArray(queueSize, this);
        runnablesQueues[NORMAL] = new ExternalCircularArray(queueSize, this);

        threadWaiting = 0;
        threadCount = 0;
        minThreadCount = minimumSize;
        ThreadExecutor executor = null;
        while (threadCount < minimumSize) {
            executor = new ThreadExecutor(threadCount, this);
            threadPool[threadCount] = executor;
            executor.start();
            ++threadCount;
        }
        NEXT_THREAD_ID = threadCount;

        runnableCount = 0;
        for (int i = 0; i < runnablePool.length; ++i) {
            runnablePool[i] = new BackendRunnable();
        }
        runnableCount = runnablePool.length;
    }

    protected synchronized BackendRunnable getRunnable(String name) throws InterruptedException {
        if (LOG.isDebugEnabled()) {
            LOG.debug(name + ": Get next runnable: (active threads: " + threadCount + ") (waiting threads: " + threadWaiting
                    + ")");
        }
        BackendRunnable runnable = (BackendRunnable) runnablesQueues[CRITICAL].poll();
        long expiration = this.expiration;
        long beginWait = 0;
        while (runnable == null && expiration > 0) {
            runnable = (BackendRunnable) runnablesQueues[IMPORTANT].poll();
            if (runnable == null) {
                runnable = (BackendRunnable) runnablesQueues[NORMAL].poll();
                if (runnable == null) {
                    ++threadWaiting;
                    if (LOG.isDebugEnabled()) {
                        LOG.debug(name + ": Cannot peek runnable, wait for " + expiration + "ms: (active threads: "
                                + threadCount + ") (waiting threads: " + threadWaiting + ")");
                    }
                    beginWait = System.currentTimeMillis();
                    wait(expiration);
                    expiration -= (System.currentTimeMillis() - beginWait);
                    --threadWaiting;
                    runnable = (BackendRunnable) runnablesQueues[CRITICAL].poll();
                    if (runnable == null) {
                        runnable = (BackendRunnable) runnablesQueues[IMPORTANT].poll();
                        if (runnable == null) {
                            runnable = (BackendRunnable) runnablesQueues[NORMAL].poll();
                        }
                    }
                }
            }
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug(name + ": Get next runnable end: (active threads: " + threadCount + ") (waiting threads: "
                    + threadWaiting + ") (runnable: " + runnable + ")");
        }
        return runnable;
    }

    protected synchronized boolean removeThread(ThreadExecutor threadExecutor) {
        if (LOG.isDebugEnabled()) {
            LOG.debug(threadExecutor.getName() + ": Try to remove thread: (active threads: " + threadCount
                    + ") (waiting threads: " + threadWaiting + ")");
        }
        if (!closed) {
            ThreadExecutor[] threadPool = this.threadPool;
            int threadCount = this.threadCount;

            if (threadCount > minThreadCount) {
                for (int i = 0; i < threadCount; ++i) {
                    if (threadPool[i] == threadExecutor) {
                        if (i != (threadCount - 1)) {
                            // Removes hole in pool array by shifting the tail by 1
                            System.arraycopy(threadPool, i + 1, threadPool, i, threadCount - i - 1);
                        }
                        threadPool[threadCount - 1] = null;
                        --this.threadCount;
                        if (LOG.isInfoEnabled()) {
                            LOG.info(threadExecutor.getName() + ": Thread is removed: (active threads: " + threadCount
                                    + ") (waiting threads: " + threadWaiting + ")");
                        }
                        return true;
                    }
                }
            }
            if (LOG.isDebugEnabled()) {
                LOG.debug(threadExecutor.getName() + ": Thread is not removed: (active threads: " + threadCount
                        + ") (waiting threads: " + threadWaiting + ")");
            }
            return false;
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug(threadExecutor.getName() + ": Thread is not removed because service is closed: (active threads: "
                    + threadCount + ") (waiting threads: " + threadWaiting + ")");
        }
        return true;
    }

    protected synchronized boolean enqueueRunnable(BackendRunnable runnable, int priority) {
        if (LOG.isInfoEnabled()) {
            LOG.info(runnable + ": Enqueue runnable with priority " + priority);
        }
        if (!closed) {
            if (threadWaiting == 0 && threadCount < threadPool.length) {
                ThreadExecutor executor = new ThreadExecutor(NEXT_THREAD_ID, this);
                threadPool[threadCount] = executor;
                if (LOG.isInfoEnabled()) {
                    LOG.info(executor.getName() + ": No available thread in the pool for runnable " + runnable
                            + ", start a new one");
                }
                executor.start();
                ++NEXT_THREAD_ID;
                ++threadCount;
            }
            if (LOG.isDebugEnabled()) {
                LOG.debug(runnable + ": Offer runnable to queue");
            }
            return runnablesQueues[priority].offer(runnable);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(runnable + ": Cannot enqueue runnable because service is closed");
        }
        return false;
    }

    protected BackendRunnable peekRunnable() {
        BackendRunnable runnable = null;
        synchronized (runnablePool) {
            if (runnableCount != 0) {
                --runnableCount;
                runnable = runnablePool[runnableCount];
            }
        }

        return runnable;
    }

    protected void putRunnable(BackendRunnable runnable) {
        runnable.clear();
        synchronized (runnablePool) {
            runnablePool[runnableCount] = runnable;
            ++runnableCount;
        }
    }

    public void send(MessageExchange exchange, BackendEndpoint destination, int priority) throws MessagingException {
        BackendRunnable runnable = peekRunnable();
        boolean inserted = false;
        if (runnable != null) {
            runnable.init(exchange, destination);
            inserted = enqueueRunnable(runnable, priority);
            if (!inserted) {
                putRunnable(runnable);
                throw new MessagingException("Backend queue is full for priority " + priority + ". Discard current exchange",
                        true);
            }
        } else {
            LOG.error("No more backend task available. Discard current exchange");
        }
    }

    public void destroy() {
        synchronized (this) {
            closed = true;
        }
        for (int i = 0; i < threadCount; ++i) {
            threadPool[i].stopExecutor();
            threadPool[i] = null;
        }
        threadCount = 0;
    }

    public int getQueueSize() {
        return runnablesQueues[CRITICAL].getSize();
    }

    public void setMinThreads(int minPoolThreads) {
        minThreadCount = minPoolThreads;
    }

    public int getMaxThreads() {
        return threadPool.length;
    }

    public void setThreadExpiration(long poolThreadExpiration) {
        expiration = poolThreadExpiration;
    }
}
