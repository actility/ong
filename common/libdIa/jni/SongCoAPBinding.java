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
 * version $Revision: 17 $
 * lastrevision $Date: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 */

package com.actility.servlet.song.coap;

import java.io.IOException;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.servlet.song.SongBindingFactory;
import com.actility.servlet.song.SongServlet;
import com.actility.servlet.song.SongServletRequest;
import com.actility.servlet.song.SongServletResponse;
import com.actility.servlet.song.SongServletMessage;
import com.actility.servlet.song.SongURI;

class diaOpt {
	String	name;
	String	val;

	void diaOpt (String n, String v) {
		name	= n;
		val	= v;
	}
}


class diajni implements Runnable {
	int	diaptr;
	SongCoAPBinding song;

	public native void diaInit();
	public native void diaPoll();
	public native void diaRun();
	public native void diaStop();
	public native void diaFree();

	public native void diaCreateResponse(String status, String resourceURI, byte[] content,
		String contentType, diaOpt[] optHeader, int tid);
	public native void diaRetrieveResponse(String status, byte[] content, String contentType,
		diaOpt[] optHeader, int tid);
	public native void diaUpdateResponse(String status, String content, String contentType, diaOpt[] optHeader, int tid);
	public native void diaDeleteResponse(String status, diaOpt[] optHeader, int tid);

	public native int diaCreateRequest(String reqEntity, String targetID, String content, String contentType, diaOpt[] optAttr, diaOpt[] optHeader);
	public native int diaRetrieveRequest(String reqEntity, String targetID, diaOpt[] optAttr, diaOpt[] optHeader);
	public native int diaUpdateRequest(String reqEntity, String targetID, String content, String contentType, diaOpt[] optAttr, diaOpt[] optHeader);
	public native int diaDeleteRequest(String reqEntity, String targetID, diaOpt[] optAttr, diaOpt[] optHeader);

	public void run() {
		diaInit ();
		diaRun();	// Loop : never returns
	}

	//	Boot time
	static {
		System.err.println ("CoAP loading diajni\n");
		try {
		System.loadLibrary("diajni");
		} catch (UnsatisfiedLinkError e) { System.err.println("Native code library failed to load.\n" + e); }
	}

	// dIa callbacks
	public void cb_diaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType,
		diaOpt[] optAttr, diaOpt[] optHeader, int tid)
	{
		try {
		SongURI uri = song.songBindingFactory.createURI(reqEntity);
		SongServletRequest request = song.songBindingFactory.createRequest("song", "POST", uri, null,
			null, 0, null, 0, false);
		// request.addHeader(name, value);
		request.setContent(content, contentType);
		request.setAttribute("diatid", new Integer(tid));
		request.setAttribute("quoi", "create");
		request.send();
		} catch (Exception e) {}

		byte[] payload = "<obj />".getBytes();
		diaCreateResponse("STATUS_OK", "uri", payload, "application/xml", null, tid);
	}
}




/**
 * Concrete binding which manages translation between HTTP messages and SONG messages.
 */
public final class SongCoAPBinding extends SongServlet {
	private static final Logger LOG = Logger.getLogger(SongCoAPBinding.class);

	private ServletContext servletContext;
	SongBindingFactory songBindingFactory;
	private volatile boolean end;

	diajni dia;

	/**
	 * Builds the CoAP SONG Binding.
	 *
	 */
	public SongCoAPBinding() {
		this.end = false;
	}

	public void init() {
		servletContext = getServletContext();
		songBindingFactory = (SongBindingFactory) servletContext.getAttribute(SONG_BINDING_FACTORY);

	dia = new diajni();
	dia.song = this;
	new Thread(dia).start();
	}

	/**
	 * Stops and deletes the CoAP Song binding.
	 */
	public void destroy() {
		end = true;
	dia.diaStop ();
	}

	/*
	 * CoAP Servlet part
	 */
	public void doCoAPCreate() throws IOException {
		// TODO build and send SONG Create request

		// SongServletRequest request = songBindingFactory.createRequest(protocol, method, requestingEntity, targetID,
		// localAddress, localPort, remoteAddress, remotePort, proxyRequest)
		// request.addHeader(name, value);
		// request.setContent(content, contentType);
		// request.setAttribute("myRequest", coapRequest);
		// request.send();
	}

	public void doCoAPRetrieve() throws IOException {
		// TODO build and send SONG Retrieve request
	}

	public void doCoAPUpdate() throws IOException {
		// TODO build and send SONG Update request
	}

	public void doCoAPDelete() throws IOException {
		// TODO build and send SONG Delete request
	}

	public void doCoAPProvisionalResponse() throws IOException {
		// Ignore this for now
	}

	public void doCoAPSuccessResponse() throws IOException {
		// TODO build and send SONG Success response
	}

	public void doCoAPErrorResponse() throws IOException {
		// TODO build and send SONG Error response
	}

	/*
	 * SONG Servlet part
	 */
	public void doCreate(SongServletRequest songRequest) throws ServletException, IOException {
		// TODO build and send CoAP Create request
	}

	public void doRetrieve(SongServletRequest songRequest) throws ServletException, IOException {
		// TODO build and send CoAP Retrieve request
	}

	public void doUpdate(SongServletRequest songRequest) throws ServletException, IOException {
		// TODO build and send CoAP Update request
	}

	public void doDelete(SongServletRequest songRequest) throws ServletException, IOException {
		// TODO build and send CoAP Delete request
	}

	public void doProvisionalResponse(SongServletResponse songResponse) throws ServletException, IOException {
		// Ignore this for now
	}

	public void doSuccessResponse(SongServletResponse songResponse) throws ServletException, IOException {
		Integer attr = (Integer)songResponse.getRequest().getAttribute("diatid");
		Integer quoi = (Integer)songResponse.getRequest().getAttribute("quoi");
		int tid = attr.intValue();

		if (quoi.compareTo("create") == 0) {
			String resourceURI = songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION);
			byte[] payload = songResponse.getRawContent();
			String ctype = songResponse.getContentType();
			dia.diaCreateResponse("STATUS_OK", resourceURI, payload, ctype, null, tid);
		}
	}

	public void doErrorResponse(SongServletResponse songResponse) throws ServletException, IOException {
		// TODO build and send SONG Error response

		// CoAPRequest coapRequest = songResponse.getRequest().getAttribute("myRequest");
		// coapRequest.sendResponse();
	}
}
