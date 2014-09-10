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

package com.actility.m2m.servlet.song.http.stats;

import java.util.Date;

import com.actility.m2m.song.binding.http.LongPollingServerStats;

public final class LongPollingServerStatsImpl implements LongPollingServerStats {

    private final Date lastContactRequest;

    private final long nbOfContactRequests;

    private final long nbOfWaitingContactRequests;

    private final long nbOfWaitingResponseContactRequests;

    private final long accOfContactRequests;

    private final long accOfNormalEndContactRequests;

    private final long accOfErrorEndContactRequests;

    private final long accOfExpiredEndContactRequests;

    private final long accOfExpiredWaitingEndContactRequests;

    private final long accOfServerFullEndContactRequests;

    private final Date lastContactResponse;

    private final long accOfContactResponses;

    private final Date lastLongPollingRequest;

    private final long nbOfLongPollingRequests;

    private final long accOfLongPollingRequests;

    private final long accOfContactEndLongPollingRequests;

    private final long accOfErrorEndLongPollingRequests;

    private final long accOfConcurrentEndLongPollingRequests;

    private final long accOfExpiredEndLongPollingRequests;

    public LongPollingServerStatsImpl(Date lastContactRequest, long nbOfContactRequests, long nbOfWaitingContactRequests,
            long nbOfWaitingResponseContactRequests, long accOfContactRequests, long accOfNormalEndContactRequests,
            long accOfErrorEndContactRequests, long accOfExpiredEndContactRequests,
            long accOfExpiredWaitingEndContactRequests, long accOfServerFullEndContactRequests, Date lastContactResponse,
            long accOfContactResponses, Date lastLongPollingRequest, long nbOfLongPollingRequests,
            long accOfLongPollingRequests, long accOfContactEndLongPollingRequests,
            long accOfErrorEndLongPollingRequests, long accOfConcurrentEndLongPollingRequests,
            long accOfExpiredEndLongPollingRequests) {
        this.lastContactRequest = lastContactRequest;
        this.nbOfContactRequests = nbOfContactRequests;
        this.nbOfWaitingContactRequests = nbOfWaitingContactRequests;
        this.nbOfWaitingResponseContactRequests = nbOfWaitingResponseContactRequests;
        this.accOfContactRequests = accOfContactRequests;
        this.accOfNormalEndContactRequests = accOfNormalEndContactRequests;
        this.accOfErrorEndContactRequests = accOfErrorEndContactRequests;
        this.accOfExpiredEndContactRequests = accOfExpiredEndContactRequests;
        this.accOfExpiredWaitingEndContactRequests = accOfExpiredWaitingEndContactRequests;
        this.accOfServerFullEndContactRequests = accOfServerFullEndContactRequests;
        this.lastContactResponse = lastContactResponse;
        this.accOfContactResponses = accOfContactResponses;
        this.lastLongPollingRequest = lastLongPollingRequest;
        this.nbOfLongPollingRequests = nbOfLongPollingRequests;
        this.accOfLongPollingRequests = accOfLongPollingRequests;
        this.accOfContactEndLongPollingRequests = accOfContactEndLongPollingRequests;
        this.accOfErrorEndLongPollingRequests = accOfErrorEndLongPollingRequests;
        this.accOfConcurrentEndLongPollingRequests = accOfConcurrentEndLongPollingRequests;
        this.accOfExpiredEndLongPollingRequests = accOfExpiredEndLongPollingRequests;
    }

    public Date getLastContactRequest() {
        return lastContactRequest;
    }

    public long getNbOfContactRequests() {
        return nbOfContactRequests;
    }

    public long getNbOfWaitingContactRequests() {
        return nbOfWaitingContactRequests;
    }

    public long getNbOfWaitingResponseContactRequests() {
        return nbOfWaitingResponseContactRequests;
    }

    public long getAccOfContactRequests() {
        return accOfContactRequests;
    }

    public long getAccOfNormalEndContactRequests() {
        return accOfNormalEndContactRequests;
    }

    public long getAccOfErrorEndContactRequests() {
        return accOfErrorEndContactRequests;
    }

    public long getAccOfExpiredEndContactRequests() {
        return accOfExpiredEndContactRequests;
    }

    public long getAccOfExpiredWaitingEndContactRequests() {
        return accOfExpiredWaitingEndContactRequests;
    }

    public long getAccOfServerFullEndContactRequests() {
        return accOfServerFullEndContactRequests;
    }

    public Date getLastContactResponse() {
        return lastContactResponse;
    }

    public long getAccOfContactResponses() {
        return accOfContactResponses;
    }

    public Date getLastLongPollingRequest() {
        return lastLongPollingRequest;
    }

    public long getNbOfLongPollingRequests() {
        return nbOfLongPollingRequests;
    }

    public long getAccOfLongPollingRequests() {
        return accOfLongPollingRequests;
    }

    public long getAccOfContactEndLongPollingRequests() {
        return accOfContactEndLongPollingRequests;
    }

    public long getAccOfErrorEndLongPollingRequests() {
        return accOfErrorEndLongPollingRequests;
    }

    public long getAccOfConcurrentEndLongPollingRequests() {
        return accOfConcurrentEndLongPollingRequests;
    }

    public long getAccOfExpiredEndLongPollingRequests() {
        return accOfExpiredEndLongPollingRequests;
    }
}
