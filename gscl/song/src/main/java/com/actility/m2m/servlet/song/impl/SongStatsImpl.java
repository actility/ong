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
 * id $Id: SongStatsImpl.java 7993 2014-03-07 15:02:32Z JReich $
 * author $Author: JReich $
 * version $Revision: 7993 $
 * lastrevision $Date: 2014-03-07 16:02:32 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 16:02:32 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.servlet.song.impl;

import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.service.ext.SongStats;

public final class SongStatsImpl implements SongStats {

    private static final int STAT_ACC_NB_REQUESTS = 0;
    private static final int STAT_ACC_NB_RESPONSES = 1;
    private static final int STAT_ACC_DURATION = 2;
    private static final int STAT_MAX_DURATION = 3;
    private static final int STAT_ACC_NB_2XX = 4;
    private static final int STAT_ACC_DURATION_2XX = 5;
    private static final int STAT_MAX_DURATION_2XX = 6;
    private static final int STAT_ACC_NB_3XX = 7;
    private static final int STAT_ACC_DURATION_3XX = 8;
    private static final int STAT_MAX_DURATION_3XX = 9;
    private static final int STAT_ACC_NB_404 = 10;
    private static final int STAT_ACC_DURATION_404 = 11;
    private static final int STAT_MAX_DURATION_404 = 12;
    private static final int STAT_ACC_NB_408 = 13;
    private static final int STAT_ACC_DURATION_408 = 14;
    private static final int STAT_MAX_DURATION_408 = 15;
    private static final int STAT_ACC_NB_409 = 16;
    private static final int STAT_ACC_DURATION_409 = 17;
    private static final int STAT_MAX_DURATION_409 = 18;
    private static final int STAT_ACC_NB_4XX = 19;
    private static final int STAT_ACC_DURATION_4XX = 20;
    private static final int STAT_MAX_DURATION_4XX = 21;
    private static final int STAT_ACC_NB_504 = 22;
    private static final int STAT_ACC_DURATION_504 = 23;
    private static final int STAT_MAX_DURATION_504 = 24;
    private static final int STAT_ACC_NB_5XX = 25;
    private static final int STAT_ACC_DURATION_5XX = 26;
    private static final int STAT_MAX_DURATION_5XX = 27;
    private static final int STAT_ACC_NB_XXX = 28;
    private static final int STAT_ACC_DURATION_XXX = 29;
    private static final int STAT_MAX_DURATION_XXX = 30;
    private static final int STAT_NB_REQUESTS = 31;

    private static final int STAT_NB = 32;

    private long[][][] statistics = new long[GROUP_NB][SUB_GROUP_NB][STAT_NB];

    public SongStatsImpl() {
        // Default contructor
    }

    public SongStatsImpl(SongStatsImpl copy) {
        synchronized (copy) {
            for (int i = 0; i < SongStats.GROUP_NB; ++i) {
                for (int j = 0; j < SongStats.SUB_GROUP_NB; ++j) {
                    for (int k = 0; k < STAT_NB; ++k) {
                        statistics[i][j][k] = copy.statistics[i][j][k];
                    }
                }
            }
        }
    }

    private double getRoundedRatio(double numerator, double denominator) {
        return Math.round((numerator / denominator) * 100.0) / 100.0;
    }

    public long getNbRequests(int group, int subGroup) {
        return statistics[group][subGroup][STAT_NB_REQUESTS];
    }

