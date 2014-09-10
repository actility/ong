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
 * id $Id: LongPollingServersCleaner.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.servlet.song.http;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;


/**
 * Thread responsible to detect the long polling requests expirations.
 * <p>
 * It does so by checking every seconds the list of ordered long polling requests (in reception order)
 */
public final class LongPollingServersCleaner extends Thread {
    private static final Logger LOG = OSGiLogger.getLogger(LongPollingServersCleaner.class, BundleLogger.LOG);

    private boolean end;
    private final SongHttpBinding songHttpBinding;

    /**
     * Creates the long polling server cleaner
     *
     * @param songHttpBinding The SONG HTTP binding which effectively hold the list of long polling servers
     */
    public LongPollingServersCleaner(SongHttpBinding songHttpBinding) {
        super("Long polling servers cleaner");
        this.songHttpBinding = songHttpBinding;
        this.end = false;
    }

    /**
     * Stops the long polling servers cleaner
     */
    public void end() {
        end = true;
        interrupt();
    }

    public void run() {
        while (!end) {
            long startTime = 0;
            long now = 0;
            do {
                LOG.debug("Check long polling server expiration");
                startTime = System.currentTimeMillis();
                songHttpBinding.checkWaitingLongPollingServersForExpiration(startTime);
                now = System.currentTimeMillis();
            } while ((startTime + 1000 - now) < 100);
            try {
                long duration = startTime + 1000 - now;
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Sleep for " + duration + "ms");
                }
                Thread.sleep(duration);
            } catch (InterruptedException e) {
                end = true;
            }
        }
    }
}
