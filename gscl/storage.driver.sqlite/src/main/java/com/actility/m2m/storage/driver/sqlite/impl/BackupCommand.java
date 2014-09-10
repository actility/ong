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

package com.actility.m2m.storage.driver.sqlite.impl;

import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.apache.log4j.Logger;

import com.actility.m2m.storage.driver.sqlite.log.BundleLogger;
import com.actility.m2m.storage.driver.sqlite.ni.SqliteDriverNIService;
import com.actility.m2m.util.log.OSGiLogger;


public class BackupCommand implements Command
{

    private PrintStream out = null;

    private SqliteDriverNIService driver;

    private static final Logger LOG = OSGiLogger.getLogger(BackupCommand.class, BundleLogger.getStaticLogger());

    public BackupCommand(SqliteDriverNIService driver)
    {
        this.driver =  driver;
    }

    public String getName()
    {
        return "backup";
    }

    public String getUsage()
    {
        return "backup <path_filename_to_backup> [<pages per step (default 5)> <sleep interval between step (default 250 ms)>]";
    }

    public String getShortDescription()
    {
        return "backup sqlite database.";
    }

    public void execute(String s, PrintStream out, PrintStream err)
    {
        StringTokenizer st = new StringTokenizer(s, " ");

        // Ignore the command name.
        st.nextToken();
        this.out = out;
        // There should be at least file name .
        if (st.countTokens() >= 1)
        {
            if (st.hasMoreTokens()) {
                String  filePath  = st.nextToken().trim();
                int pages = 0;
                int interval = 0;
                if (st.hasMoreTokens()) {
                    try {
                        int p = Integer.valueOf(st.nextToken().trim()).intValue();
                        if (p > 0 ) {
                            pages = p;
                        }
                    } catch (NumberFormatException e)
                    {
                        LOG.error("Incorrect pages per step" + e.getMessage());
                        err.println("Incorrect parameter 'pages per step'");
                    }
                }
                if (st.hasMoreTokens()) {
                    try {
                        int p = Integer.valueOf(st.nextToken().trim()).intValue();
                        if (p > 0 ) {
                            interval = p;
                        }
                    } catch (NumberFormatException e)
                    {
                        LOG.error("Incorrect sleep interval between step" + e.getMessage());
                        err.println("Incorrect parameter 'sleep interval'");
                    }
                }

                //try {
                    if (LOG.isInfoEnabled())
                    {
                        LOG.info("Start backup...please wait");
                    }
                    out.println("Backup started..");
                    if (pages == 0 || interval == 0)
                    {
                        driver.backupDb(filePath, 5, 250);
                    } else
                    {
                        driver.backupDb(filePath, pages, interval);
                    }

                    if (LOG.isInfoEnabled())
                    {
                        LOG.info("Backup is completed");
                    }
                    out.println("Backup is completed.");
               // } catch (StorageException e)
               // {
               //     LOG.error("Cannot make a backup "+e.getMessage());
               //     err.println("Backup fail.");
               // }

            }
        }
        else
        {
            err.println("Incorrect number of arguments");
        }
        this.out = null;
    }


    public void callback(String log)
    {

        if (this.out != null) {
            this.out.print(log);

        }
    }
}
