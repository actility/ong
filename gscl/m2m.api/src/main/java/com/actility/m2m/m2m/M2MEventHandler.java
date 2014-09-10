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
 * id $Id: M2MEventHandler.java 6080 2013-10-15 13:25:46Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6080 $
 * lastrevision $Date: 2013-10-15 15:25:46 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:25:46 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.m2m;

import java.io.IOException;
import java.io.Serializable;

public interface M2MEventHandler {
    void init(M2MContext m2mContext);

    void doCreateIndication(Indication indication) throws IOException, M2MException;

    void doRetrieveIndication(Indication indication) throws IOException, M2MException;

    void doUpdateIndication(Indication indication) throws IOException, M2MException;

    void doDeleteIndication(Indication indication) throws IOException, M2MException;

    void doSuccessConfirm(Confirm confirm) throws IOException, M2MException;

    void doUnsuccessConfirm(Confirm confirm) throws IOException, M2MException;

    void timeout(String timerId, M2MSession session, Serializable info) throws IOException, M2MException;

    void sessionExpired(M2MSession session) throws IOException, M2MException;
}
