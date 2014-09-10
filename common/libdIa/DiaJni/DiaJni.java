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

import com.actility.servlet.song.coap.DiaOpt;
import com.actility.servlet.song.SongServletRequest;
import com.actility.servlet.song.SongServletResponse;
import com.actility.servlet.song.SongURI;
import java.util.Hashtable;
import org.apache.log4j.Logger;
import com.actility.util.log.LogUtil;
import org.apache.log4j.Level;
/*
FATAL : utilisé pour journaliser une erreur grave pouvant mener à l'arrêt prématuré de l'application ;
ERROR : utilisé pour journaliser une erreur qui n'empêche cependant pas l'application de fonctionner ;
WARN : utilisé pour journaliser un avertissement, il peut s'agir par exemple d'une incohérence dans la configuration, l'application peut continuer à fonctionner mais pas forcément de la façon attendue ;
INFO : utilisé pour journaliser des messages à caractère informatif (nom des fichiers, etc.) ;
DEBUG : utilisé pour générer des messages pouvant être utiles au débogage.
*/


public class DiaJni implements Runnable {
	int	diaptr;
	com.actility.servlet.song.coap.SongCoAPBinding song;
	public Hashtable transactionIDs;
	public String _host, _port;
	private static final Logger logger = Logger.getLogger(DiaJni.class);

	public native void diaInit(String host, String port);
	public native void diaTrace(String level, String files, String sizemax);
	public native void diaPoll();
	public native void diaRun();
	public native void diaStop();
	public native void diaFree();

	public native void diaCreateResponse(String status, String resourceURI, byte[] content, String contentType,
		DiaOpt[] optHeader, int tid, String peer);
	public native void diaRetrieveResponse(String status, byte[] content, String contentType,
		DiaOpt[] optHeader, int tid, String peer);
	public native void diaUpdateResponse(String status, byte[] content, String contentType,
		DiaOpt[] optHeader, int tid, String peer);
	public native void diaDeleteResponse(String status,
		DiaOpt[] optHeader, int tid, String peer);

	public native int diaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr, DiaOpt[] optHeader);
	public native int diaRetrieveRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader);
	public native int diaUpdateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr, DiaOpt[] optHeader);
	public native int diaDeleteRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader);

	public void DiaJni() {
		transactionIDs	= new Hashtable();
		_host = "::";
		_port = "5683";
	}

	public void DiaJni(String host, String port) {
		transactionIDs	= new Hashtable();
		_host = host;
		_port = port;
	}

	public void run() {
		diaInit (_host, _port);
		diaRun();	// Loop : never returns
	}

	//	Boot time
	static {
		try {
		System.loadLibrary("diajni");
		} catch (UnsatisfiedLinkError e) { System.err.println("Native code library failed to load.\n" + e); }
	}

