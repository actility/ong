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
 * id $Id: ServicesCommand.java 8246 2014-03-21 13:16:40Z JReich $
 * author $Author: JReich $
 * version $Revision: 8246 $
 * lastrevision $Date: 2014-03-21 14:16:40 +0100 (Fri, 21 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-21 14:16:40 +0100 (Fri, 21 Mar 2014) $
 */

package com.actility.m2m.framework.command.me;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Vector;

import org.apache.felix.shell.Command;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

public class ServicesCommand implements Command {
    private BundleContext bundleContext;

    public ServicesCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "services";
    }

    public String getUsage() {
        return "services [-i] [-l] [-sid #id#] [-f #filter#] [-r] [-u] [<bundle>] ...\n" + "-i          Sort on bundle id\n"
                + "-l          Verbose output\n" + "-r          Show services registered by named bundles (default)\n"
                + "-sid #id#   Show the service with the specified service id\n"
                + "-u          Show services used by named bundles\n"
                + "-f #filter# Show all services that matches the specified filter\n" + "<bundle>    Name or id of bundle\n";
    }

    public String getShortDescription() {
        return "List registered services";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        boolean sortId = false;
        boolean verbose = false;
        boolean registeredByNames = false;
        boolean usedByNames = false;
        String serviceId = null;
        String filter = null;
        ArrayList argBundles = new ArrayList();
        if (st.countTokens() >= 1) {
            while (st.hasMoreTokens()) {
                String token = st.nextToken().trim();
                if (token.equals("-i")) {
                    sortId = true;
                } else if (token.equals("-l")) {
                    verbose = true;
                } else if (token.equals("-r")) {
                    registeredByNames = true;
                } else if (token.equals("-u")) {
                    usedByNames = true;
                } else if (token.equals("-sid")) {
                    if (st.hasMoreElements()) {
                        serviceId = st.nextToken().trim();
                    } else {
                        out.print(getUsage());
                        out.flush();
                        return;
                    }
                } else if (token.equals("-f")) {
                    if (st.hasMoreElements()) {
                        filter = st.nextToken().trim();
                    } else {
                        out.print(getUsage());
                        out.flush();
                        return;
                    }
                } else if (token.startsWith("-")) {
                    out.println("Unknown argument: " + token);
                    out.print(getUsage());
                    out.flush();
                    return;
                } else {
                    argBundles.add(token);
                    while (st.hasMoreTokens()) {
                        argBundles.add(st.nextToken().trim());
                    }
                }
            }
        }

        String[] selectedBundles = null;
        if (argBundles.size() > 0) {
            selectedBundles = (String[]) argBundles.toArray(new String[argBundles.size()]);
        }
        Bundle[] bundles = getBundles(selectedBundles, sortId);
        boolean useDefaultOper = !registeredByNames && !usedByNames;
        ServiceReference[] servicesRefs = null;
        if (filter != null && '(' != filter.charAt(0)) {
            filter = "(" + filter + ")";
        }
        if (serviceId != null) {
            if (filter != null) {
                filter = "(&(service.id=" + serviceId + ")" + filter + ")";
            } else {
                filter = "(service.id=" + serviceId + ")";
            }
        }
        if (filter != null) {
            try {
                servicesRefs = bundleContext.getServiceReferences(null, filter);
                if (null == servicesRefs) {
                    out.println("No services matching '" + filter + "'.");
                    return;
                }
            } catch (InvalidSyntaxException ise) {
                out.println("Invalid filter '" + filter + "' found: " + ise.getMessage());
                return;
            }
        }

        for (int i = 0; i < bundles.length; i++) {
            if (bundles[i] != null) {
                Bundle bundle = bundles[i];
                String heading = "Bundle: " + shortName(bundle) + " (#" + bundle.getBundleId() + ")";
                boolean headingPrinted = false;
                if (registeredByNames || useDefaultOper) {
                    headingPrinted = showServices(getServicesRegisteredBy(bundles[i], servicesRefs), out, heading,
                            headingPrinted, "  registered:", verbose);
                }
                if (usedByNames) {
                    headingPrinted = showServices(getServicesUsedBy(bundles[i], servicesRefs), out, heading, headingPrinted,
                            "  uses:", verbose);
                }
            }
        }
    }

    private Bundle[] getBundles(String[] selection, boolean sortNumeric) {
        Bundle[] b = bundleContext.getBundles();
        selectBundles(b, selection);
        if (sortNumeric) {
            sortBundlesId(b);
        }

        return b;
    }

    /**
     * Sort an array of bundle objects based on their Bundle Id. All entries containing NULL are placed at the end of the array.
     *
     * @param b array of bundles to be sorted, modified with result
     */
    private void sortBundlesId(Bundle[] b) {
        int x = b.length;

        for (int l = x; x > 0;) {
            x = 0;
            long p = b[0] != null ? b[0].getBundleId() : Long.MAX_VALUE;
            for (int i = 1; i < l; i++) {
                long n = b[i] != null ? b[i].getBundleId() : Long.MAX_VALUE;
                if (p > n) {
                    x = i - 1;
                    Bundle t = b[x];
                    b[x] = b[i];
                    b[i] = t;
                } else {
                    p = n;
                }
            }
        }
    }

    /**
     * Select bundles from an array of Bundle objects. All the bundles in the bundle array is tested against all the strings in
     * the selection array. If none of the strings match the element in the bundle array is set to NULL. A selection string
     * matches if:
     * <ul>
     * <item>It matches the bundle ID.</item> <item>It matches the location of the bundle.</item> <item>It matches the symbolic
     * name of the bundle.</item> <item>It matches the short name of the bundle.</item>
     * </ul>
     *
     * It is also possible to start or end the selection string with the wildcard character '*' (Does not work for bundle id).
     *
     * @param bundles array of bundles to be checked, modified with result
     * @param selection array of selection string to match against bundles
     * @param selectionMatches Each patterns from selection array that matches a bundle will be added to this set.
     */
    private void selectBundles(Bundle[] bundles, String[] selection) {
        if (selection != null) {
            for (int i = 0; i < bundles.length; i++) {
                int j = 0;
                final String bn = shortName(bundles[i]);
                final String sn = symbolicName(bundles[i]); // May be null!
                final String l = bundles[i].getLocation();
                for (; j < selection.length; j++) {
                    try {
                        long id = Long.parseLong(selection[j]);
                        if (bundles[i].getBundleId() == id) {
                            break;
                        }
                    } catch (NumberFormatException e) {
                        // Ignore
                    }
                    if (bn.equals(selection[j]) || l.equals(selection[j]) || (null != sn && sn.equals(selection[j]))) {
                        break;
                    }
                    if (selection[j].startsWith("*")) {
                        final String s = selection[j].substring(1);
                        if (bn.endsWith(s) || l.endsWith(s) || (null != sn && sn.endsWith(s))) {
                            break;
                        }
                    } else if (selection[j].endsWith("*")) {
                        final String s = selection[j].substring(0, selection[j].length() - 1);
                        if (bn.startsWith(s) || (null != sn && sn.startsWith(s))) {
                            break;
                        }
                        if (s.indexOf(':') != -1 && l.startsWith(s)) {
                            break;
                        }
                    } else {
                        if (l.endsWith("/" + selection[j] + ".jar") || l.endsWith("\\" + selection[j] + ".jar")) {
                            break;
                        }
                    }
                }
                if (j == selection.length) {
                    bundles[i] = null;
                }
            }
        }
    }

    /**
     * Get short name of specified bundle. First, try to get the BUNDLE-NAME header. If it fails use the location of the bundle
     * with all characters upto and including the last '/' or '\' and any trailing ".jar" stripped off.
     *
     * @param bundle the bundle
     * @return The bundles shortname or null if input was null
     */
    private String shortName(Bundle bundle) {
        if (bundle == null) {
            return null;
        }
        String n = (String) bundle.getHeaders().get("Bundle-Name");
        if (n == null) {
            n = bundle.getLocation();
            int x = n.lastIndexOf('/');
            int y = n.lastIndexOf('\\');
            if (y > x) {
                x = y;
            }
            if (x != -1) {
                n = n.substring(x + 1);
            }
            if (n.endsWith(".jar")) {
                n = n.substring(0, n.length() - 4);
            }
        }
        return n;
    }

    /**
     * Get the symbolic name of the specified bundle. All directives and parameters attached to the symbolic name attribute will
     * be stripped.
     *
     * @param bundle the bundle
     * @return The bundles symbolic name or null if not specified.
     */
    private String symbolicName(Bundle bundle) {
        if (bundle == null) {
            return null;
        }

        final Dictionary d = bundle.getHeaders();
        String bsn = (String) d.get("Bundle-SymbolicName");
        if (bsn != null && bsn.length() > 0) {
            // Remove parameters and directives from the value
            final int semiPos = bsn.indexOf(';');
            if (-1 < semiPos) {
                bsn = bsn.substring(0, semiPos).trim();
            }
        }
        return bsn;
    }

    private ServiceReference[] getServicesRegisteredBy(Bundle b, ServiceReference[] services) {
        if (services == null) {
            return b.getRegisteredServices();
        }
        // Filter the given services on registered by.
        long bid = b.getBundleId();
        int count = 0;
        for (int j = 0; j < services.length; j++) {
            if (bid == services[j].getBundle().getBundleId()) {
                count++;
            }
        }
        if (0 == count) {
            return null;
        }
        ServiceReference[] res = new ServiceReference[count];
        int ix = 0;
        for (int j = 0; j < services.length; j++) {
            if (bid == services[j].getBundle().getBundleId()) {
                res[ix++] = services[j];
            }
        }
        return res;
    }

    private ServiceReference[] getServicesUsedBy(Bundle b, ServiceReference[] services) {
        if (null == services) {
            return b.getServicesInUse();
        }
        // Filter the given services on using bundle.
        long bid = b.getBundleId();
        int count = 0;
        for (int j = 0; j < services.length; j++) {
            Bundle[] usingBundles = services[j].getUsingBundles();
            for (int k = 0; usingBundles != null && k < usingBundles.length; k++) {
                if (bid == usingBundles[k].getBundleId()) {
                    count++;
                    break;
                }
            }
        }
        if (0 == count) {
            return null;
        }
        ServiceReference[] res = new ServiceReference[count];
        int ix = 0;
        for (int j = 0; j < services.length; j++) {
            Bundle[] usingBundles = services[j].getUsingBundles();
            for (int k = 0; usingBundles != null && k < usingBundles.length; k++) {
                if (bid == usingBundles[k].getBundleId()) {
                    res[ix++] = services[j];
                    break;
                }
            }
        }
        return res;
    }

    private boolean showServices(final ServiceReference[] services, final PrintStream out, final String heading,
            final boolean headinPrinted, final String title, final boolean detailed) {
        if (services != null && services.length > 0) {
            if (!headinPrinted) {
                out.println(heading);
            }
            out.print(title);
            for (int j = 0; j < services.length; j++) {
                if (null != services[j]) {
                    if (detailed) {
                        out.print("\n    ");
                        showLongService(services[j], "    ", out);
                    } else {
                        out.print(" " + showServiceClasses(services[j]));
                    }
                }
            }
            out.println("");
            return true;
        }
        return false;
    }

    private void showLongService(ServiceReference s, String pad, PrintStream out) {
        out.print(showServiceClasses(s));
        String[] k = s.getPropertyKeys();
        for (int i = 0; i < k.length; i++) {
            out.print("\n  " + pad + k[i] + " = " + showObject(s.getProperty(k[i])));
        }
    }

    /**
     * Get Service class list as a string.
     *
     * @param sr The service
     * @return The bundles identifier
     */
    private String showServiceClasses(ServiceReference sr) {
        StringBuffer sb = new StringBuffer();
        String[] c = (String[]) sr.getProperty("objectClass");
        if (c.length >= 2) {
            sb.append("[");
        }
        for (int i = 0; i < c.length; i++) {
            if (i > 0) {
                sb.append(",");
            }
            int x = c[i].lastIndexOf('.');
            sb.append(x != -1 ? c[i].substring(x + 1) : c[i]);
        }
        if (c.length >= 2) {
            sb.append("]");
        }
        return sb.toString();
    }

    /**
     * Get string representation of an object. If it is an array or Enumeration, do toString on each element. All other objects
     * are shown with toString.
     *
     * @param o the object to show
     * @return A string showing the object
     */
    private String showObject(Object o) {
        if (o == null) {
            return "null";
        } else {
            return o.toString();
        }
    }

    private String showObject(Vector o) {
        StringBuffer sb = new StringBuffer();
        Enumeration e = o.elements();
        sb.append("[");
        if (e.hasMoreElements()) {
            sb.append(showObject(e.nextElement()));
        }
        while (e.hasMoreElements()) {
            sb.append(", " + showObject(e.nextElement()));
        }
        sb.append("]");
        return sb.toString();
    }

    private String showObject(Object[] o) {
        StringBuffer sb = new StringBuffer();
        int len = o.length;
        sb.append("[");
        if (len > 0) {
            sb.append(showObject(o[0]));
        }
        for (int i = 1; i < len; i++) {
            sb.append(", " + showObject(o[i]));
        }
        sb.append("]");
        return sb.toString();
    }

}
