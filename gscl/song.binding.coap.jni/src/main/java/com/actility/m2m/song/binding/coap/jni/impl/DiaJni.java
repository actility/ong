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
 * id $Id: DiaJni.java 5452 2013-07-22 09:02:34Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 5452 $
 * lastrevision $Date: 2013-07-22 11:02:34 +0200 (Mon, 22 Jul 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-07-22 11:02:34 +0200 (Mon, 22 Jul 2013) $
 */

package com.actility.m2m.song.binding.coap.jni.impl;

import java.io.File;
import java.io.IOException;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

import com.actility.m2m.song.binding.coap.jni.log.BundleLogger;
import com.actility.m2m.song.binding.coap.ni.api.DiaOpt;
import com.actility.m2m.song.binding.coap.ni.api.NiSongBindingCoapServiceCallback;
import com.actility.m2m.util.log.OSGiLogger;

public final class DiaJni implements Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(DiaJni.class, BundleLogger.LOG);

    private static final Level[] LEVELS = { Level.FATAL, Level.ERROR, Level.WARN, Level.INFO, Level.DEBUG, Level.TRACE };

    // Boot time
    static {
        try {
            System.loadLibrary("DiaJni");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    private NiSongBindingCoapServiceCallback bindingCb;
    public RuntimeException e;
    public final long diaPtr;

    public DiaJni(String serverAddress, String serverPort, int diaTraceLevel,
            String diaLogFile, int diaMaxLogFileSize) {
        String realLogFile = null;
        try {
            realLogFile = new File(diaLogFile).getCanonicalPath();
        } catch (IOException e) {
            LOG.error("Cannot resolve log file destination for dIa", e);
        }
        diaPtr = diaInit(serverAddress, serverPort, diaTraceLevel, realLogFile, diaMaxLogFileSize);
        if (diaPtr == -1) {
            throw new IllegalStateException("Could not initialize dIa stack");
        }
    }

    public void registerCallback(NiSongBindingCoapServiceCallback bindingCb) {
        this.bindingCb = bindingCb;
    }

    public void unregisterCallback(NiSongBindingCoapServiceCallback bindingCb) {
        if (this.bindingCb == bindingCb) {
            this.bindingCb = null;
        }
    }

    public native long diaInit(String host, String port, int level, String logFile, int sizemax);

    public native void diaTrace(int level);

    public native int diaPrepare();

    public native void diaRun(long diaPtr);

    public native void diaStop(long diaPtr);

    public native void diaEnterCS(long diaPtr);

    public native void diaLeaveCS(long diaPtr);

    public native void diaCreateResponse(long diaPtr, String status, String resourceURI, byte[] content, String contentType,
            DiaOpt[] optHeader, int tid, String peer);

    public native void diaRetrieveResponse(long diaPtr, String status, byte[] content, String contentType,
            DiaOpt[] optHeader, int tid, String peer);

    public native void diaUpdateResponse(long diaPtr, String status, byte[] content, String contentType,
            DiaOpt[] optHeader, int tid, String peer);

    public native void diaDeleteResponse(long diaPtr, String status, DiaOpt[] optHeader, int tid, String peer);

    public native int diaCreateRequest(long diaPtr, String reqEntity, String targetID, byte[] content, String contentType,
            DiaOpt[] optAttr, DiaOpt[] optHeader);

    public native int diaRetrieveRequest(long diaPtr, String reqEntity, String targetID, DiaOpt[] optAttr,
            DiaOpt[] optHeader);

    public native int diaUpdateRequest(long diaPtr, String reqEntity, String targetID, byte[] content, String contentType,
            DiaOpt[] optAttr, DiaOpt[] optHeader);

    public native int diaDeleteRequest(long diaPtr, String reqEntity, String targetID, DiaOpt[] optAttr,
            DiaOpt[] optHeader);

    public void run() {
        try {
            if (diaPrepare() < 0) {
                this.e = new IllegalStateException("Cannot prepare dIa stack");
            }
        } catch (RuntimeException e) {
            this.e = e;
        }
        synchronized (this) {
            notify();
        }
        if (e == null) {
            try {
                diaRun(diaPtr);
            } catch (RuntimeException e) {
                LOG.error("Problem while running dIa stack", e);
            }
        }
    }

    public void cbDiaRetrieveRequest(String reqEntity, String targetID, DiaOptImpl[] optAttr, DiaOptImpl[] optHeader, int tid,
            String peer, boolean proxyRequest) {
        LOG.debug("cbDiaRetrieveRequest");
        bindingCb.doDiaRetrieveRequest(reqEntity, targetID, optAttr, optHeader, tid, peer, proxyRequest);
    }

    public void cbDiaRetrieveResponse(String scode, byte[] content, String contentType, DiaOptImpl[] optHeader, int tid) {
        LOG.debug("cbDiaRetrieveResponse");
        bindingCb.doDiaRetrieveResponse(scode, content, contentType, optHeader, tid);
    }

    public void cbDiaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOptImpl[] optAttr,
            DiaOptImpl[] optHeader, int tid, String peer, boolean proxyRequest) {
        LOG.debug("cbDiaCreateRequest");
        bindingCb.doDiaCreateRequest(reqEntity, targetID, content, contentType, optAttr, optHeader, tid, peer, proxyRequest);
    }

    public void cbDiaCreateResponse(String scode, String resourceUri, byte[] content, String contentType,
            DiaOptImpl[] optHeader, int tid) {
        LOG.debug("cbDiaCreateResponse");
        bindingCb.doDiaCreateResponse(scode, resourceUri, content, contentType, optHeader, tid);
    }

    public void cbDiaUpdateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOptImpl[] optAttr,
            DiaOptImpl[] optHeader, int tid, String peer, boolean proxyRequest) {
        LOG.debug("cbDiaUpdateRequest");
        bindingCb.doDiaUpdateRequest(reqEntity, targetID, content, contentType, optAttr, optHeader, tid, peer, proxyRequest);
    }

    public void cbDiaUpdateResponse(String scode, byte[] content, String contentType, DiaOptImpl[] optHeader, int tid) {
        LOG.debug("cbDiaUpdateResponse");
        bindingCb.doDiaUpdateResponse(scode, content, contentType, optHeader, tid);
    }

    public void cbDiaDeleteRequest(String reqEntity, String targetID, DiaOptImpl[] optAttr, DiaOptImpl[] optHeader, int tid,
            String peer, boolean proxyRequest) {
        LOG.debug("cbDiaDeleteRequest");
        bindingCb.doDiaDeleteRequest(reqEntity, targetID, optAttr, optHeader, tid, peer, proxyRequest);
    }

    public void cbDiaDeleteResponse(String scode, DiaOptImpl[] optHeader, int tid) {
        LOG.debug("cbDiaDeleteResponse");
        bindingCb.doDiaDeleteResponse(scode, optHeader, tid);
    }

    public void cbDiaErrorResponse(byte[] content, String contentType, DiaOptImpl[] optHeader, int tid) {
        LOG.debug("cbDiaErrorResponse");
        bindingCb.doDiaErrorResponse("STATUS_GATEWAY_TIMEOUT", content, contentType, optHeader, tid);
    }

    public void traceFunction(int level, String message) {
        LOG.debug("traceFunction");
        Level l;
        if (level > 5) {
            level = 5;
        }
        l = LEVELS[level];
        LOG.log(l, message);
    }
}
