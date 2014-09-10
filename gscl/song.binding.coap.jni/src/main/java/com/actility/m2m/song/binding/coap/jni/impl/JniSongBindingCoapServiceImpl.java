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

package com.actility.m2m.song.binding.coap.jni.impl;

import org.apache.log4j.Logger;

import com.actility.m2m.song.binding.coap.jni.log.BundleLogger;
import com.actility.m2m.song.binding.coap.ni.api.DiaOpt;
import com.actility.m2m.song.binding.coap.ni.api.NiSongBindingCoapService;
import com.actility.m2m.song.binding.coap.ni.api.NiSongBindingCoapServiceCallback;
import com.actility.m2m.util.log.OSGiLogger;

public class JniSongBindingCoapServiceImpl implements NiSongBindingCoapService {

    private static final Logger LOG = OSGiLogger.getLogger(JniSongBindingCoapServiceImpl.class, BundleLogger.LOG);
    private final DiaJni diaJni;
    private Thread diaThread;

    public JniSongBindingCoapServiceImpl(String host, String port, int level, String logFile, int sizemax) {
        diaJni = new DiaJni(host, port, level, logFile, sizemax);
    }

    public void diaStart() throws InterruptedException {
        synchronized (diaJni) {
            diaThread = new Thread(diaJni);
            diaThread.start();
            diaJni.wait();
        }
        if (diaJni.e != null) {
            throw diaJni.e;
        }
    }

    public void diaStop() {
        diaJni.diaStop(diaJni.diaPtr);
        try {
            diaThread.join();
        } catch (InterruptedException e) {
            LOG.error("Thread interrupted while joining dIa native code thread", e);
        }
    }

    public void registerCallback(NiSongBindingCoapServiceCallback cb) {
        diaJni.registerCallback(cb);
    }

    public void unregisterCallback(NiSongBindingCoapServiceCallback cb) {
        diaJni.unregisterCallback(cb);
    }

    public void diaEnterCS() {
        diaJni.diaEnterCS(diaJni.diaPtr);
    }

    public void diaLeaveCS() {
        diaJni.diaLeaveCS(diaJni.diaPtr);
    }

    public void diaCreateResponse(String status, String resourceURI, byte[] content, String contentType, DiaOpt[] optHeader,
            int tid, String peer) {
        diaJni.diaCreateResponse(diaJni.diaPtr, status, resourceURI, content, contentType, optHeader, tid, peer);
    }

    public void diaRetrieveResponse(String status, byte[] content, String contentType, DiaOpt[] optHeader, int tid,
            String peer) {
        diaJni.diaRetrieveResponse(diaJni.diaPtr, status, content, contentType, optHeader, tid, peer);
    }

    public void diaUpdateResponse(String status, byte[] content, String contentType, DiaOpt[] optHeader, int tid, String peer) {
        diaJni.diaUpdateResponse(diaJni.diaPtr, status, content, contentType, optHeader, tid, peer);
    }

    public void diaDeleteResponse(String status, DiaOpt[] optHeader, int tid, String peer) {
        diaJni.diaDeleteResponse(diaJni.diaPtr, status, optHeader, tid, peer);
    }

    public int diaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr,
            DiaOpt[] optHeader) {
        return diaJni.diaCreateRequest(diaJni.diaPtr, reqEntity, targetID, content, contentType, optAttr, optHeader);
    }

    public int diaRetrieveRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader) {
        return diaJni.diaRetrieveRequest(diaJni.diaPtr, reqEntity, targetID, optAttr, optHeader);
    }

    public int diaUpdateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr,
            DiaOpt[] optHeader) {
        return diaJni.diaUpdateRequest(diaJni.diaPtr, reqEntity, targetID, content, contentType, optAttr, optHeader);
    }

    public int diaDeleteRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader) {
        return diaJni.diaDeleteRequest(diaJni.diaPtr, reqEntity, targetID, optAttr, optHeader);
    }
}
