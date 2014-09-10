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

import com.actility.m2m.song.binding.http.LongPollingServerStats;
import com.actility.m2m.song.binding.http.SongHttpBindingStatsService;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;

public final class GetLongPollingServerStatsCommand implements Command {

    private final BundleContext context;

    public GetLongPollingServerStatsCommand(BundleContext context) {
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
        return "http-getlpserverstats";
    }

    public String getUsage() {
        return "http-getlpserverstats <id>\n" + "    - id   : The identifier of the long polling server";
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

            LongPollingServerStats lpServerStats = songHttpBindingStatsService.getLongPollingServerStatistics(id);

            if (lpServerStats != null) {
                long endedContactRequests = lpServerStats.getAccOfContactRequests() - lpServerStats.getNbOfContactRequests();
                long unknownEndContactRequests = endedContactRequests - lpServerStats.getAccOfNormalEndContactRequests()
                        - lpServerStats.getAccOfExpiredEndContactRequests()
                        - lpServerStats.getAccOfExpiredWaitingEndContactRequests()
                        - lpServerStats.getAccOfErrorEndContactRequests()
                        - lpServerStats.getAccOfServerFullEndContactRequests();
                long endedLongPollingRequests = lpServerStats.getAccOfLongPollingRequests()
                        - lpServerStats.getNbOfLongPollingRequests();
                long unknownEndLongPollingRequests = endedLongPollingRequests
                        - lpServerStats.getAccOfContactEndLongPollingRequests()
                        - lpServerStats.getAccOfErrorEndLongPollingRequests()
                        - lpServerStats.getAccOfConcurrentEndLongPollingRequests()
                        - lpServerStats.getAccOfExpiredEndLongPollingRequests();

                out.print("LastContactRequest                   : ");
                printValue(lpServerStats.getLastContactRequest(), out);
                out.print("NbOfContactRequests                  : ");
                printValue(lpServerStats.getNbOfContactRequests(), out);
                out.print("NbOfWaitingContactRequests           : ");
                printValue(lpServerStats.getNbOfWaitingContactRequests(), out);
                out.print("NbOfWaitingResponseContactRequests   : ");
                printValue(lpServerStats.getNbOfWaitingResponseContactRequests(), out);
                out.print("AccOfContactRequests                 : ");
                printValue(lpServerStats.getAccOfContactRequests(), out);
                out.print("AccOfNormalEndContactRequests        : ");
                printValueWithPourcentage(lpServerStats.getAccOfNormalEndContactRequests(), endedContactRequests, out);
                out.print("AccOfErrorEndContactRequests         : ");
                printValueWithPourcentage(lpServerStats.getAccOfErrorEndContactRequests(), endedContactRequests, out);
                out.print("AccOfExpiredEndContactRequests       : ");
                printValueWithPourcentage(lpServerStats.getAccOfExpiredEndContactRequests(), endedContactRequests, out);
                out.print("AccOfExpiredWaitingEndContactRequests: ");
                printValueWithPourcentage(lpServerStats.getAccOfExpiredWaitingEndContactRequests(), endedContactRequests, out);
                out.print("AccOfServerFullEndContactRequests    : ");
                printValueWithPourcentage(lpServerStats.getAccOfServerFullEndContactRequests(), endedContactRequests, out);
                out.print("AccOfUnknownEndContactRequests       : ");
                printValueWithPourcentage(unknownEndContactRequests, endedContactRequests, out);
                out.print("LastContactResponse                  : ");
                printValue(lpServerStats.getLastContactResponse(), out);
                out.print("AccOfContactResponses                : ");
                printValue(lpServerStats.getAccOfContactResponses(), out);
                out.print("LastLongPollingRequest               : ");
                printValue(lpServerStats.getLastLongPollingRequest(), out);
                out.print("NbOfLongPollingRequests              : ");
                printValue(lpServerStats.getNbOfLongPollingRequests(), out);
                out.print("AccOfLongPollingRequests             : ");
                printValue(lpServerStats.getAccOfLongPollingRequests(), out);
                out.print("AccOfContactEndLongPollingRequests   : ");
                printValueWithPourcentage(lpServerStats.getAccOfContactEndLongPollingRequests(), endedLongPollingRequests, out);
                out.print("AccOfErrorEndLongPollingRequests     : ");
                printValueWithPourcentage(lpServerStats.getAccOfErrorEndLongPollingRequests(), endedLongPollingRequests, out);
                out.print("AccOfConcurrentEndLongPollingRequests: ");
                printValueWithPourcentage(lpServerStats.getAccOfConcurrentEndLongPollingRequests(), endedLongPollingRequests,
                        out);
                out.print("AccOfExpiredEndLongPollingRequests   : ");
                printValueWithPourcentage(lpServerStats.getAccOfExpiredEndLongPollingRequests(), endedLongPollingRequests, out);
                out.print("AccOfUnknownEndLongPollingRequests   : ");
                printValueWithPourcentage(unknownEndLongPollingRequests, endedLongPollingRequests, out);
            } else {
                err.println("Unknown long polling server id");
            }

            context.ungetService(sr);
        } else {
            err.println("You must specify an <id>");
            err.println(getUsage());
        }
    }
}