/*
	SongServletRequest createRequest(String protocol, String method, SongURI requestingEntity, SongURI targetID,
	            InetAddress localAddress, int localPort, InetAddress remoteAddress, int remotePort, boolean proxyRequest);

	SongServletResponse createResponse(SongServletRequest request, int statusCode, String reasonPhrase,
		InetAddress localAddress, int localPort, InetAddress remoteAddress, int remotePort);
*/

	public int dia2song_code(String scode) {
		if	(scode.compareTo("STATUS_CREATED") == 0)		return 201;
		if	(scode.compareTo("STATUS_DELETED") == 0)		return 202;
		if	(scode.compareTo("STATUS_OK") == 0)			return 205;
		if	(scode.compareTo("STATUS_BAD_REQUEST") == 0)		return 400;
		if	(scode.compareTo("STATUS_PERMISSION_DENIED") == 0)	return 401;
		if	(scode.compareTo("STATUS_NOT_FOUND") == 0)		return 404;
		if	(scode.compareTo("STATUS_METHOD_NOT_ALLOWED") == 0)	return 405;
		if	(scode.compareTo("STATUS_NOT_ACCEPTABLE") == 0)	return 406;
		if	(scode.compareTo("STATUS_CONFLICT") == 0)		return 409;
		if	(scode.compareTo("STATUS_UNSUPPORTED_MEDIA_TYPE") == 0)return 415;
		if	(scode.compareTo("STATUS_INTERNAL_SERVER_ERROR") == 0)	return 500;
		if	(scode.compareTo("STATUS_NOT_IMPLEMENTED") == 0)	return 501;
		if	(scode.compareTo("STATUS_BAD_GATEWAY") == 0)		return 502;
		if	(scode.compareTo("STATUS_SERVICE_UNAVAILABLE") == 0)	return 503;
		if	(scode.compareTo("STATUS_GATEWAY_TIMEOUT") == 0)	return 504;
		return 405;
	}
	public void cb_diaRetrieveRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader, int tid, String peer)
	{
		try {
		SongURI song_reqEntity = song.songBindingFactory.createURI(reqEntity);
		SongURI song_targetID = song.songBindingFactory.createURI(targetID);
		SongServletRequest request = song.songBindingFactory.createRequest("coap/1.0", "RETRIEVE",
			song_reqEntity, song_targetID, null, 0, null, 0, false);
		// request.addHeader(name, value);
		request.setAttribute("diatid", new Integer(tid));
		request.setAttribute("diapeer", peer);
		request.setAttribute("quoi", "retrieve");
		request.send();
		} catch (Exception e) {
		}
	}

	public void cb_diaRetrieveResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid)
	{
		try {
 		SongServletRequest request = (SongServletRequest)transactionIDs.get(new Integer(tid));
		if	(request != null) {
 			transactionIDs.remove(new Integer(tid));
			SongServletResponse response = song.songBindingFactory.createResponse(request, dia2song_code(scode),
				null, null, 0, null, 0);
			response.setContent(content, contentType);
			response.send();
		}
		} catch (Exception e) {}
	}

	public void cb_diaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType,
		DiaOpt[] optAttr, DiaOpt[] optHeader, int tid, String peer)
	{
		try {
		SongURI song_reqEntity = song.songBindingFactory.createURI(reqEntity);
		SongURI song_targetID = song.songBindingFactory.createURI(targetID);
		SongServletRequest request = song.songBindingFactory.createRequest("coap/1.0", "CREATE",
			song_reqEntity, song_targetID, null, 0, null, 0, false);
		// request.addHeader(name, value);
		request.setContent(content, contentType);
		request.setAttribute("diatid", new Integer(tid));
		request.setAttribute("diapeer", peer);
		request.setAttribute("quoi", "create");
		request.send();
		} catch (Exception e) {}
/*
		byte[] payload = "<obj />".getBytes();
		diaCreateResponse("STATUS_OK", "uri", payload, "application/xml", null, tid);
*/
	}

	public void cb_diaCreateResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid)
	{
		try {
 		SongServletRequest request = (SongServletRequest)transactionIDs.get(new Integer(tid));
		if	(request != null) {
 			transactionIDs.remove(new Integer(tid));
			SongServletResponse response = song.songBindingFactory.createResponse(request, dia2song_code(scode),
				null, null, 0, null, 0);
			response.setContent(content, contentType);
			response.send();
		}
		} catch (Exception e) {}
	}

	public void cb_diaUpdateRequest(String reqEntity, String targetID, byte[] content, String contentType,
		DiaOpt[] optAttr, DiaOpt[] optHeader, int tid, String peer)
	{
		try {
		SongURI song_reqEntity = song.songBindingFactory.createURI(reqEntity);
		SongURI song_targetID = song.songBindingFactory.createURI(targetID);
		SongServletRequest request = song.songBindingFactory.createRequest("coap/1.0", "UPDATE",
			song_reqEntity, song_targetID, null, 0, null, 0, false);
		// request.addHeader(name, value);
		request.setContent(content, contentType);
		request.setAttribute("diatid", new Integer(tid));
		request.setAttribute("diapeer", peer);
		request.setAttribute("quoi", "update");
		request.send();
		} catch (Exception e) {}
	}

	public void cb_diaUpdateResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid)
	{
		try {
 		SongServletRequest request = (SongServletRequest)transactionIDs.get(new Integer(tid));
		if	(request != null) {
 			transactionIDs.remove(new Integer(tid));
			int code	= dia2song_code(scode);
			if	(code== 205)	code = 204;
			SongServletResponse response = song.songBindingFactory.createResponse(request, code,
				null, null, 0, null, 0);
			response.setContent(content, contentType);
			response.send();
		}
		} catch (Exception e) {}
	}

	public void cb_diaDeleteRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader, int tid, String peer)
	{
		try {
		SongURI song_reqEntity = song.songBindingFactory.createURI(reqEntity);
		SongURI song_targetID = song.songBindingFactory.createURI(targetID);
		SongServletRequest request = song.songBindingFactory.createRequest("coap/1.0", "DELETE",
			song_reqEntity, song_targetID, null, 0, null, 0, false);
		// request.addHeader(name, value);
		request.setAttribute("diatid", new Integer(tid));
		request.setAttribute("diapeer", peer);
		request.setAttribute("quoi", "delete");
		request.send();
		} catch (Exception e) {}
	}

	public void cb_diaDeleteResponse(String scode, DiaOpt[] optHeader, int tid)
	{
		try {
 		SongServletRequest request = (SongServletRequest)transactionIDs.get(new Integer(tid));
		if	(request != null) {
 			transactionIDs.remove(new Integer(tid));
			SongServletResponse response = song.songBindingFactory.createResponse(request, dia2song_code(scode),
				null, null, 0, null, 0);
			response.send();
		}
		} catch (Exception e) {}
	}

	public void cb_diaErrorResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid)
	{
		try {
 		SongServletRequest request = (SongServletRequest)transactionIDs.get(new Integer(tid));
		if	(request != null) {
 			transactionIDs.remove(new Integer(tid));
			SongServletResponse response = song.songBindingFactory.createResponse(request, dia2song_code(scode),
				null, null, 0, null, 0);
			response.setContent(content, contentType);
			response.send();
		}
		} catch (Exception e) {}
	}

	Level _levels[] = { Level.FATAL, Level.ERROR, Level.WARN, Level.INFO, Level.DEBUG, Level.TRACE };

	public void _traceFunction(int level, String message) {
		Level l;
		if	(level > 5)
			level	= 5;
		l	= _levels[level];
		logger.log(l, message);
	}
}
