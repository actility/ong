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
 * id $Id: SongCoAPBinding.java 9835 2014-10-31 16:33:57Z JReich $
 * author $Author: JReich $
 * version $Revision: 9835 $
 * lastrevision $Date: 2014-10-31 17:33:57 +0100 (Fri, 31 Oct 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-10-31 17:33:57 +0100 (Fri, 31 Oct 2014) $
 */

package com.actility.m2m.servlet.song.coap;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.binding.SongBindingFactory;
import com.actility.m2m.servlet.song.coap.log.BundleLogger;
import com.actility.m2m.song.binding.coap.ni.api.DiaOpt;
import com.actility.m2m.song.binding.coap.ni.api.NiSongBindingCoapService;
import com.actility.m2m.song.binding.coap.ni.api.NiSongBindingCoapServiceCallback;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Concrete binding which manages translation between CoAP messages and SONG messages.
 */
public final class SongCoAPBinding extends SongServlet implements NiSongBindingCoapServiceCallback {
    /**
     *
     */
    private static final long serialVersionUID = 648694131119669766L;

    private static final Logger LOG = OSGiLogger.getLogger(SongCoAPBinding.class, BundleLogger.getStaticLogger());

    public static final String DEFAULT_PROTOCOL = "CoAP/1.0";
    private static final Map DIA_CODE_TO_SONG_CODE = new HashMap();
    private static final String AT_DIA_TID = "diatid";
    private static final String AT_DIA_PEER = "diapeer";
//    private final long diaPtr;

    static {
        DIA_CODE_TO_SONG_CODE.put("STATUS_OK", new Integer(SongServletResponse.SC_OK));
        DIA_CODE_TO_SONG_CODE.put("STATUS_CREATED", new Integer(SongServletResponse.SC_CREATED));
        DIA_CODE_TO_SONG_CODE.put("STATUS_ACCEPTED", new Integer(SongServletResponse.SC_ACCEPTED));
        DIA_CODE_TO_SONG_CODE.put("STATUS_NO_CONTENT", new Integer(SongServletResponse.SC_NO_CONTENT));
        DIA_CODE_TO_SONG_CODE.put("STATUS_BAD_REQUEST", new Integer(SongServletResponse.SC_BAD_REQUEST));
        DIA_CODE_TO_SONG_CODE.put("STATUS_PERMISSION_DENIED", new Integer(SongServletResponse.SC_PERMISSION_DENIED));
        DIA_CODE_TO_SONG_CODE.put("STATUS_FORBIDDEN", new Integer(SongServletResponse.SC_FORBIDDEN));
        DIA_CODE_TO_SONG_CODE.put("STATUS_NOT_FOUND", new Integer(SongServletResponse.SC_NOT_FOUND));
        DIA_CODE_TO_SONG_CODE.put("STATUS_METHOD_NOT_ALLOWED", new Integer(SongServletResponse.SC_METHOD_NOT_ALLOWED));
        DIA_CODE_TO_SONG_CODE.put("STATUS_NOT_ACCEPTABLE", new Integer(SongServletResponse.SC_NOT_ACCEPTABLE));
        DIA_CODE_TO_SONG_CODE.put("STATUS_REQUEST_TIMEOUT", new Integer(SongServletResponse.SC_REQUEST_TIMEOUT));
        DIA_CODE_TO_SONG_CODE.put("STATUS_CONFLICT", new Integer(SongServletResponse.SC_CONFLICT));
        DIA_CODE_TO_SONG_CODE.put("STATUS_DELETED", new Integer(SongServletResponse.SC_DELETED));
        DIA_CODE_TO_SONG_CODE.put("STATUS_UNSUPPORTED_MEDIA_TYPE", new Integer(SongServletResponse.SC_UNSUPPORTED_MEDIA_TYPE));
        DIA_CODE_TO_SONG_CODE.put("STATUS_INTERNAL_SERVER_ERROR", new Integer(SongServletResponse.SC_INTERNAL_SERVER_ERROR));
        DIA_CODE_TO_SONG_CODE.put("STATUS_NOT_IMPLEMENTED", new Integer(SongServletResponse.SC_NOT_IMPLEMENTED));
        DIA_CODE_TO_SONG_CODE.put("STATUS_BAD_GATEWAY", new Integer(SongServletResponse.SC_BAD_GATEWAY));
        DIA_CODE_TO_SONG_CODE.put("STATUS_SERVICE_UNAVAILABLE", new Integer(SongServletResponse.SC_SERVICE_UNAVAILABLE));
        DIA_CODE_TO_SONG_CODE.put("STATUS_GATEWAY_TIMEOUT", new Integer(SongServletResponse.SC_GATEWAY_TIMEOUT));
    }

    private ServletContext servletContext;
    private SongBindingFactory songBindingFactory;
    private NiSongBindingCoapService dia;
    private Map transactionIDs;

    /**
     * Builds the CoAP SONG Binding.
     *
     */
    public SongCoAPBinding(NiSongBindingCoapService dia, String serverAddress, int serverPort) {
        transactionIDs = new HashMap();
        this.dia = dia;
    }

    public void init() {
        servletContext = getServletContext();
        songBindingFactory = (SongBindingFactory) servletContext.getAttribute(SONG_BINDING_FACTORY);
    }

    public void startDia() throws InterruptedException {
        dia.registerCallback(this);
    }

    /**
     * Stops and deletes the CoAP Song binding.
     */
    public void destroy() {
        dia.unregisterCallback(this);
    }

    public SongBindingFactory getSongBindingFactory() {
        return songBindingFactory;
    }

    public String getDiaCodeFromSongCode(int code) {
        switch (code) {
        case SongServletResponse.SC_OK:
            return "STATUS_OK";
        case SongServletResponse.SC_CREATED:
            return "STATUS_CREATED";
        case SongServletResponse.SC_ACCEPTED:
            return "STATUS_DELETED";
        case SongServletResponse.SC_NO_CONTENT:
            return "STATUS_OK";
        case 205: // ??
            return "STATUS_OK";
        case SongServletResponse.SC_BAD_REQUEST:
            return "STATUS_BAD_REQUEST";
        case SongServletResponse.SC_PERMISSION_DENIED:
            return "STATUS_PERMISSION_DENIED";
        case SongServletResponse.SC_FORBIDDEN:
            return "STATUS_FORBIDDEN";
        case SongServletResponse.SC_NOT_FOUND:
            return "STATUS_NOT_FOUND";
        case SongServletResponse.SC_METHOD_NOT_ALLOWED:
            return "STATUS_METHOD_NOT_ALLOWED";
        case SongServletResponse.SC_NOT_ACCEPTABLE:
            return "STATUS_NOT_ACCEPTABLE";
        case SongServletResponse.SC_REQUEST_TIMEOUT:
            return "STATUS_REQUEST_TIMEOUT";
        case SongServletResponse.SC_CONFLICT:
            return "STATUS_CONFLICT";
        case SongServletResponse.SC_DELETED:
            return "STATUS_DELETED";
        case SongServletResponse.SC_PRECONDITION_FAILED:
            return "STATUS_BAD_REQUEST";
        case SongServletResponse.SC_UNSUPPORTED_MEDIA_TYPE:
            return "STATUS_UNSUPPORTED_MEDIA_TYPE";
        case SongServletResponse.SC_INTERNAL_SERVER_ERROR:
            return "STATUS_INTERNAL_SERVER_ERROR";
        case SongServletResponse.SC_NOT_IMPLEMENTED:
            return "STATUS_NOT_IMPLEMENTED";
        case SongServletResponse.SC_BAD_GATEWAY:
            return "STATUS_BAD_GATEWAY";
        case SongServletResponse.SC_SERVICE_UNAVAILABLE:
            return "STATUS_SERVICE_UNAVAILABLE";
        case SongServletResponse.SC_GATEWAY_TIMEOUT:
            return "STATUS_GATEWAY_TIMEOUT";
        default:
            return "STATUS_BAD_REQUEST";
        }
    }

    public int getSongCodeFromDiaCode(String scode) {
        Integer songStatus = (Integer) DIA_CODE_TO_SONG_CODE.get(scode);
        if (songStatus != null) {
            return songStatus.intValue();
        }
        return SongServletResponse.SC_BAD_REQUEST;
    }

    public void doCreate(SongServletRequest songRequest) throws ServletException, IOException {
        try {
            dia.diaEnterCS();
            if (LOG.isInfoEnabled()) {
                LOG.info(songRequest.getId() + ": >>> CoAP.SONGBinding: CREATE (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            int tid = dia.diaCreateRequest(songRequest.getRequestingEntity().absoluteURI(), songRequest.getTargetID()
                    .absoluteURI(), songRequest.getRawContent(), songRequest.getContentType(), null, null);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: POST (requestingEntity: "
                                + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                                + songRequest.getTargetID().absoluteURI() + ")");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: POST (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            transactionIDs.put(new Integer(tid), songRequest);
        } finally {
            dia.diaLeaveCS();
        }
    }

    public void doRetrieve(SongServletRequest songRequest) throws ServletException, IOException {
        try {
            dia.diaEnterCS();
            if (LOG.isInfoEnabled()) {
                LOG.info(songRequest.getId() + ": >>> CoAP.SONGBinding: RETRIEVE (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            int tid = dia.diaRetrieveRequest(songRequest.getRequestingEntity().absoluteURI(), songRequest.getTargetID()
                    .absoluteURI(), null, null);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: GET (requestingEntity: "
                                + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                                + songRequest.getTargetID().absoluteURI() + ")");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: GET (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            transactionIDs.put(new Integer(tid), songRequest);
        } finally {
            dia.diaLeaveCS();
        }
    }

    public void doUpdate(SongServletRequest songRequest) throws ServletException, IOException {
        try {
            dia.diaEnterCS();
            if (LOG.isInfoEnabled()) {
                LOG.info(songRequest.getId() + ": >>> CoAP.SONGBinding: UPDATE (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            int tid = dia.diaUpdateRequest(songRequest.getRequestingEntity().absoluteURI(), songRequest.getTargetID()
                    .absoluteURI(), songRequest.getRawContent(), songRequest.getContentType(), null, null);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: PUT (requestingEntity: "
                                + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                                + songRequest.getTargetID().absoluteURI() + ")");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: PUT (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            transactionIDs.put(new Integer(tid), songRequest);
        } finally {
            dia.diaLeaveCS();
        }
    }

    public void doDelete(SongServletRequest songRequest) throws ServletException, IOException {
        try {
            dia.diaEnterCS();
            if (LOG.isInfoEnabled()) {
                LOG.info(songRequest.getId() + ": >>> CoAP.SONGBinding: DELETE (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            int tid = dia.diaDeleteRequest(songRequest.getRequestingEntity().absoluteURI(), songRequest.getTargetID()
                    .absoluteURI(), null, null);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: DELETE (requestingEntity: "
                                + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                                + songRequest.getTargetID().absoluteURI() + ")");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(tid + "/" + songRequest.getId() + ": <<< CoAP.CoAPBinding: DELETE (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            transactionIDs.put(new Integer(tid), songRequest);
        } finally {
            dia.diaLeaveCS();
        }
    }

    public void doProvisionalResponse(SongServletResponse songResponse) throws ServletException, IOException {
        // Ignore this for now
    }

    public void doResponse(SongServletResponse songResponse) throws ServletException, IOException {
        try {
            dia.diaEnterCS();
            if (LOG.isInfoEnabled()) {
                LOG.info(songResponse.getId() + ": >>> CoAP.SONGBinding: " + songResponse.getStatus() + " "
                        + songResponse.getReasonPhrase());
            }
            SongServletRequest request = songResponse.getRequest();
            Integer attr = (Integer) request.getAttribute(AT_DIA_TID);
            String peer = (String) request.getAttribute(AT_DIA_PEER);
            int tid = attr.intValue();
            String method = songResponse.getRequest().getMethod();
            String scode = getDiaCodeFromSongCode(songResponse.getStatus());

            if (SongServletRequest.MD_CREATE.equals(method)) {
                String resourceURI = songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION);
                byte[] payload = songResponse.getRawContent();
                String ctype = songResponse.getContentType();
                dia.diaCreateResponse(scode, resourceURI, payload, ctype, null, tid, peer);
            } else if (SongServletRequest.MD_RETRIEVE.equals(method)) {
                byte[] payload = songResponse.getRawContent();
                String ctype = songResponse.getContentType();
                // logger.log(Level.ERROR, "ctype="+ctype);
                dia.diaRetrieveResponse(getDiaCodeFromSongCode(songResponse.getStatus()), payload, ctype, null, tid,
                        peer);
            } else if (SongServletRequest.MD_UPDATE.equals(method)) {
                byte[] payload = songResponse.getRawContent();
                String ctype = songResponse.getContentType();
                dia.diaUpdateResponse(getDiaCodeFromSongCode(songResponse.getStatus()), payload, ctype, null, tid, peer);
            } else if (SongServletRequest.MD_DELETE.equals(method)) {
                dia.diaDeleteResponse(getDiaCodeFromSongCode(songResponse.getStatus()), null, tid, peer);
            }
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(songResponse.getId() + ": <<< CoAP.CoAPBinding: " + scode);
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(songResponse.getId() + ": <<< CoAP.CoAPBinding: " + scode);
            }
        } finally {
            dia.diaLeaveCS();
        }
    }

    public void doDiaRetrieveRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader, int tid,
            String peer, boolean proxyRequest) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    tid + ": >>> CoAP.CoAPBinding: GET (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: GET (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        LOG.debug("doDiaRetrieveRequest");
        try {
            SongURI song_reqEntity = songBindingFactory.createURI(reqEntity);
            SongURI song_targetID = songBindingFactory.createURI(targetID);
            SongServletRequest request = songBindingFactory.createRequest(DEFAULT_PROTOCOL, SongServletRequest.MD_RETRIEVE,
                    song_reqEntity, song_targetID, null, 0, null, 0, proxyRequest, String.valueOf(tid));
            // request.addHeader(name, value);
            request.setAttribute(SongCoAPBinding.AT_DIA_TID, new Integer(tid));
            request.setAttribute(SongCoAPBinding.AT_DIA_PEER, peer);
            if (LOG.isInfoEnabled()) {
                LOG.info(request.getId() + ": <<< CoAP.SONGBinding: RETRIEVE (requestingEntity: "
                        + song_reqEntity.absoluteURI() + ") (targetID: " + song_targetID.absoluteURI() + ")");
            }
            request.send();
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaRetrieveRequest", e);
        }
    }

    public void doDiaRetrieveResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        try {
            SongServletRequest request = (SongServletRequest) transactionIDs.remove(new Integer(tid));
            if (request != null) {
                SongServletResponse response = songBindingFactory.createResponse(request, getSongCodeFromDiaCode(scode), null,
                        null, 0, null, 0);
                response.setContent(content, contentType);
                if (LOG.isInfoEnabled()) {
                    LOG.info(response.getId() + ": <<< CoAP.SONGBinding: " + response.getStatus() + " "
                            + response.getReasonPhrase());
                }
                response.send();
            } else {
                LOG.error("Received a dIa response which does not correspond to any request: " + tid);
            }
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaRetrieveResponse", e);
        }
    }

    public void doDiaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr,
            DiaOpt[] optHeader, int tid, String peer, boolean proxyRequest) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    tid + ": >>> CoAP.CoAPBinding: POST (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: POST (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        try {
            SongURI song_reqEntity = songBindingFactory.createURI(reqEntity);
            SongURI song_targetID = songBindingFactory.createURI(targetID);
            SongServletRequest request = songBindingFactory.createRequest(DEFAULT_PROTOCOL, SongServletRequest.MD_CREATE,
                    song_reqEntity, song_targetID, null, 0, null, 0, proxyRequest, String.valueOf(tid));
            // request.addHeader(name, value);
            request.setContent(content, contentType);
            request.setAttribute(SongCoAPBinding.AT_DIA_TID, new Integer(tid));
            request.setAttribute(SongCoAPBinding.AT_DIA_PEER, peer);
            if (LOG.isInfoEnabled()) {
                LOG.info(request.getId() + ": <<< CoAP.SONGBinding: CREATE (requestingEntity: " + song_reqEntity.absoluteURI()
                        + ") (targetID: " + song_targetID.absoluteURI() + ")");
            }
            request.send();
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaCreateRequest", e);
        }
    }

    public void doDiaCreateResponse(String scode, String resourceUri, byte[] content, String contentType, DiaOpt[] optHeader,
            int tid) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        try {
            SongServletRequest request = (SongServletRequest) transactionIDs.remove(new Integer(tid));
            if (request != null) {
                SongServletResponse response = songBindingFactory.createResponse(request, getSongCodeFromDiaCode(scode), null,
                        null, 0, null, 0);
                response.setHeader(SongServletMessage.HD_CONTENT_LOCATION, resourceUri);
                response.setContent(content, contentType);
                if (LOG.isInfoEnabled()) {
                    LOG.info(response.getId() + ": <<< CoAP.SONGBinding: " + response.getStatus() + " "
                            + response.getReasonPhrase());
                }
                response.send();
            } else {
                LOG.error("Received a dIa response which does not correspond to any request: " + tid);
            }
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaCreateResponse", e);
        }
    }

    public void doDiaUpdateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr,
            DiaOpt[] optHeader, int tid, String peer, boolean proxyRequest) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    tid + ": >>> CoAP.CoAPBinding: PUT (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: PUT (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        try {
            SongURI song_reqEntity = songBindingFactory.createURI(reqEntity);
            SongURI song_targetID = songBindingFactory.createURI(targetID);
            SongServletRequest request = songBindingFactory.createRequest(DEFAULT_PROTOCOL, SongServletRequest.MD_UPDATE,
                    song_reqEntity, song_targetID, null, 0, null, 0, proxyRequest, String.valueOf(tid));
            // request.addHeader(name, value);
            request.setContent(content, contentType);
            request.setAttribute(SongCoAPBinding.AT_DIA_TID, new Integer(tid));
            request.setAttribute(SongCoAPBinding.AT_DIA_PEER, peer);
            if (LOG.isInfoEnabled()) {
                LOG.info(request.getId() + ": <<< CoAP.SONGBinding: UPDATE (requestingEntity: " + song_reqEntity.absoluteURI()
                        + ") (targetID: " + song_targetID.absoluteURI() + ")");
            }
            request.send();
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaUpdateRequest", e);
        }
    }

    public void doDiaUpdateResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        try {
            SongServletRequest request = (SongServletRequest) transactionIDs.remove(new Integer(tid));
            if (request != null) {
                int code = getSongCodeFromDiaCode(scode);
                if (code == 205) {
                    code = 204;
                }
                SongServletResponse response = songBindingFactory.createResponse(request, code, null, null, 0, null, 0);
                response.setContent(content, contentType);
                if (LOG.isInfoEnabled()) {
                    LOG.info(response.getId() + ": <<< CoAP.SONGBinding: " + response.getStatus() + " "
                            + response.getReasonPhrase());
                }
                response.send();
            } else {
                LOG.error("Received a dIa response which does not correspond to any request: " + tid);
            }
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaUpdateResponse", e);
        }
    }

    public void doDiaDeleteRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader, int tid,
            String peer, boolean proxyRequest) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    tid + ": >>> CoAP.CoAPBinding: DELETE (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: DELETE (requestingEntity: " + reqEntity + ") (targetID: " + targetID + ")");
        }
        try {
            SongURI song_reqEntity = songBindingFactory.createURI(reqEntity);
            SongURI song_targetID = songBindingFactory.createURI(targetID);
            SongServletRequest request = songBindingFactory.createRequest(DEFAULT_PROTOCOL, SongServletRequest.MD_DELETE,
                    song_reqEntity, song_targetID, null, 0, null, 0, proxyRequest, String.valueOf(tid));
            // request.addHeader(name, value);
            request.setAttribute(SongCoAPBinding.AT_DIA_TID, new Integer(tid));
            request.setAttribute(SongCoAPBinding.AT_DIA_PEER, peer);
            if (LOG.isInfoEnabled()) {
                LOG.info(request.getId() + ": <<< CoAP.SONGBinding: DELETE (requestingEntity: " + song_reqEntity.absoluteURI()
                        + ") (targetID: " + song_targetID.absoluteURI() + ")");
            }
            request.send();
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaDeleteRequest", e);
        }
    }

    public void doDiaDeleteResponse(String scode, DiaOpt[] optHeader, int tid) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        try {
            SongServletRequest request = (SongServletRequest) transactionIDs.remove(new Integer(tid));
            if (request != null) {
                SongServletResponse response = songBindingFactory.createResponse(request, getSongCodeFromDiaCode(scode), null,
                        null, 0, null, 0);
                if (LOG.isInfoEnabled()) {
                    LOG.info(response.getId() + ": <<< CoAP.SONGBinding: " + response.getStatus() + " "
                            + response.getReasonPhrase());
                }
                response.send();
            } else {
                LOG.error("Received a dIa response which does not correspond to any request: " + tid);
            }
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaDeleteResponse", e);
        }
    }

    public void doDiaErrorResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(tid + ": >>> CoAP.CoAPBinding: " + scode);
        }
        try {
            SongServletRequest request = (SongServletRequest) transactionIDs.remove(new Integer(tid));
            if (request != null) {
                SongServletResponse response = songBindingFactory.createResponse(request, getSongCodeFromDiaCode(scode), null,
                        null, 0, null, 0);
                response.setContent(content, contentType);
                if (LOG.isInfoEnabled()) {
                    LOG.info(response.getId() + ": <<< CoAP.SONGBinding: " + response.getStatus() + " "
                            + response.getReasonPhrase());
                }
                response.send();
            } else {
                LOG.error("Received a dIa response which does not correspond to any request: " + tid);
            }
        } catch (Exception e) {
            LOG.error("Exception while executing cb_diaErrorResponse", e);
        }
    }

}
