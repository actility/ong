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
import org.apache.log4j.Level;

import com.actility.servlet.song.SongBindingFactory;
import com.actility.servlet.song.SongServlet;
import com.actility.servlet.song.SongServletRequest;
import com.actility.servlet.song.SongServletResponse;
import com.actility.servlet.song.SongServletMessage;
import com.actility.servlet.song.SongURI;

import com.actility.servlet.song.coap.DiaJni;
import com.actility.servlet.song.coap.DiaOpt;


/**
 * Concrete binding which manages translation between HTTP messages and SONG messages.
 */
public final class SongCoAPBinding extends SongServlet {
	private static final Logger logger = Logger.getLogger(SongCoAPBinding.class);
	private ServletContext servletContext;
	SongBindingFactory songBindingFactory;
	private volatile boolean end;

	DiaJni dia;

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

//		dia = new DiaJni("::", "5683");
		dia = new DiaJni();
		dia.diaTrace ("9", "/tmp/DiaJni", null);
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


	public String song2dia_code(int code) {
		switch (code) {
		case 200 : return "STATUS_OK";
		case 201 : return "STATUS_CREATED";
		case 202 : return "STATUS_DELETED";
		case 204 : return "STATUS_OK";
		case 205 : return "STATUS_OK";
		case 400 : return "STATUS_BAD_REQUEST";
		case 401 : return "STATUS_PERMISSION_DENIED";
		case 404 : return "STATUS_NOT_FOUND";
		case 405 : return "STATUS_METHOD_NOT_ALLOWED";
		case 406 : return "STATUS_NOT_ACCEPTABLE";
		case 409 : return "STATUS_CONFLICT";
		case 415 : return "STATUS_UNSUPPORTED_MEDIA_TYPE";
		case 500 : return "STATUS_INTERNAL_SERVER_ERROR";
		case 501 : return "STATUS_NOT_IMPLEMENTED";
		case 502 : return "STATUS_BAD_GATEWAY";
		case 503 : return "STATUS_SERVICE_UNAVAILABLE";
		case 504 : return "STATUS_GATEWAY_TIMEOUT";
		default : return	"STATUS_BAD_REQUEST";
		}
	}

	public void doCreate(SongServletRequest songRequest) throws ServletException, IOException {
		int tid = dia.diaCreateRequest(songRequest.getRequestingEntity().absoluteURI(),
			songRequest.getTargetID().absoluteURI(), songRequest.getRawContent(),
			songRequest.getContentType(), null, null);
		songRequest.setAttribute("diatid", new Integer(tid));
		dia.transactionIDs.put (new Integer(tid), songRequest);
	}

	public void doRetrieve(SongServletRequest songRequest) throws ServletException, IOException {
		int tid = dia.diaRetrieveRequest(songRequest.getRequestingEntity().absoluteURI(),
			songRequest.getTargetID().absoluteURI(), null, null);
		songRequest.setAttribute("diatid", new Integer(tid));
		dia.transactionIDs.put (new Integer(tid), songRequest);
	}

	public void doUpdate(SongServletRequest songRequest) throws ServletException, IOException {
		int tid = dia.diaUpdateRequest(songRequest.getRequestingEntity().absoluteURI(),
			songRequest.getTargetID().absoluteURI(), songRequest.getRawContent(),
			songRequest.getContentType(), null, null);
		songRequest.setAttribute("diatid", new Integer(tid));
		dia.transactionIDs.put (new Integer(tid), songRequest);
	}

	public void doDelete(SongServletRequest songRequest) throws ServletException, IOException {
		int tid = dia.diaDeleteRequest(songRequest.getRequestingEntity().absoluteURI(),
			songRequest.getTargetID().absoluteURI(), null, null);
		songRequest.setAttribute("diatid", new Integer(tid));
		dia.transactionIDs.put (new Integer(tid), songRequest);
	}

	public void doProvisionalResponse(SongServletResponse songResponse) throws ServletException, IOException {
		// Ignore this for now
	}

	public void doSuccessResponse(SongServletResponse songResponse) throws ServletException, IOException {
		SongServletRequest request = songResponse.getRequest();
		Integer attr = (Integer)request.getAttribute("diatid");
		String quoi = (String)request.getAttribute("quoi");
		String peer = (String)request.getAttribute("diapeer");
		int tid = attr.intValue();

		logger.log(Level.ERROR, "doSuccessResponse "+quoi+" "+songResponse.getStatus()+" tid="+tid+" peer="+peer);

		if (quoi.compareTo("create") == 0) {
			String resourceURI = songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION);
			byte[] payload = songResponse.getRawContent();
			String ctype = songResponse.getContentType();
			dia.diaCreateResponse(song2dia_code(songResponse.getStatus()), resourceURI, payload, ctype, null, tid, peer);
		}
		else if (quoi.compareTo("retrieve") == 0) {
			byte[] payload = songResponse.getRawContent();
			String ctype = songResponse.getContentType();
//			logger.log(Level.ERROR, "ctype="+ctype);
			dia.diaRetrieveResponse(song2dia_code(songResponse.getStatus()), payload, ctype, null, tid, peer);
		}
		else if (quoi.compareTo("update") == 0) {
			byte[] payload = songResponse.getRawContent();
			String ctype = songResponse.getContentType();
			dia.diaUpdateResponse(song2dia_code(songResponse.getStatus()), payload, ctype, null, tid, peer);
		}
		else if (quoi.compareTo("delete") == 0) {
			dia.diaDeleteResponse(song2dia_code(songResponse.getStatus()), null, tid, peer);
		}
	}

	public void doErrorResponse(SongServletResponse songResponse) throws ServletException, IOException {
		doSuccessResponse (songResponse);
	}
}
