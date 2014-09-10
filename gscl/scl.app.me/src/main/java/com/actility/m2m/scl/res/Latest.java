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
 * id $Id: Latest.java 4186 2013-01-23 10:35:19Z JReich $
 * author $Author: JReich $
 * version $Revision: 4186 $
 * lastrevision $Date: 2013-01-23 11:35:19 +0100 (Wed, 23 Jan 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-01-23 11:35:19 +0100 (Wed, 23 Jan 2013) $
 */

package com.actility.m2m.scl.res;

import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class Latest implements SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(Latest.class, BundleLogger.getStaticLogger());

    private static final Latest INSTANCE = new Latest();

    public static Latest getInstance() {
        return INSTANCE;
    }

    public boolean dependsOnRequestingEntity() {
        return false;
    }

    public boolean dependsOnFilterCriteria() {
        return true;
    }

    public FilterCriteria buildFilterCriteria() {
        return ContentInstances.getInstance().buildFilterCriteria();
    }

    public boolean filterMatches(XoObject resource, FilterCriteria filterCriteria) {
        return ContentInstance.getInstance().filterMatches(resource, filterCriteria);
    }

    public FilterCriteria mergeFilterCriteria(FilterCriteria mergedFilterCriteria, FilterCriteria filterCriteria) {
        return ContentInstances.getInstance().mergeFilterCriteria(mergedFilterCriteria, filterCriteria);
    }

    public void prepareResourceForResponse(SclManager manager, String path, XoObject resource, FilterCriteria filterCriteria,
            Set supported) throws XoException {
        ContentInstance.getInstance().prepareResourceForResponse(manager, path, resource, filterCriteria, supported);
    }

    public void prepareResponse(Map context, SclManager manager, String path, XoObject resource, FilterCriteria filterCriteria) {
        // TODO
    }

    public void restoreResponse(Map context, XoObject resource) {
        // TODO
    }

    public byte[] getResponseRepresentation(SclManager manager, String path, FilterCriteria filterCriteria, Set supported,
            String mediaType) throws StorageException, XoException {
        ConditionBuilder conditionBuilder = manager.getConditionBuilder();
        Condition condition = conditionBuilder.createStringCondition(Constants.ATTR_TYPE, ConditionBuilder.OPERATOR_EQUAL,
                Constants.TYPE_CONTENT_INSTANCE);
        SearchResult searchResult = manager.getStorageContext().search(path, StorageRequestExecutor.SCOPE_EXACT, condition,
                StorageRequestExecutor.ORDER_DESC, 1);
        Map children = searchResult.getResults();
        if (children.size() == 1) {
            Entry entry = (Entry) children.entrySet().iterator().next();
            String subPath = (String) entry.getKey();
            byte[] contentInstance = (byte[]) entry.getValue();
            XoObject latest = null;
            try {
                latest = manager.getXoService().readBinaryXmlXoObject(contentInstance);
                if (ContentInstance.getInstance().filterMatches(latest, filterCriteria)) {
                    ContentInstance.getInstance().prepareResourceForResponse(manager, subPath, latest, filterCriteria,
                            supported);
                    if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                        return latest.saveExi();
                    }
                    return latest.saveXml();
                }
                return null;
            } finally {
                if (latest != null) {
                    latest.free(true);
                }
            }
        }
        LOG.error("No latest value !");
        return null;
    }
}
