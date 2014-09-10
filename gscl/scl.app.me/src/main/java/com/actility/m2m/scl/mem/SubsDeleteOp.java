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
 * id $Id: SubsDeleteOp.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.TransientOp;

public final class SubsDeleteOp implements TransientOp {

    private final SclManager manager;
    private final String subsPath;

    public SubsDeleteOp(SclManager manager, String subsPath) {
        this.manager = manager;
        this.subsPath = subsPath;
    }

    public void prepare() {
        // Ignore
    }

    public void rollback() {
        // Ignore
    }

    public void postCommit() {
        // Subscription manager
        SubsManager subsManager = (SubsManager) manager.getM2MContext().getAttribute(subsPath);
        if (subsManager != null) {
            subsManager.resourceDeleted();
            manager.getM2MContext().removeAttribute(subsPath);
        }
    }

}
