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
 * id $Id: SongStats.java 7987 2014-03-07 14:53:50Z JReich $
 * author $Author: JReich $
 * version $Revision: 7987 $
 * lastrevision $Date: 2014-03-07 15:53:50 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 15:53:50 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.servlet.song.service.ext;

/**
 * Class that gathers statistics on SONG activity
 */
public interface SongStats {
    /**
     * Group ID for all requests (local, remote and forward)
     */
    int GROUP_GLOBAL = 0;
    /**
     * Group ID for local requests. Local requests are requests generated locally and sent to another servlet or on the network
     */
    int GROUP_LOCAL = 1;
    /**
     * Group ID for remote requests. Remote requests are requests received from the network
     */
    int GROUP_REMOTE = 2;
    /**
     * Group ID for forward requests. Forward requests are requests forwarded by servlets to another servlet or on the network
     */
    int GROUP_FORWARD = 3;

    /**
     * Defines the number of groups for loops
     */
    int GROUP_NB = 4;

    /**
     * Sub group ID for all type of requests
     */
    int SUB_GROUP_GLOBAL = 0;
    /**
     * Sub group ID for all CREATE requests
     */
    int SUB_GROUP_CREATE = 1;
    /**
     * Sub group ID for all RETRIEVE requests
     */
    int SUB_GROUP_RETRIEVE = 2;
    /**
     * Sub group ID for all UPDATE requests
     */
    int SUB_GROUP_UPDATE = 3;
    /**
     * Sub group ID for all DELETE requests
     */
    int SUB_GROUP_DELETE = 4;
    /**
     * Sub group ID for all Unknown requests
     */
    int SUB_GROUP_UNKNOWN = 5;

    /**
     * Defines the number of sub-groups for loops
     */
    int SUB_GROUP_NB = 6;

    /**
     * Gets the number of pending requests in SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of pending requests in SONG container
     */
    long getNbRequests(int group, int subGroup);

    /**
     * Gets the number of requests handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of requests handled by the SONG container
     */
    long getAccNbRequests(int group, int subGroup);

    /**
     * Gets the number of responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of responses handled by the SONG container
     */
    long getAccNbResponses(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverageRequestDuration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMaxRequestDuration(int group, int subGroup);

    /**
     * Gets the number of 2xx responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 2xx responses handled by the SONG container
     */
    long getAccNb2xx(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage2xxDuration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax2xxDuration(int group, int subGroup);

    /**
     * Gets the number of 3xx responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 3xx responses handled by the SONG container
     */
    long getAccNb3xx(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage3xxDuration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax3xxDuration(int group, int subGroup);

    /**
     * Gets the number of 404 responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 404 responses handled by the SONG container
     */
    long getAccNb404(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage404Duration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax404Duration(int group, int subGroup);

    /**
     * Gets the number of 408 responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 408 responses handled by the SONG container
     */
    long getAccNb408(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage408Duration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax408Duration(int group, int subGroup);

    /**
     * Gets the number of 409 responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 409 responses handled by the SONG container
     */
    long getAccNb409(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage409Duration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax409Duration(int group, int subGroup);

    /**
     * Gets the number of 4xx responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 4xx responses handled by the SONG container
     */
    long getAccNb4xx(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage4xxDuration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax4xxDuration(int group, int subGroup);

    /**
     * Gets the number of 504 responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 504 responses handled by the SONG container
     */
    long getAccNb504(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage504Duration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax504Duration(int group, int subGroup);

    /**
     * Gets the number of 5xx responses handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of 5xx responses handled by the SONG container
     */
    long getAccNb5xx(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverage5xxDuration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMax5xxDuration(int group, int subGroup);

    /**
     * Gets the number of Xxx response (non 2xx - 4xx - 5xx responses) handled by the SONG container.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The number of Xxx responses handled by the SONG container
     */
    long getAccNbXxx(int group, int subGroup);

    /**
     * Gets the average duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The average duration of a request/response
     */
    double getAverageXxxDuration(int group, int subGroup);

    /**
     * Gets the maximum duration of a request/response.
     *
     * @param group The group of requests (global, local, remote or forward)
     * @param subGroup The sub group (global, create, retrieve, update, delete or unknown)
     * @return The maximum duration of a request/response
     */
    long getMaxXxxDuration(int group, int subGroup);
}
