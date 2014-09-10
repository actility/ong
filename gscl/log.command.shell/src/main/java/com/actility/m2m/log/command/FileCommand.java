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
 * id $Id: FileCommand.java 8752 2014-05-21 15:21:42Z JReich $
 * author $Author: JReich $
 * version $Revision: 8752 $
 * lastrevision $Date: 2014-05-21 17:21:42 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:21:42 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.log.command;

import java.io.IOException;
import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public final class FileCommand implements Command {
    private final ResourcesAccessorService resourcesAccessorService;
    private final BundleContext bundleContext;

    public FileCommand(ResourcesAccessorService resourcesAccessorService, BundleContext bundleContext) {
        this.resourcesAccessorService = resourcesAccessorService;
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "logconfig-file";
    }

    public String getUsage() {
        return "logconfig-file [-maxSize #maxSize#] [-maxNb #maxNb#] [-flush | -noflush]\n"
                + "-maxSize #maxSize# Set the maximum size of one log file (characters)\n"
                + "-maxNb #maxNb#     Set the number of log file generations that are kept\n"
                + "-flush             Turns on log file flushing after each log entry\n"
                + "-noflush           Turns off log file flushing after each log entry\n";
    }

    public String getShortDescription() {
        return "Configures the file logging (the no argument version prints the current settings)";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        ServiceReference serviceReference = bundleContext.getServiceReference(ConfigurationAdmin.class.getName());
        ConfigurationAdmin configAdmin = (ConfigurationAdmin) bundleContext.getService(serviceReference);
        try {
            if (configAdmin != null) {
                LogConfiguration config = new LogConfiguration(resourcesAccessorService, bundleContext, configAdmin);

                // Parse command line.
                StringTokenizer st = new StringTokenizer(line, " ");

                // Ignore the command name.
                st.nextToken();

                // Check for optional argument.
                boolean optionFound = false;
                if (st.countTokens() >= 1) {
                    while (st.hasMoreTokens()) {
                        String token = st.nextToken().trim();
                        if (token.equals("-flush")) {
                            optionFound = true;
                            config.setForceFlush(true);
                        } else if (token.equals("-noflush")) {
                            optionFound = true;
                            config.setForceFlush(false);
                        } else if (token.equals("-size")) {
                            if (st.hasMoreElements()) {
                                optionFound = true;
                                config.setMaxFileSize(Integer.parseInt(st.nextToken()));
                            } else {
                                out.print(getUsage());
                                out.flush();
                                return;
                            }
                        } else if (token.equals("-gen")) {
                            if (st.hasMoreElements()) {
                                optionFound = true;
                                config.setMaxNbOfFiles(Integer.parseInt(st.nextToken()));
                            } else {
                                out.print(getUsage());
                                out.flush();
                                return;
                            }
                        }
                    }
                }

                if (optionFound) {
                    // Create persistent CM-config
                    config.save();
                } else {
                    // Show current config
                    out.println("  max file size  :    " + config.getMaxFileSize());
                    out.println("  max nb of files:  " + config.getMaxNbOfFiles());
                    out.println("  force flush:        " + config.getForceFlush());
                    out.println("  log location: " + config.getLogDirectory());
                }
            }
        } catch (NumberFormatException e) {
            out.println("Bad integer argument: " + e.getMessage());
            out.print(getUsage());
            out.flush();
            return;
        } catch (IOException e) {
            out.println("Internal error: " + e.getMessage());
            out.flush();
            return;
        } finally {
            bundleContext.getServiceReference(ConfigurationAdmin.class.getName());
        }
    }
}
