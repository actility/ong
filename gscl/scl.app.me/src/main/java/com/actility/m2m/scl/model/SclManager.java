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
 * id $Id: SclManager.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.model;

import java.io.Serializable;
import java.net.URI;

import com.actility.m2m.m2m.M2MContext;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public interface SclManager {
    URI getNsclUri();

    URI getLocalSclUri(URI reference) throws M2MException;

    URI createPocUriFromServerRoot(URI reference, String path) throws M2MException;

    M2MContext getM2MContext();

    StorageRequestExecutor getStorageContext();

    XoService getXoService();

    SclConfig getSclConfig();

    StatsManager getStatsManager();

    ResourceController getControllerFromTag(String tag);

    Object[] getControllerAndResource(String path) throws StorageException, XoException, M2MException;

    XoObject getXoResource(String path) throws StorageException, XoException;

    int getDefaultResourceId(String path);

    String startResourceTimer(long timeout, String path, int ctrlId, Serializable info);
}
