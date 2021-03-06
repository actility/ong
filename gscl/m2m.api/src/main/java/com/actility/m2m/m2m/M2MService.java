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
 * id $Id: M2MService.java 7982 2014-03-07 14:49:04Z JReich $
 * author $Author: JReich $
 * version $Revision: 7982 $
 * lastrevision $Date: 2014-03-07 15:49:04 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 15:49:04 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.m2m;

import java.net.URI;
import java.util.Map;

public interface M2MService {
    String PRIVATE_PATH = "com.actility.m2m.PrivatePath";

    M2MContext registerScl(String applicationName, String applicationPath, M2MEventHandler m2mHandler,
            M2MProxyHandler proxyHandler, Map configuration) throws M2MException;

    M2MContext registerApplication(String applicationName, String applicationPath, URI sclUri, M2MEventHandler m2mHandler,
            Map configuration) throws M2MException;

    void unregisterContext(M2MContext m2mContext);

    M2MUtils getM2MUtils();
}
