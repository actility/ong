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
 * id $Id: SclTransaction.java 3496 2012-11-13 10:09:48Z JReich $
 * author $Author: JReich $
 * version $Revision: 3496 $
 * lastrevision $Date: 2012-11-13 11:09:48 +0100 (Tue, 13 Nov 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-11-13 11:09:48 +0100 (Tue, 13 Nov 2012) $
 */

package com.actility.m2m.scl.model;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.storage.Batch;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.xo.XoException;

public final class SclTransaction {
    private final StorageRequestExecutor storageContext;
    private final Batch batch;
    private List transientOps;

    public SclTransaction(StorageRequestExecutor storageContext) {
        this.storageContext = storageContext;
        this.batch = storageContext.getStorageFactory().createBatch();
    }

    public void createResource(Document document) throws XoException {
        batch.create(null, document);
    }

    public void updateResource(Document document) throws XoException {
        batch.update(null, document, null);
    }

    public void deleteResource(String path) {
        Document document = storageContext.getStorageFactory().createDocument(path);
        batch.delete(null, document, null);
    }

    public void createResource(Map config, Document document) throws XoException {
        batch.create(config, document);
    }

    public void updateResource(Map config, Document document) throws XoException {
        batch.update(config, document, null);
    }

    public void deleteResource(Map config, String path) {
        Document document = storageContext.getStorageFactory().createDocument(path);
        batch.delete(config, document, null);
    }

    public void addTransientOp(TransientOp transientOp) {
        if (transientOps == null) {
            transientOps = new ArrayList();
        }
        transientOps.add(transientOp);
    }

    public void execute() throws ParseException, StorageException, XoException, M2MException {
        boolean executed = false;
        try {
            if (transientOps != null) {
                Iterator it = transientOps.iterator();
                TransientOp transientOp = null;
                while (it.hasNext()) {
                    transientOp = (TransientOp) it.next();
                    transientOp.prepare();
                }
            }

            int result = batch.execute();
            switch (result) {
            case Batch.BATCH_OK:
                executed = true;
                break;
            case Batch.BATCH_CREATE_FAILED:
            case Batch.BATCH_UPDATE_FAILED:
            case Batch.BATCH_DELETE_FAILED:
                throw new StorageException("Cannot execute batch operation");
            }

            if (transientOps != null) {
                Iterator it = transientOps.iterator();
                TransientOp transientOp = null;
                while (it.hasNext()) {
                    transientOp = (TransientOp) it.next();
                    transientOp.postCommit();
                }
            }
        } finally {
            if (!executed) {
                rollback();
            }
        }
    }

    private void rollback() {
        if (transientOps != null) {
            Iterator it = transientOps.iterator();
            TransientOp transientOp = null;
            while (it.hasNext()) {
                transientOp = (TransientOp) it.next();
                transientOp.rollback();
            }
        }
    }
}
