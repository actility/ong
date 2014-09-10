/*
* Copyright (C) Actility, SA. All Rights Reserved.
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
*/
/* diajni.java -- 
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the libdiajni.so library. Please see
 * README for terms of use.
 */

class diaOpt {
	String	name;
	String	val;

	void diaOpt (String n, String v) {
		name	= n;
		val	= v;
	}
}

class diajni {
	int	diaptr;

	private native void diaInit();
	private native void diaPoll();
	private native void diaRun();
	private native void diaStop();
	private native void diaFree();

	private native void diaCreateResponse(String status, String resourceURI, byte[] content,
		String contentType, diaOpt[] optHeader, int tid);
	private native void diaRetrieveResponse(String status, byte[] content, String contentType,
		diaOpt[] optHeader, int tid);
	private native void diaUpdateResponse(String status, String content, String contentType, diaOpt[] optHeader, int tid);
	private native void diaDeleteResponse(String status, diaOpt[] optHeader, int tid);

	private native int diaCreateRequest(String reqEntity, String targetID, String content, String contentType, diaOpt[] optAttr, diaOpt[] optHeader);
	private native int diaRetrieveRequest(String reqEntity, String targetID, diaOpt[] optAttr, diaOpt[] optHeader);
	private native int diaUpdateRequest(String reqEntity, String targetID, String content, String contentType, diaOpt[] optAttr, diaOpt[] optHeader);
	private native int diaDeleteRequest(String reqEntity, String targetID, diaOpt[] optAttr, diaOpt[] optHeader);

        public static void main(String[] args) {
                diajni dia = new diajni();
		dia.diaInit ();
		dia.diaRun();
/*
		while (true) {
			dia.diaPoll();
		}
*/
	}

	public void cb_diaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType,
		diaOpt[] optAttr, diaOpt[] optHeader, int tid)
	{
		System.err.println ("Java : cb_diaCreateRequest(reqEntity="+reqEntity+" targetID="+targetID+" tid="+tid);
		byte[] payload = "<obj />".getBytes();
		diaCreateResponse("STATUS_OK", "uri", payload, "application/xml", null, tid);
	}

	public void cb_diaRetrieveRequest(String reqEntity, String targetID, diaOpt[] optAttr, diaOpt[] optHeader, int tid) {
		System.err.println ("Java : cb_diaRetrieveRequest(reqEntity="+reqEntity+" targetID="+targetID+" tid="+tid);
		byte[] payload = "<obj />".getBytes();
		diaRetrieveResponse("STATUS_OK", payload, "application/xml", null, tid);
	}

	static {
		try {
//			System.load("/Users/denis/Actility/libdIa/jni/libdiajni.so");
//			System.load("/home/actility/libdIa/jni/libdiajni.so");
//			System.load("./libdiajni.so");
			System.loadLibrary("diajni");
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Native code library failed to load.\n" + e);
		}
	}
}
