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

package com.actility.m2m.cm.command;

import java.util.Vector;

import org.apache.felix.shell.Command;
import org.apache.felix.shell.Session;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.Filter;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

public class Activator implements BundleActivator {
    /***************************************************************************
     * Key in the session properties dictionary used to store the current (open) configuration.*
     **************************************************************************/
    public static final String CURRENT = "com.actility.cm.command.impl.current";

    /***************************************************************************
     * Key in the session properties dictionary used to store the dictionary that is edited for the current configuration.*
     **************************************************************************/
    public static final String EDITED = "com.actility.cm.command.impl.edited";

    /***************************************************************************
     * Key in the session properties dictionary used to store the * result of the latest list command for later reference using
     * -i options to several commands. *
     **************************************************************************/
    public static final String LISTED_CONFIGS = "com.actility.cm.command.impl.listed.configs";

    private BundleContext bundleContext;
    private ServiceRegistration createRegistration;
    private ServiceRegistration currentRegistration;
    private ServiceRegistration deleteRegistration;
    private ServiceRegistration editRegistration;
    private ServiceRegistration listRegistration;
    private ServiceRegistration saveRegistration;
    private ServiceRegistration setRegistration;
    private ServiceRegistration showRegistration;
    private ServiceRegistration unsetRegistration;

    public void start(BundleContext context) throws Exception {
        bundleContext = context;
        createRegistration = context.registerService(Command.class.getName(), new CreateCommand(bundleContext), null);
        currentRegistration = context.registerService(Command.class.getName(), new CurrentCommand(bundleContext), null);
        deleteRegistration = context.registerService(Command.class.getName(), new DeleteCommand(bundleContext), null);
        editRegistration = context.registerService(Command.class.getName(), new EditCommand(bundleContext), null);
        listRegistration = context.registerService(Command.class.getName(), new ListCommand(bundleContext), null);
        saveRegistration = context.registerService(Command.class.getName(), new SaveCommand(bundleContext), null);
        setRegistration = context.registerService(Command.class.getName(), new SetCommand(bundleContext), null);
        showRegistration = context.registerService(Command.class.getName(), new ShowCommand(bundleContext), null);
        unsetRegistration = context.registerService(Command.class.getName(), new UnsetCommand(bundleContext), null);
    }

    public void stop(BundleContext context) throws Exception {
        createRegistration.unregister();
        currentRegistration.unregister();
        deleteRegistration.unregister();
        editRegistration.unregister();
        listRegistration.unregister();
        saveRegistration.unregister();
        setRegistration.unregister();
        showRegistration.unregister();
        unsetRegistration.unregister();
    }

    public static Configuration[] getConfigurations(BundleContext context, Session session, ConfigurationAdmin cm,
            String[] selection) throws Exception {
        Filter[] filters = convertToFilters(context, session, selection);
        return getConfigurationsMatchingFilters(cm, filters);
    }

    private static Filter[] convertToFilters(BundleContext context, Session session, String[] selection) throws Exception {
        if (selection == null) {
            return null;
        }
        Filter[] filters = new Filter[selection.length];
        for (int i = 0; i < selection.length; ++i) {
            String current = selection[i];
            Filter filter = null;
            if (isInteger(current)) {
                filter = tryToCreateFilterFromIndex(context, session, current);
            } else if (current.startsWith("(")) {
                filter = tryToCreateFilterFromLdapExpression(context, current);
            } else {
                filter = tryToCreateFilterFromPidContainingWildcards(context, current);
            }
            if (filter == null) {
                throw new Exception("Unable to handle selection argument " + current);
            }
            filters[i] = filter;
        }
        return filters;
    }

    private static Configuration[] getConfigurationsMatchingFilters(ConfigurationAdmin cm, Filter[] filters) throws Exception {
        Configuration[] cs = cm.listConfigurations(null);
        if (cs == null || cs.length == 0) {
            return new Configuration[0];
        }
        if (filters == null || filters.length == 0) {
            return cs;
        }

        Vector matching = new Vector();
        for (int i = 0; i < cs.length; ++i) {
            for (int j = 0; j < filters.length; ++j) {
                if (filters[j].matchCase(cs[i].getProperties())) {
                    matching.addElement(cs[i]);
                    break;
                }
            }
        }

        Configuration[] result = new Configuration[matching.size()];
        matching.copyInto(result);
        return result;
    }

    private static boolean isInteger(String possiblyAnInteger) {
        try {
            Integer.parseInt(possiblyAnInteger);
        } catch (NumberFormatException e) {
            return false;
        }
        return true;
    }

    private static Filter tryToCreateFilterFromIndex(BundleContext context, Session session, String index) throws Exception {
        String pid = getPidWithIndexInLastList(session, index);
        return tryToCreateFilterFromPidContainingWildcards(context, pid);
    }

    private static Filter tryToCreateFilterFromPidContainingWildcards(BundleContext context, String pidContainingWildcards)
            throws Exception {
        return tryToCreateFilterFromLdapExpression(context, "(" + Constants.SERVICE_PID + "=" + pidContainingWildcards + ")");
    }

    private static Filter tryToCreateFilterFromLdapExpression(BundleContext context, String ldapExpression) throws Exception {
        return context.createFilter(ldapExpression);
    }

    private static String getPidWithIndexInLastList(Session session, String index) throws Exception {
        Configuration[] cs = (Configuration[]) session.getAttribute(Activator.LISTED_CONFIGS);
        if (cs == null) {
            throw new Exception("The 'list' command has not been used yet to create a list.");
        }
        if (cs.length == 0) {
            throw new Exception("No configurations listed by latest 'list' call.");
        }
        int i = Integer.parseInt(index);
        if (i < 0 || cs.length <= i) {
            throw new Exception("Invalid index."
                    + ((cs.length == 1) ? "0 is the only valid index." : ("Valid indices are 0 to " + (cs.length - 1))));
        }

        String pid = cs[i].getPid();
        if (pid == null) {
            throw new Exception("Unable to retrieve pid with index " + index + " from last 'list'.");
        }

        return pid;
    }
}
