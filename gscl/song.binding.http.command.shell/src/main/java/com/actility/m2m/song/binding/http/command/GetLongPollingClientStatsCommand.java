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

package com.actility.m2m.song.binding.http.command;

import java.io.PrintStream;
import java.util.Date;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.song.binding.http.LongPollingClientStats;
import com.actility.m2m.song.binding.http.SongHttpBindingStatsService;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;

public final class GetLongPollingClientStatsCommand implements Command {

    private final BundleContext context;

    public GetLongPollingClientStatsCommand(BundleContext context) {
        this.context = context;
    }

    private void repeatChar(int nbPixels, char character, PrintStream out) {
        for (int i = 0; i < nbPixels; ++i) {
            out.print(character);
        }
    }

    private void printValue(Date value, PrintStream out) {
        out.println(FormatUtils.formatDateTime(value, UtilConstants.LOCAL_TIMEZONE));
    }

    private void printValue(long value, PrintStream out) {
        printColumnAlignRight(7, value, out);
        out.println();
    }

    private void printValueWithPourcentage(long value, long allValues, PrintStream out) {
        printColumnAlignRight(7, value, out);
        out.print(" (");
        long pourcentageCents = ((long) ((value * 10000.0) / allValues)) % 100L;
        long pourcentage = (long) ((value * 100.0) / allValues);
        printColumnAlignRight(3, pourcentage, out);
        out.print('.');
        if (pourcentageCents < 10) {
            out.print('0');
        }
        out.print(pourcentageCents);
        out.println("%)");
    }

    private void printColumnAlignRight(int nbPixels, long text, PrintStream out) {
        printColumnAlignRight(nbPixels, String.valueOf(text), out);
    }

    private void printColumnAlignRight(int nbPixels, String text, PrintStream out) {
        int textLength = text.length();
        if (textLength > (nbPixels)) {
            out.print(text.substring(0, nbPixels));
        } else {
            repeatChar(nbPixels - textLength, ' ', out);
            out.print(text);
        }
    }

    public String getName() {
        return "http-getlpclientstats";
    }

    public String getUsage() {
        return "http-getlpclientstats <id>\n" + "    - id   : The identifier of the long polling client";
    }

    public String getShortDescription() {
        return "Gets statistics about the identified long polling client";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        if (st.countTokens() >= 1) {
            String id = st.nextToken().trim();

            ServiceReference sr = context.getServiceReference(SongHttpBindingStatsService.class.getName());
            if (sr == null) {
                err.println("Unable to get the SongHttpBindingStatsService");
                return;
            }
            SongHttpBindingStatsService songHttpBindingStatsService = (SongHttpBindingStatsService) context.getService(sr);
            if (songHttpBindingStatsService == null) {
                err.println("Unable to get the SongHttpBindingStatsService");
                return;
            }

            LongPollingClientStats lpClientStats = songHttpBindingStatsService.getLongPollingClientStatistics(id);

            if (lpClientStats != null) {
                long endedContactRequests = lpClientStats.getAccOfContactRequests() - lpClientStats.getNbOfContactRequests();
                long unknownEndContactRequests = endedContactRequests - lpClientStats.getAccOfNormalEndContactRequests()
                        - lpClientStats.getAccOfBadRequestEndContactRequests()
                        - lpClientStats.getAccOfErrorEndContactRequests();
                long endedLongPollingRequests = lpClientStats.getAccOfLongPollingRequests()
                        - lpClientStats.getNbOfLongPollingRequests();
                long unknownEndLongPollingRequests = endedLongPollingRequests
                        - lpClientStats.getAccOfContactEndLongPollingRequests()
                        - lpClientStats.getAccOfEmptyEndLongPollingRequests()
                        - lpClientStats.getAccOfErrorEndLongPollingRequests()
                        - lpClientStats.getAccOfExceptionEndLongPollingRequests()
                        - lpClientStats.getAccOfExpiredEndLongPollingRequests();

                out.print("LastContactRequest                  : ");
                printValue(lpClientStats.getLastContactRequest(), out);
                out.print("NbOfContactRequests                 : ");
                printValue(lpClientStats.getNbOfContactRequests(), out);
                out.print("AccOfContactRequests                : ");
                printValue(lpClientStats.getAccOfContactRequests(), out);
                out.print("AccOfNormalEndContactRequests       : ");
                printValueWithPourcentage(lpClientStats.getAccOfNormalEndContactRequests(), endedContactRequests, out);
                out.print("AccOfBadRequestEndContactRequests   : ");
                printValueWithPourcentage(lpClientStats.getAccOfBadRequestEndContactRequests(), endedContactRequests, out);
                out.print("AccOfErrorEndContactRequests        : ");
                printValueWithPourcentage(lpClientStats.getAccOfErrorEndContactRequests(), endedContactRequests, out);
                out.print("AccOfUnknownEndContactRequests      : ");
                printValueWithPourcentage(unknownEndContactRequests, endedContactRequests, out);
                out.print("LastContactResponse                 : ");
                printValue(lpClientStats.getLastContactResponse(), out);
                out.print("AccOfContactResponses               : ");
                printValue(lpClientStats.getAccOfContactResponses(), out);
                out.print("AccOfErrorEndContactResponses       : ");
                printValue(lpClientStats.getAccOfErrorEndContactResponses(), out);
                out.print("LastLongPollingRequest              : ");
                printValue(lpClientStats.getLastLongPollingRequest(), out);
                out.print("NbOfLongPollingRequests             : ");
                printValue(lpClientStats.getNbOfLongPollingRequests(), out);
                out.print("AccOfLongPollingRequests            : ");
                printValue(lpClientStats.getAccOfLongPollingRequests(), out);
                out.print("AccOfContactEndLongPollingRequests  : ");
                printValueWithPourcentage(lpClientStats.getAccOfContactEndLongPollingRequests(), endedLongPollingRequests, out);
                out.print("AccOfEmptyEndLongPollingRequests    : ");
                printValueWithPourcentage(lpClientStats.getAccOfEmptyEndLongPollingRequests(), endedLongPollingRequests, out);
                out.print("AccOfErrorEndLongPollingRequests    : ");
                printValueWithPourcentage(lpClientStats.getAccOfErrorEndLongPollingRequests(), endedLongPollingRequests, out);
                out.print("AccOfExceptionEndLongPollingRequests: ");
                printValueWithPourcentage(lpClientStats.getAccOfExceptionEndLongPollingRequests(), endedLongPollingRequests,
                        out);
                out.print("AccOfExpiredEndLongPollingRequests  : ");
                printValueWithPourcentage(lpClientStats.getAccOfExpiredEndLongPollingRequests(), endedLongPollingRequests, out);
                out.print("AccOfUnknownEndLongPollingRequests  : ");
                printValueWithPourcentage(unknownEndLongPollingRequests, endedLongPollingRequests, out);
                out.println("State                               : " + lpClientStats.getStateMessage());
            } else {
                err.println("Unknown long polling client id");
            }

            context.ungetService(sr);
        } else {
            err.println("You must specify an <id>");
            err.println(getUsage());
        }
    }
}
