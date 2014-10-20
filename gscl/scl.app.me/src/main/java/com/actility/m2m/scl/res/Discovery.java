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
 * id $Id: Discovery.java 3644 2012-11-27 16:26:04Z JReich $
 * author $Author: JReich $
 * version $Revision: 3644 $
 * lastrevision $Date: 2012-11-27 17:26:04 +0100 (Tue, 27 Nov 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-11-27 17:26:04 +0100 (Tue, 27 Nov 2012) $
 */

package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.Serializable;
import java.net.URI;
import java.text.ParseException;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.m2m.Response;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.model.PartialAccessor;
import com.actility.m2m.scl.model.ResourceController;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * Discovery resource.
 *
 * <pre>
 * m2m:Discovery from ong:t_xml_obj
 * {
 *     m2m:matchSize    XoString    { } // (optional) (xmlType: xsd:long)
 *     m2m:truncated    XoString    { } // (optional) (xmlType: xsd:boolean)
 *     m2m:discoveryURI    m2m:AnyURIList    { } // (optional)
 * }
 * alias m2m:Discovery with m2m:discovery
 * </pre>
 */
public final class Discovery implements ResourceController {
    private static final Discovery INSTANCE = new Discovery();

    public static Discovery getInstance() {
        return INSTANCE;
    }

    private Discovery() {
        // Cannot be instantiated
    }

    private void sendMethodNotAllowed(SclManager manager, String path, Indication indication, boolean createAllowed,
            boolean retrieveAllowed, boolean updateAllowed, boolean deleteAllowed) throws XoException, IOException {
        Response response = indication.createUnsuccessResponse(StatusCode.STATUS_METHOD_NOT_ALLOWED);
        XoObject errorInfo = null;
        try {
            response.setAllow(createAllowed, retrieveAllowed, updateAllowed, deleteAllowed);
            errorInfo = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
            errorInfo.setNameSpace(M2MConstants.PREFIX_M2M);
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, StatusCode.STATUS_METHOD_NOT_ALLOWED.name());
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, "Method is not allowed on targetted resource: "
                    + path);
            try {
                response.setRepresentation(errorInfo);
            } catch (M2MException e) {
                response.setRepresentationWithDefaultMediaType(errorInfo);
            }

            response.send();
        } finally {
            if (errorInfo != null) {
                errorInfo.free(true);
            }
        }
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        sendMethodNotAllowed(manager, path, indication, false, true, false, false);
    }

    private Condition appendCondition(ConditionBuilder conditionBuilder, Condition current, Condition newCondition) {
        Condition result = null;
        if (current == null) {
            result = newCondition;
        } else {
            result = conditionBuilder.createConjunction(current, newCondition);
        }
        return result;
    }

    public void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        SclLogger.logRequestIndication("discoveryRetrieveRequestIndication", "discoveryRetrieveResponseConfirm", indication,
                null, 0);

        String accessRightId = manager.getSclConfig().getDiscoveryAccessRightID();
        if (accessRightId != null) {
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": Check " + M2MConstants.FLAG_READ + " right on " + path);
            }
            Application.getInstance().retrieveAndCheckPermissionFromAccessRightID(indication.getTransactionId(), manager, path,
                    indication.getRequestingEntity(), M2MConstants.FLAG_READ, accessRightId);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": " + M2MConstants.FLAG_READ + " right granted on " + path);
            }
        }

        ConditionBuilder conditionBuilder = manager.getConditionBuilder();
        String appPath = manager.getM2MContext().getApplicationPath();
        int limit = -1;
        if (indication.getMaxSize() != null) {
            limit = (int) indication.getMaxSize().longValue();
        }
        String searchPrefix = indication.getSearchPrefix();
        String scopeParameter = indication.getQueryParameter("scope");
        FilterCriteria filterCriteria = indication.getFilterCriteria();
        SearchResult searchResult = null;
        Map result = null;

        int scope = StorageRequestExecutor.SCOPE_SUBTREE;
        if (searchPrefix == null) {
            searchPrefix = M2MConstants.URI_SEP;
        } else {
            if (!searchPrefix.endsWith(M2MConstants.URI_SEP)) {
                searchPrefix += M2MConstants.URI_SEP;
            }
            if (searchPrefix.startsWith(appPath)) {
                searchPrefix = searchPrefix.substring(appPath.length());
            } else {
                result = EmptyUtils.EMPTY_MAP;
            }
        }
        if (scopeParameter != null && "exact".equalsIgnoreCase(scopeParameter)) {
            scope = StorageRequestExecutor.SCOPE_EXACT;
        }

        if (result == null) {
            // Build storage conditions
            Condition condition = null;
            if (filterCriteria.getCreatedAfter() != null) {
                condition = appendCondition(conditionBuilder, condition, conditionBuilder.createDateCondition(
                        M2MConstants.ATTR_CREATION_TIME, ConditionBuilder.OPERATOR_GREATER, filterCriteria.getCreatedAfter()));
            }
            if (filterCriteria.getCreatedBefore() != null) {
                condition = appendCondition(conditionBuilder, condition, conditionBuilder.createDateCondition(
                        M2MConstants.ATTR_CREATION_TIME, ConditionBuilder.OPERATOR_LOWER, filterCriteria.getCreatedBefore()));
            }
            if (filterCriteria.getIfModifiedSince() != null) {
                condition = appendCondition(conditionBuilder, condition, conditionBuilder.createDateCondition(
                        M2MConstants.ATTR_LAST_MODIFIED_TIME, ConditionBuilder.OPERATOR_GREATER,
                        filterCriteria.getIfModifiedSince()));
            }
            if (filterCriteria.getIfUnmodifiedSince() != null) {
                condition = appendCondition(conditionBuilder, condition, conditionBuilder.createDateCondition(
                        M2MConstants.ATTR_LAST_MODIFIED_TIME, ConditionBuilder.OPERATOR_LOWER,
                        filterCriteria.getIfUnmodifiedSince()));
            }
            if (filterCriteria.getIfNoneMatch() != null) {
                String[] ifNoneMatch = filterCriteria.getIfNoneMatch();
                Date ifNotLastModifiedTime = null;
                for (int i = 0; i < ifNoneMatch.length; ++i) {
                    try {
                        ifNotLastModifiedTime = FormatUtils.parseDateTime(ifNoneMatch[i]);
                        condition = appendCondition(conditionBuilder, condition, conditionBuilder.createDateCondition(
                                M2MConstants.ATTR_LAST_MODIFIED_TIME, ConditionBuilder.OPERATOR_NOT_EQUAL_TO,
                                ifNotLastModifiedTime));
                    } catch (ParseException e) {
                        // Ignore
                    }
                }
            }
            if (filterCriteria.getSearchString() != null) {
                String[] searchString = filterCriteria.getSearchString();
                for (int i = 0; i < searchString.length; ++i) {
                    condition = appendCondition(conditionBuilder, condition, conditionBuilder.createStringCondition(
                            M2MConstants.ATTR_SEARCH_STRING, ConditionBuilder.OPERATOR_EQUAL, searchString[i]));
                }
            }

            searchResult = manager.getStorageContext().search(searchPrefix, scope, condition, StorageRequestExecutor.ORDER_ASC,
                    -1);
            result = searchResult.getResults();
        }

        XoObject representation = null;
        try {
            representation = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_DISCOVERY);
            representation.setNameSpace(M2MConstants.PREFIX_M2M);
            XoObject discoveryURI = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_DISCOVERY_U_R_I);
            representation.setXoObjectAttribute(M2MConstants.TAG_M2M_DISCOVERY_U_R_I, discoveryURI);
            List discoveryUriList = discoveryURI.getStringListAttribute(M2MConstants.TAG_REFERENCE);
            Iterator it = result.entrySet().iterator();
            Entry entry = null;
            String refPath = null;
            byte[] rawXoObject = null;
            XoObject xoObject = null;
            ResourceController controller = null;
            int urisCount = limit;
            if (limit == -1) {
                urisCount = Integer.MAX_VALUE;
            }
            while (it.hasNext()) {
                entry = (Entry) it.next();
                refPath = (String) entry.getKey();
                rawXoObject = (byte[]) entry.getValue();
                xoObject = manager.getXoService().readBinaryXmlXoObject(rawXoObject);
                try {
                    controller = manager.getControllerFromTag(xoObject.getName());

                    urisCount = controller.appendDiscoveryURIs(indication.getTransactionId(), manager, refPath, xoObject,
                            indication.getRequestingEntity(), indication.getTargetID(), appPath,
                            filterCriteria.getSearchString(), discoveryUriList, urisCount);
                } finally {
                    xoObject.free(true);
                }
            }
            if (limit == -1) {
                urisCount = Integer.MAX_VALUE - urisCount;
            } else {
                urisCount = limit - urisCount;
            }
            representation.setStringAttribute(M2MConstants.TAG_M2M_MATCH_SIZE, Integer.toString(urisCount));
            if (limit != -1 && urisCount > limit) {
                representation.setStringAttribute(M2MConstants.TAG_M2M_TRUNCATED, "true");
            } else {
                representation.setStringAttribute(M2MConstants.TAG_M2M_TRUNCATED, "false");
            }

            Response response = indication.createSuccessResponse(StatusCode.STATUS_OK);
            response.setRepresentation(representation);
            SclLogger.logResponseConfirm(response, StatusCode.STATUS_OK, representation, null);
            response.send();
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    public void doUpdateIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        sendMethodNotAllowed(manager, path, indication, false, true, false, false);
    }

    public void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        sendMethodNotAllowed(manager, path, indication, false, true, false, false);
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        throw new UnsupportedOperationException();
    }

    public void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        throw new UnsupportedOperationException();
    }

    public void doUpdateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        throw new UnsupportedOperationException();
    }

    public void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        throw new UnsupportedOperationException();
    }

    public void timeout(SclManager manager, String path, String timerId, M2MSession session, Serializable info)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        throw new UnsupportedOperationException();
    }

    public void sessionExpired(SclManager manager, String path, M2MSession session) throws ParseException, IOException,
            StorageException, XoException, M2MException {
        throw new UnsupportedOperationException();
    }

    public int appendDiscoveryURIs(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            URI targetID, String appPath, String[] searchStrings, List discoveryURIs, int remainingURIs) throws IOException,
            StorageException, XoException {
        throw new UnsupportedOperationException();
    }

}
