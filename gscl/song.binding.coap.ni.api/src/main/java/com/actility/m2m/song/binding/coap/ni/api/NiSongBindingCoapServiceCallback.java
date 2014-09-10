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

package com.actility.m2m.song.binding.coap.ni.api;


public interface NiSongBindingCoapServiceCallback {

    public void doDiaRetrieveRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader, int tid,
            String peer, boolean proxyRequest);

    public void doDiaRetrieveResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid);

    public void doDiaCreateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr,
            DiaOpt[] optHeader, int tid, String peer, boolean proxyRequest);

    public void doDiaCreateResponse(String scode, String resourceUri, byte[] content, String contentType, DiaOpt[] optHeader,
            int tid);

    public void doDiaUpdateRequest(String reqEntity, String targetID, byte[] content, String contentType, DiaOpt[] optAttr,
            DiaOpt[] optHeader, int tid, String peer, boolean proxyRequest);

    public void doDiaUpdateResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid);

    public void doDiaDeleteRequest(String reqEntity, String targetID, DiaOpt[] optAttr, DiaOpt[] optHeader, int tid,
            String peer, boolean proxyRequest);

    public void doDiaDeleteResponse(String scode, DiaOpt[] optHeader, int tid);

    public void doDiaErrorResponse(String scode, byte[] content, String contentType, DiaOpt[] optHeader, int tid);

}
