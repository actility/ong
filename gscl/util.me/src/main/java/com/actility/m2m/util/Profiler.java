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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.Writer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Profile module to detect slow running fucntions
 */
public final class Profiler {
    private static final Profiler INSTANCE = new Profiler();
    private static final DateFormat dateTimeFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS");

    private File traceOutputFile;
    private Writer traceWriter;

    private Profiler() {
        // Singleton
        traceOutputFile = new File("traces.log");
        traceWriter = null;
    }

    /**
     * Gets the profiler singleton instance.
     *
     * @return The profiler singleton instance
     */
    public static Profiler getInstance() {
        return INSTANCE;
    }

    /**
     * Trace a message to a trace file together with the current date
     * <p>
     * @param message The message to print in the profile file
     */
    public void trace(String message) {
        if (traceWriter != null) {
            // Write message
            String now = dateTimeFormat.format(new Date());
            synchronized (this) {
                if (traceWriter != null) {
                    try {
                        traceWriter.write(now);
                        traceWriter.write(' ');
                        traceWriter.write(message);
                        traceWriter.write('\n');
                    } catch (Exception e) {
                        // Discard error silently
                    }
                }
            }
        }
    }

    /**
     * Checks whether the trace are activated
     *
     * @return Whether traces are activated
     */
    public boolean isTraceEnabled() {
        return traceWriter != null;
    }

    /**
     * Activates or deactivates profiler traces.
     *
     * @param traceEnabled Whether to activate traces
     */
    public synchronized void setTraceEnabled(boolean traceEnabled) {
        if (traceEnabled && traceWriter == null) {
            try {
                FileWriter fstream = new FileWriter(traceOutputFile);
                traceWriter = new BufferedWriter(fstream);
            } catch (Exception e) {
                // Discard error silently
            }
        } else if (!traceEnabled && traceWriter != null) {
            try {
                traceWriter.close();
            } catch (Exception e) {
                // Discard error silently
            }
        }
    }

    /**
     * Gets the file in which profiler traces are written
     *
     * @return The file in which profiler traces are written
     */
    public File getTraceOutputFile() {
        return traceOutputFile;
    }

    /**
     * Sets the file in which profiler traces are written
     *
     * @param traceOuputFile The file in which profiler traces are written
     */
    public synchronized void setTraceOutputFile(File traceOuputFile) {
        if (traceOuputFile == null) {
            return;
        }
        if (!this.traceOutputFile.equals(traceOuputFile) && traceWriter != null) {
            try {
                traceWriter.close();
                FileWriter fstream = new FileWriter(traceOutputFile);
                traceWriter = new BufferedWriter(fstream);
            } catch (Exception e) {
                // Discard error silently
            }
        }
        this.traceOutputFile = traceOuputFile;
    }
}
