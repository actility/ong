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
 * id $Id: SongStatsCommand.java 8067 2014-03-10 14:07:37Z JReich $
 * author $Author: JReich $
 * version $Revision: 8067 $
 * lastrevision $Date: 2014-03-10 15:07:37 +0100 (Mon, 10 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-10 15:07:37 +0100 (Mon, 10 Mar 2014) $
 */

package com.actility.m2m.song.command;

import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.servlet.song.service.ext.ExtSongService;
import com.actility.m2m.servlet.song.service.ext.SongStats;

public class SongStatsCommand implements Command {

    private BundleContext bundleContext;

    public SongStatsCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    private void repeatChar(int nbPixels, char character, PrintStream out) {
        for (int i = 0; i < nbPixels; ++i) {
            out.print(character);
        }
    }

    private void printLine(int nbPixels, PrintStream out) {
        repeatChar(nbPixels, '-', out);
        out.println();
    }

    private void printColumnAlignCenter(int nbPixels, String text, PrintStream out) {
        out.print(' ');
        int textLength = text.length();
        if (textLength > (nbPixels - 3)) {
            out.print(text.substring(0, nbPixels - 3));
        } else {
            int startLength = (nbPixels - 3 - textLength) / 2;
            repeatChar(startLength, ' ', out);
            out.print(text);
            repeatChar(nbPixels - 3 - textLength - startLength, ' ', out);
        }
        out.print(" |");
    }

    private void printColumnAlignLeft(int nbPixels, String text, PrintStream out) {
        out.print(' ');
        int textLength = text.length();
        if (textLength > (nbPixels - 3)) {
            out.print(text.substring(0, nbPixels - 3));
        } else {
            out.print(text);
            repeatChar(nbPixels - 3 - textLength, ' ', out);
        }
        out.print(" |");
    }

    private void printColumnAlignRight(int nbPixels, double text, PrintStream out) {
        double truncatedValue = ((long) (text * 100.0)) / 100.0;
        printColumnAlignRight(nbPixels, String.valueOf(truncatedValue), out);
    }

    private void printColumnAlignRight(int nbPixels, long text, PrintStream out) {
        printColumnAlignRight(nbPixels, String.valueOf(text), out);
    }

    private void printColumnAlignRight(int nbPixels, String text, PrintStream out) {
        out.print(' ');
        int textLength = text.length();
        if (textLength > (nbPixels - 3)) {
            out.print(text.substring(0, nbPixels - 3));
        } else {
            repeatChar(nbPixels - 3 - textLength, ' ', out);
            out.print(text);
        }
        out.print(" |");
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        int group = SongStats.GROUP_GLOBAL;
        if (st.countTokens() >= 1) {
            String groupStr = st.nextToken().trim();
            if ("global".equalsIgnoreCase(groupStr)) {
                group = SongStats.GROUP_GLOBAL;
            } else if ("local".equalsIgnoreCase(groupStr)) {
                group = SongStats.GROUP_LOCAL;
            } else if ("remote".equalsIgnoreCase(groupStr)) {
                group = SongStats.GROUP_REMOTE;
            } else if ("forward".equalsIgnoreCase(groupStr)) {
                group = SongStats.GROUP_FORWARD;
            } else {
                err.println(getUsage());
                return;
            }
        }

        ServiceReference sr1 = bundleContext.getServiceReference(ExtSongService.class.getName());
        if (sr1 == null) {
            out.println("Unable to get the ExtSongService");
            return;
        }
        ExtSongService songService = (ExtSongService) bundleContext.getService(sr1);
        if (songService == null) {
            out.println("Unable to get the ExtSongService");
            return;
        }

        int nbMaxPixels = 156;
        int nbColumns = 7;
        int columnWidth = (nbMaxPixels - 1) / nbColumns;
        int nbPixels = 1 + (nbColumns * columnWidth);

        printLine(nbPixels, out);
        out.print("|");
        printColumnAlignCenter(columnWidth, "", out);
        printColumnAlignCenter(columnWidth, "Total", out);
        printColumnAlignCenter(columnWidth, "CREATE", out);
        printColumnAlignCenter(columnWidth, "RETRIEVE", out);
        printColumnAlignCenter(columnWidth, "UPDATE", out);
        printColumnAlignCenter(columnWidth, "DELETE", out);
        printColumnAlignCenter(columnWidth, "Unknown", out);
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        SongStats stats = songService.getSongStats();

        printColumnAlignLeft(columnWidth, "Pending requests", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getNbRequests(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Nb requests", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNbRequests(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Not responded", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNbRequests(group, j) - stats.getAccNbResponses(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Nb responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNbResponses(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/max all", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverageRequestDuration(group, j) + " / " + stats.getMaxRequestDuration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "2xx responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb2xx(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 2xx", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage2xxDuration(group, j) + " / " + stats.getMax2xxDuration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "3xx responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb3xx(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 3xx", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage3xxDuration(group, j) + " / " + stats.getMax3xxDuration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "404 responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb404(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 404", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage404Duration(group, j) + " / " + stats.getMax404Duration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "408 responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb408(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 408", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage408Duration(group, j) + " / " + stats.getMax408Duration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "409 responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb409(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 409", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage409Duration(group, j) + " / " + stats.getMax409Duration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "4xx responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb4xx(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 4xx", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage4xxDuration(group, j) + " / " + stats.getMax4xxDuration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "504 responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb504(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 504", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage504Duration(group, j) + " / " + stats.getMax504Duration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "5xx responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNb5xx(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max 5xx", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverage5xxDuration(group, j) + " / " + stats.getMax5xxDuration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Unknown responses", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth, stats.getAccNbXxx(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);
        out.print("|");

        printColumnAlignLeft(columnWidth, "Average/Max Unknwon", out);
        for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
            printColumnAlignRight(columnWidth,
                    stats.getAverageXxxDuration(group, j) + " / " + stats.getMaxXxxDuration(group, j), out);
        }
        out.println();
        printLine(nbPixels, out);

        bundleContext.ungetService(sr1);
    }

    public String getName() {
        return "song-stats";
    }

    public String getShortDescription() {
        return "Get SONG statistics";
    }

    public String getUsage() {
        return "song-stats <type>\n"
                + "    - type   : The type of requests whether global, local, remote or forward (default is global)";
    }

}
