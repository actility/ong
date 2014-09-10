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
 * version $Revision: 17 $
 * lastrevision $Date: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 */

package com.actility.m2m.framework.command.se;

import java.io.PrintStream;

import org.apache.felix.shell.Command;

public final class ThreadsCommand implements Command {

    public String getName() {
        return "threads";
    }

    public String getUsage() {
        return "threads";
    }

    public String getShortDescription() {
        return "Display threads within this framework";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        ThreadGroup tg = Thread.currentThread().getThreadGroup();

        for (ThreadGroup ctg = tg; ctg != null; ctg = ctg.getParent()) {
            tg = ctg;
        }

        Thread[] threads;
        int count;
        while (true) {
            int acount = tg.activeCount() + 5;
            threads = new Thread[acount];
            count = tg.enumerate(threads);
            if (count < acount) {
                break;
            }
        }
        int groupCols = tg.getName().length();
        boolean sameGroup = true;
        for (int i = 0; i < count; i++) {
            ThreadGroup itg = threads[i].getThreadGroup();
            if (!tg.equals(itg)) {
                int cols = itg.getName().length();
                if (groupCols < cols) {
                    groupCols = cols;
                }
                sameGroup = false;
            }
        }
        out.print("Pri ");
        if (!sameGroup) {
            String glabel = "Group                              ";
            if (groupCols < 4) {
                groupCols = 4;
            }
            if (++groupCols > glabel.length()) {
                groupCols = glabel.length();
                out.print(glabel);
            } else {
                out.print(glabel.substring(0, groupCols));
            }
        }
        out.println("Name");
        for (int i = 0; i < count; i++) {
            try {
                StringBuffer sb = new StringBuffer();
                int p = threads[i].getPriority();
                if (p < 10) {
                    sb.append(' ');
                }
                sb.append(p);
                do {
                    sb.append(' ');
                } while (sb.length() < 4);
                if (!sameGroup) {
                    String g = threads[i].getThreadGroup().getName();
                    sb.append(g);
                    int l = g.length();
                    do {
                        sb.append(' ');
                        l++;
                    } while (l < groupCols);
                }
                sb.append(threads[i].getName());
                out.println(sb.toString());
            } catch (NullPointerException _ignore) {
                // Handle disappering thread
            }
        }
    }
}
