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

package com.actility.m2m.song.binding.http;

import java.util.Date;

public interface LongPollingServerStats {
    Date getLastContactRequest();

    long getNbOfContactRequests();

    long getNbOfWaitingContactRequests();

    long getNbOfWaitingResponseContactRequests();

    long getAccOfContactRequests();

    long getAccOfNormalEndContactRequests();

    long getAccOfErrorEndContactRequests();

    long getAccOfExpiredEndContactRequests();

    long getAccOfExpiredWaitingEndContactRequests();

    long getAccOfServerFullEndContactRequests();

    Date getLastContactResponse();

    long getAccOfContactResponses();

    Date getLastLongPollingRequest();

    long getNbOfLongPollingRequests();

    long getAccOfLongPollingRequests();

    long getAccOfContactEndLongPollingRequests();

    long getAccOfErrorEndLongPollingRequests();

    long getAccOfConcurrentEndLongPollingRequests();

    long getAccOfExpiredEndLongPollingRequests();
}