    public long getAccNbRequests(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_REQUESTS];
    }

    public long getAccNbResponses(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_RESPONSES];
    }

    public double getAverageRequestDuration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION],
                statistics[group][subGroup][STAT_ACC_NB_RESPONSES]);
    }

    public long getMaxRequestDuration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION];
    }

    public long getAccNb2xx(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_2XX];
    }

    public double getAverage2xxDuration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_2XX], statistics[group][subGroup][STAT_ACC_NB_2XX]);
    }

    public long getMax2xxDuration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_2XX];
    }

    public long getAccNb3xx(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_3XX];
    }

    public double getAverage3xxDuration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_3XX], statistics[group][subGroup][STAT_ACC_NB_3XX]);
    }

    public long getMax3xxDuration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_3XX];
    }

    public long getAccNb404(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_404];
    }

    public double getAverage404Duration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_404], statistics[group][subGroup][STAT_ACC_NB_404]);
    }

    public long getMax404Duration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_404];
    }

    public long getAccNb408(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_408];
    }

    public double getAverage408Duration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_408], statistics[group][subGroup][STAT_ACC_NB_408]);
    }

    public long getMax408Duration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_408];
    }

    public long getAccNb409(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_409];
    }

    public double getAverage409Duration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_409], statistics[group][subGroup][STAT_ACC_NB_409]);
    }

    public long getMax409Duration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_409];
    }

    public long getAccNb4xx(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_4XX];
    }

    public double getAverage4xxDuration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_4XX], statistics[group][subGroup][STAT_ACC_NB_4XX]);
    }

    public long getMax4xxDuration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_4XX];
    }

    public long getAccNb504(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_504];
    }

    public double getAverage504Duration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_504], statistics[group][subGroup][STAT_ACC_NB_504]);
    }

    public long getMax504Duration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_504];
    }

    public long getAccNb5xx(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_5XX];
    }

    public double getAverage5xxDuration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_5XX], statistics[group][subGroup][STAT_ACC_NB_5XX]);
    }

    public long getMax5xxDuration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_5XX];
    }

    public long getAccNbXxx(int group, int subGroup) {
        return statistics[group][subGroup][STAT_ACC_NB_XXX];
    }

    public double getAverageXxxDuration(int group, int subGroup) {
        return getRoundedRatio(statistics[group][subGroup][STAT_ACC_DURATION_XXX], statistics[group][subGroup][STAT_ACC_NB_XXX]);
    }

    public long getMaxXxxDuration(int group, int subGroup) {
        return statistics[group][subGroup][STAT_MAX_DURATION_XXX];
    }

    private int getMethodId(String method) {
        if (SongServletRequest.MD_CREATE.equals(method)) {
            return SUB_GROUP_CREATE;
        } else if (SongServletRequest.MD_RETRIEVE.equals(method)) {
            return SUB_GROUP_RETRIEVE;
        } else if (SongServletRequest.MD_UPDATE.equals(method)) {
            return SUB_GROUP_UPDATE;
        } else if (SongServletRequest.MD_DELETE.equals(method)) {
            return SUB_GROUP_DELETE;
        }
        return SUB_GROUP_UNKNOWN;
    }

    private void sendRequest(int group, int subGroup) {
        statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_REQUESTS]++;
        statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_REQUESTS]++;
        statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_REQUESTS]++;
        statistics[group][subGroup][STAT_ACC_NB_REQUESTS]++;

        statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_NB_REQUESTS]++;
        statistics[GROUP_GLOBAL][subGroup][STAT_NB_REQUESTS]++;
        statistics[group][SUB_GROUP_GLOBAL][STAT_NB_REQUESTS]++;
        statistics[group][subGroup][STAT_NB_REQUESTS]++;
    }

    public synchronized void sendLocalRequest(String method) {
        sendRequest(GROUP_LOCAL, getMethodId(method));
    }

    public synchronized void sendRemoteRequest(String method) {
        sendRequest(GROUP_REMOTE, getMethodId(method));
    }

    public synchronized void sendForwardRequest(String method) {
        sendRequest(GROUP_FORWARD, getMethodId(method));
    }

    private void receivedResponse(int group, int subGroup, long duration, int status) {
        statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_RESPONSES]++;
        statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_RESPONSES]++;
        statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_RESPONSES]++;
        statistics[group][subGroup][STAT_ACC_NB_RESPONSES]++;

        statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION] += duration;
        statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION] += duration;
        statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION] += duration;
        statistics[group][subGroup][STAT_ACC_DURATION] += duration;

        if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION] < duration) {
            statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION] = duration;
        }
        if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION] < duration) {
            statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION] = duration;
        }
        if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION] < duration) {
            statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION] = duration;
        }
        if (statistics[group][subGroup][STAT_MAX_DURATION] < duration) {
            statistics[group][subGroup][STAT_MAX_DURATION] = duration;
        }

        int statusGroup = status / 100;
        switch (statusGroup) {
        case 2:
            statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_2XX]++;
            statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_2XX]++;
            statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_2XX]++;
            statistics[group][subGroup][STAT_ACC_NB_2XX]++;

            statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_2XX] += duration;
            statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_2XX] += duration;
            statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_2XX] += duration;
            statistics[group][subGroup][STAT_ACC_DURATION_2XX] += duration;

            if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_2XX] < duration) {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_2XX] = duration;
            }
            if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_2XX] < duration) {
                statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_2XX] = duration;
            }
            if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_2XX] < duration) {
                statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_2XX] = duration;
            }
            if (statistics[group][subGroup][STAT_MAX_DURATION_2XX] < duration) {
                statistics[group][subGroup][STAT_MAX_DURATION_2XX] = duration;
            }
            break;
        case 3:
            statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_3XX]++;
            statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_3XX]++;
            statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_3XX]++;
            statistics[group][subGroup][STAT_ACC_NB_3XX]++;

            statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_3XX] += duration;
            statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_3XX] += duration;
            statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_3XX] += duration;
            statistics[group][subGroup][STAT_ACC_DURATION_3XX] += duration;

            if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_3XX] < duration) {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_3XX] = duration;
            }
            if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_3XX] < duration) {
                statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_3XX] = duration;
            }
            if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_3XX] < duration) {
                statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_3XX] = duration;
            }
            if (statistics[group][subGroup][STAT_MAX_DURATION_3XX] < duration) {
                statistics[group][subGroup][STAT_MAX_DURATION_3XX] = duration;
            }
            break;
        case 4:
            if (status == 404) {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_404]++;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_404]++;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_404]++;
                statistics[group][subGroup][STAT_ACC_NB_404]++;

                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_404] += duration;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_404] += duration;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_404] += duration;
                statistics[group][subGroup][STAT_ACC_DURATION_404] += duration;

                if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_404] < duration) {
                    statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_404] = duration;
                }
                if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_404] < duration) {
                    statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_404] = duration;
                }
                if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_404] < duration) {
                    statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_404] = duration;
                }
                if (statistics[group][subGroup][STAT_MAX_DURATION_404] < duration) {
                    statistics[group][subGroup][STAT_MAX_DURATION_404] = duration;
                }
            } else if (status == 408) {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_408]++;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_408]++;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_408]++;
                statistics[group][subGroup][STAT_ACC_NB_408]++;

                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_408] += duration;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_408] += duration;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_408] += duration;
                statistics[group][subGroup][STAT_ACC_DURATION_408] += duration;

                if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_408] < duration) {
                    statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_408] = duration;
                }
                if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_408] < duration) {
                    statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_408] = duration;
                }
                if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_408] < duration) {
                    statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_408] = duration;
                }
                if (statistics[group][subGroup][STAT_MAX_DURATION_408] < duration) {
                    statistics[group][subGroup][STAT_MAX_DURATION_408] = duration;
                }
            } else if (status == 409) {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_409]++;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_409]++;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_409]++;
                statistics[group][subGroup][STAT_ACC_NB_409]++;

                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_409] += duration;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_409] += duration;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_409] += duration;
                statistics[group][subGroup][STAT_ACC_DURATION_409] += duration;

                if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_409] < duration) {
                    statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_409] = duration;
                }
                if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_409] < duration) {
                    statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_409] = duration;
                }
                if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_409] < duration) {
                    statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_409] = duration;
                }
                if (statistics[group][subGroup][STAT_MAX_DURATION_409] < duration) {
                    statistics[group][subGroup][STAT_MAX_DURATION_409] = duration;
                }
            } else {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_4XX]++;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_4XX]++;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_4XX]++;
                statistics[group][subGroup][STAT_ACC_NB_4XX]++;

                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_4XX] += duration;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_4XX] += duration;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_4XX] += duration;
                statistics[group][subGroup][STAT_ACC_DURATION_4XX] += duration;

                if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_4XX] < duration) {
                    statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_4XX] = duration;
                }
                if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_4XX] < duration) {
                    statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_4XX] = duration;
                }
                if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_4XX] < duration) {
                    statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_4XX] = duration;
                }
                if (statistics[group][subGroup][STAT_MAX_DURATION_4XX] < duration) {
                    statistics[group][subGroup][STAT_MAX_DURATION_4XX] = duration;
                }
            }
            break;
        case 5:
            if (status == 504) {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_504]++;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_504]++;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_504]++;
                statistics[group][subGroup][STAT_ACC_NB_504]++;

                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_504] += duration;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_504] += duration;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_504] += duration;
                statistics[group][subGroup][STAT_ACC_DURATION_504] += duration;

                if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_504] < duration) {
                    statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_504] = duration;
                }
                if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_504] < duration) {
                    statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_504] = duration;
                }
                if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_504] < duration) {
                    statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_504] = duration;
                }
                if (statistics[group][subGroup][STAT_MAX_DURATION_504] < duration) {
                    statistics[group][subGroup][STAT_MAX_DURATION_504] = duration;
                }
            } else {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_5XX]++;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_5XX]++;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_5XX]++;
                statistics[group][subGroup][STAT_ACC_NB_5XX]++;

                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_5XX] += duration;
                statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_5XX] += duration;
                statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_5XX] += duration;
                statistics[group][subGroup][STAT_ACC_DURATION_5XX] += duration;

                if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_5XX] < duration) {
                    statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_5XX] = duration;
                }
                if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_5XX] < duration) {
                    statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_5XX] = duration;
                }
                if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_5XX] < duration) {
                    statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_5XX] = duration;
                }
                if (statistics[group][subGroup][STAT_MAX_DURATION_5XX] < duration) {
                    statistics[group][subGroup][STAT_MAX_DURATION_5XX] = duration;
                }
            }
            break;
        default:
            statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_NB_XXX]++;
            statistics[GROUP_GLOBAL][subGroup][STAT_ACC_NB_XXX]++;
            statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_NB_XXX]++;
            statistics[group][subGroup][STAT_ACC_NB_XXX]++;

            statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_XXX] += duration;
            statistics[GROUP_GLOBAL][subGroup][STAT_ACC_DURATION_XXX] += duration;
            statistics[group][SUB_GROUP_GLOBAL][STAT_ACC_DURATION_XXX] += duration;
            statistics[group][subGroup][STAT_ACC_DURATION_XXX] += duration;

            if (statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_XXX] < duration) {
                statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_XXX] = duration;
            }
            if (statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_XXX] < duration) {
                statistics[GROUP_GLOBAL][subGroup][STAT_MAX_DURATION_XXX] = duration;
            }
            if (statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_XXX] < duration) {
                statistics[group][SUB_GROUP_GLOBAL][STAT_MAX_DURATION_XXX] = duration;
            }
            if (statistics[group][subGroup][STAT_MAX_DURATION_XXX] < duration) {
                statistics[group][subGroup][STAT_MAX_DURATION_XXX] = duration;
            }
            break;
        }

        statistics[GROUP_GLOBAL][SUB_GROUP_GLOBAL][STAT_NB_REQUESTS]--;
        statistics[GROUP_GLOBAL][subGroup][STAT_NB_REQUESTS]--;
        statistics[group][SUB_GROUP_GLOBAL][STAT_NB_REQUESTS]--;
        statistics[group][subGroup][STAT_NB_REQUESTS]--;
    }

    public synchronized void receivedLocalResponse(long duration, String method, int status) {
        receivedResponse(GROUP_LOCAL, getMethodId(method), duration, status);
    }

    public synchronized void receivedRemoteResponse(long duration, String method, int status) {
        receivedResponse(GROUP_REMOTE, getMethodId(method), duration, status);
    }

    public synchronized void receivedForwardResponse(long duration, String method, int status) {
        receivedResponse(GROUP_FORWARD, getMethodId(method), duration, status);
    }
}
