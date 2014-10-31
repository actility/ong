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

package com.actility.m2m.log.impl;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.Date;

import org.osgi.framework.BundleContext;
import org.osgi.service.log.LogEntry;
import org.osgi.service.log.LogListener;

import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;

public final class FileLog implements LogListener {
    /** Base name of log files. */
    private static final String FILE_PREFIX = "osgi_";
    /** Suffix for old log files. */
    private static final String LOG_FILE_SUFFIX = ".log";
    /** Init file name. */
    private static final String FILE_START_NAME = FILE_PREFIX + "start" + LOG_FILE_SUFFIX;
    /** Suffix for old log files. */
    private static final String OLD_FILE_SUFFIX = ".old";

    /** Directory where the log files are stored. */
    private final File logDir;

    // ********** Instance data **********
    /**
     * PrintWriter for current log file. Is <code>null</code> if opening of the log file failed.
     */
    private PrintWriter log;

    private int currentFileSize;
    private int maxNbOfFiles;

    /** Handle to the configuration object */
    private final LogConfiguration configuration;

    /**
     * The constructor creates a file log object base on configuration data. It will initialize a new empty log file.
     *
     * @param context handle to the framework.
     * @param configuration the log configuration to use.
     */
    public FileLog(BundleContext context, LogConfiguration configuration) {
        this.configuration = configuration;
        this.logDir = configuration.getLogDirectory();
        this.maxNbOfFiles = configuration.getMaxNbOfFiles();

        if (this.logDir == null) {
            throw new IllegalArgumentException("Log directory must be defined");
        }

        // Open a new log, if possible
        copyLogsAndRemoveOld();
        openLog();
    }

    /**
     * * The stop method is called by the log reader service factory when * the log bundle is stoped. Flush data and close the
     * current log file.
     */
    public void stop() {
        if (log != null) {
            log.flush();
            log.close();
            log = null;
        }
    }

    /**
     * A new log entry has arrived write it to the log file.
     *
     * @param logEntry The new LogEntry
     */
    public void logged(LogEntry logEntry) {
        if (log != null) {
            String s = logEntry.toString();
            if (currentFileSize + s.length() > configuration.getMaxFileSize()) {
                log.close();
                rotateLogFiles();
                openLog();
            }
            currentFileSize += s.length();
            log.println(s);
            if (configuration.getForceFlush()) {
                log.flush();
            }
        }
    }

    /**
     * Open and initialize a new log file. Make sure that <code>saveOldGen()</code> or <code>savePreviousLog()</code> has been
     * called before this method is called. I.e. make sure that the file <code>LOGBASE0</code> in <code>logdir</code> is
     * available for writing.
     */
    private void openLog() {
        File logfile = new File(logDir, FILE_PREFIX + "0.log");
        try {
            FileOutputStream fos = new FileOutputStream(logfile);
            OutputStreamWriter osw = new OutputStreamWriter(fos);
            BufferedWriter bw = new BufferedWriter(osw);
            log = new PrintWriter(bw);
            String s = "Log started " + FormatUtils.formatDateTime(new Date(), UtilConstants.LOCAL_TIMEZONE);
            currentFileSize = s.length() + (int) logfile.length();
            log.println(s);
            log.flush();
        } catch (IOException e) {
            System.err.println("Failed to open logfile " + logfile + " due to: " + e.getMessage());
            log = null;
        }
    }

    /**
     * Copies current log files and remove old ones
     */
    private void copyLogsAndRemoveOld() {
        String[] files = logDir.list();
        Arrays.sort(files);
        for (int i = files.length - 1; i >= 0; i--) {
            if (files[i].startsWith(FILE_PREFIX)) {
                if (files[i].endsWith(OLD_FILE_SUFFIX)) {
                    new File(logDir, files[i]).delete();
                } else if (files[i].endsWith(LOG_FILE_SUFFIX)) {
                    new File(logDir, files[i]).renameTo(new File(logDir, files[i] + OLD_FILE_SUFFIX));
                }
            }
        }
    }

    /**
     * Set the number of log file generations.
     *
     * @param oldMaxNbOfFiles the old generation count value.
     * @param maxNbOfFiles the new generation count value.
     */
    private void resetMaxNbOfFiles(int maxNbOfFiles, int oldMaxNbOfFiles) {
        if (maxNbOfFiles < 1) {
            maxNbOfFiles = 1;
        }
        if (maxNbOfFiles != oldMaxNbOfFiles) {
            if (oldMaxNbOfFiles > maxNbOfFiles) { // Remove excessive files
                trimMaxNbOfFiles(maxNbOfFiles);
            }
        }
        this.maxNbOfFiles = maxNbOfFiles;
    }

    /**
     * Rename log files one step.
     */
    private void rotateLogFiles() {
        int maxNbOfFiles = configuration.getMaxNbOfFiles();
        if (maxNbOfFiles != this.maxNbOfFiles) {
            resetMaxNbOfFiles(maxNbOfFiles, this.maxNbOfFiles);
        }
        File startFile = new File(FILE_START_NAME);
        if (!startFile.exists()) {
            File src = new File(logDir, FILE_PREFIX + "0" + LOG_FILE_SUFFIX);
            src.renameTo(startFile);
        }
        for (int i = maxNbOfFiles - 1; i > 0; i--) {
            File dst = new File(logDir, FILE_PREFIX + i + LOG_FILE_SUFFIX);
            File src = new File(logDir, FILE_PREFIX + (i - 1) + LOG_FILE_SUFFIX);
            if (dst.exists()) {
                dst.delete();
            }
            src.renameTo(dst);
        }
    }

    /**
     * Delete all log files above given generation
     *
     * @param maxNbOfFiles is the first file to remove
     */
    private void trimMaxNbOfFiles(int maxNbOfFiles) {
        // Delete old logs
        boolean done = false;
        for (int i = maxNbOfFiles; !done; i++) {
            File src = new File(logDir, FILE_PREFIX + i + LOG_FILE_SUFFIX);
            if (src.exists()) {
                src.delete();
            } else {
                done = true;
            }
        }
    }
}
