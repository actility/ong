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
 * id $Id: SclResource.java 4308 2013-02-19 11:04:06Z JReich $
 * author $Author: JReich $
 * version $Revision: 4308 $
 * lastrevision $Date: 2013-02-19 12:04:06 +0100 (Tue, 19 Feb 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-02-19 12:04:06 +0100 (Tue, 19 Feb 2013) $
 */

package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URLDecoder;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.ContentInstanceFilterCriteria;
import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.m2m.Response;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.SubsManager;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.PartialAccessor;
import com.actility.m2m.scl.model.ResourceController;
import com.actility.m2m.scl.model.SclConfig;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.StringReader;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public abstract class SclResource implements ResourceController {
    private static final Logger LOG = OSGiLogger.getLogger(SclResource.class, BundleLogger.getStaticLogger());

    private final boolean collection;
    private final String retrieveRequestIndication;
    private final String retrieveResponseConfirm;
    private final String updateRequestIndication;
    private final String updateResponseConfirm;
    private final String deleteRequestIndication;
    private final String deleteResponseConfirm;
    private final int filterCriteriaMode;
    private final boolean hasLastModifiedTag;
    private final boolean hasSubscriptionsOnResource;
    private final SclResource parentResourceController;
    private final boolean parentHasSubscriptionsOnResource;
    private final boolean buildSupported;

    protected SclResource(boolean collection, String retrieveRequestIndication, String retrieveResponseConfirm,
            String updateRequestIndication, String updateResponseConfirm, String deleteRequestIndication,
            String deleteResponseConfirm, int filterCriteriaMode, boolean hasLastModifiedTag,
            boolean hasSubscriptionsOnResource, SclResource parentResourceController, boolean parentHasSubscriptionsOnResource,
            boolean buildSupported) {
        this.collection = collection;
        this.retrieveRequestIndication = retrieveRequestIndication;
        this.retrieveResponseConfirm = retrieveResponseConfirm;
        this.updateRequestIndication = updateRequestIndication;
        this.updateResponseConfirm = updateResponseConfirm;
        this.deleteRequestIndication = deleteRequestIndication;
        this.deleteResponseConfirm = deleteResponseConfirm;
        this.filterCriteriaMode = filterCriteriaMode;
        this.hasLastModifiedTag = hasLastModifiedTag;
        this.hasSubscriptionsOnResource = hasSubscriptionsOnResource;
        this.parentResourceController = parentResourceController;
        this.parentHasSubscriptionsOnResource = parentHasSubscriptionsOnResource;
        this.buildSupported = buildSupported;
    }

    public abstract void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            String flag) throws UnsupportedEncodingException, StorageException, XoException, M2MException;

    public abstract boolean updateResource(String logId, SclManager manager, String path, XoObject resource,
            Indication indication, XoObject representation, Date now) throws ParseException, UnsupportedEncodingException,
            XoException, StorageException, M2MException;

    public abstract void deleteResource(String logId, SclManager manager, String path, XoObject resource,
            SclTransaction transaction) throws UnsupportedEncodingException, XoException, StorageException, M2MException;

    public abstract void deleteChildResource(String logId, SclManager manager, String path, XoObject resource,
            XoObject childResource, Date now, SclTransaction transaction) throws ParseException, UnsupportedEncodingException,
            XoException, StorageException, M2MException;

    public boolean dependsOnFilterCriteria() {
        return filterCriteriaMode > 0;
    }

    public FilterCriteria buildFilterCriteria() {
        return new FilterCriteria();
    }

    public boolean filterMatches(XoObject resource, FilterCriteria filterCriteria) {
        boolean matches = true;
        if (filterCriteria != null) {
            matches = filterMatchesOnLastModifiedTime(resource, filterCriteria)
                    && filterMatchesOnSearchStrings(resource, filterCriteria)
                    && filterMatchesOnCreationTime(resource, filterCriteria);
        }
        return matches;
    }

    public boolean filterMatchesOnLastModifiedTime(XoObject resource, FilterCriteria filterCriteria) {
        String attrValue = resource.getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME);
        if (attrValue != null) {
            try {
                Date dateValue = FormatUtils.parseDateTime(attrValue);
                if (filterCriteria.getIfModifiedSince() != null
                        && !(filterCriteria.getIfModifiedSince().getTime() < dateValue.getTime())) {
                    return false;
                }
                if (filterCriteria.getIfUnmodifiedSince() != null
                        && !(filterCriteria.getIfUnmodifiedSince().getTime() > dateValue.getTime())) {
                    return false;
                }
                if (filterCriteria.getIfNoneMatch() != null
                        && Arrays.binarySearch(filterCriteria.getIfNoneMatch(), attrValue) >= 0) {
                    return false;
                }
            } catch (ParseException e) {
                LOG.error("Cannot parse " + M2MConstants.TAG_M2M_LAST_MODIFIED_TIME + " '" + attrValue + "' of resource", e);
                return false;
            }
        }
        return true;
    }

    public boolean filterMatchesOnSearchStrings(XoObject resource, FilterCriteria filterCriteria) {
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        if (searchStrings != null) {
            List seachStringsList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            String[] searchString = filterCriteria.getSearchString();
            if (searchString != null) {
                int i = 0;
                for (i = 0; i < searchString.length; ++i) {
                    if (seachStringsList.contains(searchString[i])) {
                        break;
                    }
                }
                if (i == (searchString.length + 1)) {
                    return false;
                }
            }
        }
        return true;
    }

    public boolean filterMatchesOnCreationTime(XoObject resource, FilterCriteria filterCriteria) {
        String attrValue = resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME);
        if (attrValue != null) {
            try {
                Date dateValue = FormatUtils.parseDateTime(attrValue);
                if (filterCriteria.getCreatedAfter() != null
                        && !(filterCriteria.getCreatedAfter().getTime() < dateValue.getTime())) {
                    return false;
                }
                if (filterCriteria.getCreatedBefore() != null
                        && !(filterCriteria.getCreatedBefore().getTime() > dateValue.getTime())) {
                    return false;
                }
            } catch (ParseException e) {
                LOG.error("Cannot parse " + M2MConstants.TAG_M2M_CREATION_TIME + " '" + attrValue + "' of resource", e);
                return false;
            }
        }
        return true;
    }

    public FilterCriteria mergeFilterCriteria(FilterCriteria mergedFilterCriteria, FilterCriteria filterCriteria) {
        FilterCriteria result = null;
        if (mergedFilterCriteria != null && filterCriteria != null) {
            result = mergedFilterCriteria;
            if (result.getIfModifiedSince() == null || filterCriteria.getIfModifiedSince() == null) {
                result.setIfModifiedSince(null);
            } else if (result.getIfModifiedSince().getTime() > filterCriteria.getIfModifiedSince().getTime()) {
                result.setIfModifiedSince(filterCriteria.getIfModifiedSince());
            }
            if (result.getIfUnmodifiedSince() == null || filterCriteria.getIfUnmodifiedSince() == null) {
                result.setIfUnmodifiedSince(null);
            } else if (result.getIfUnmodifiedSince().getTime() < filterCriteria.getIfUnmodifiedSince().getTime()) {
                result.setIfUnmodifiedSince(filterCriteria.getIfUnmodifiedSince());
            }
            if (result.getIfNoneMatch() == null || filterCriteria.getIfNoneMatch() == null) {
                result.setIfNoneMatch(null);
            } else {
                List interList = new ArrayList();
                String[] resultIfNoneMatch = result.getIfNoneMatch();
                String[] fcIfNoneMatch = filterCriteria.getIfNoneMatch();
                for (int i = 0; i < fcIfNoneMatch.length; ++i) {
                    if (Arrays.binarySearch(resultIfNoneMatch, fcIfNoneMatch[i]) >= 0) {
                        interList.add(fcIfNoneMatch[i]);
                    }
                }
                fcIfNoneMatch = new String[interList.size()];
                interList.toArray(fcIfNoneMatch);
                result.setIfNoneMatch(fcIfNoneMatch);
            }
            if (result.getSearchString() == null || filterCriteria.getSearchString() == null) {
                result.setSearchString(null);
            } else {
                List mergedList = new ArrayList();
                mergedList.addAll(Arrays.asList(result.getSearchString()));
                String[] resultSearchString = result.getSearchString();
                String[] fcSearchString = filterCriteria.getSearchString();
                for (int i = 0; i < fcSearchString.length; ++i) {
                    if (Arrays.binarySearch(resultSearchString, fcSearchString[i]) < 0) {
                        mergedList.add(fcSearchString[i]);
                    }
                }
                resultSearchString = new String[mergedList.size()];
                mergedList.toArray(resultSearchString);
                Arrays.sort(resultSearchString);
                result.setSearchString(resultSearchString);
            }
            if (result.getCreatedAfter() == null || filterCriteria.getCreatedAfter() == null) {
                result.setCreatedAfter(null);
            } else if (result.getCreatedAfter().getTime() > filterCriteria.getCreatedAfter().getTime()) {
                result.setCreatedAfter(filterCriteria.getCreatedAfter());
            }
            if (result.getCreatedBefore() == null || filterCriteria.getCreatedBefore() == null) {
                result.setCreatedBefore(null);
            } else if (result.getCreatedBefore().getTime() < filterCriteria.getCreatedBefore().getTime()) {
                result.setCreatedBefore(filterCriteria.getCreatedBefore());
            }
        }
        return result;
    }

    public void prepareResourceForResponse(String logId, SclManager manager, String path, XoObject resource,
            URI requestingEntity, FilterCriteria filterCriteria, Set supported) throws UnsupportedEncodingException,
            StorageException, XoException, M2MException {
        // Must be overidden if needed
    }

    public void prepareResponse(Map context, SclManager manager, String path, XoObject resource, FilterCriteria filterCriteria) {
        // Nothing to do
    }

    public void restoreResponse(Map context, XoObject resource) {
        // Nothing to do
    }

    public byte[] getResponseRepresentation(String logId, SclManager manager, String path, URI requestingEntity,
            FilterCriteria filterCriteria, Set supported, String mediaType) throws UnsupportedEncodingException,
            StorageException, XoException, M2MException {
        XoObject localResource = null;
        try {
            localResource = manager.getXoResource(path);
            if (filterMatches(localResource, filterCriteria)) {
                prepareResourceForResponse(logId, manager, path, localResource, requestingEntity, filterCriteria, supported);
                if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                    return localResource.saveExi();
                }
                return localResource.saveXml();
            }
            return null;
        } finally {
            if (localResource != null) {
                localResource.free(true);
            }
        }
    }

    public void sendSuccessResponse(SclManager manager, String path, XoObject resource, Indication indication,
            StatusCode statusCode, boolean withRepresentation, String eTag, long lastModified) throws M2MException,
            XoException, StorageException, IOException {
        if (withRepresentation) {
            Set supported = null;
            if (buildSupported) {
                supported = new HashSet();
                ListIterator it = indication.getHeaders(Constants.HD_X_ACY_SUPPORTED);
                while (it.hasNext()) {
                    supported.add(it.next());
                }
            }
            prepareResourceForResponse(indication.getTransactionId(), manager, path, resource,
                    indication.getRequestingEntity(), getFilterCriteria(indication), supported);
        }
        URI resourceURI = manager.getM2MContext().createLocalUri(indication.getTargetID(), path);
        Response response = indication.createSuccessResponse(statusCode);
        response.setResouceURI(resourceURI);
        if (eTag != null) {
            response.setETag(eTag);
        }
        if (lastModified > 0) {
            response.setLastModified(lastModified);
        }
        if (withRepresentation) {
            response.setRepresentation(resource);
            SclLogger.logResponseConfirm(response, statusCode, resource, null);
        } else {
            SclLogger.logResponseConfirm(response, statusCode, (XoObject) null, null);
        }
        response.send();
    }

    public void sendCreatedSuccessResponse(SclManager manager, String path, XoObject resource, Indication indication,
            boolean withRepresentation) throws M2MException, XoException, StorageException, IOException {
        String eTag = null;
        long lastModified = -1;
        if (hasLastModifiedTag) {
            eTag = resource.getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME);

            try {
                lastModified = FormatUtils.parseDateTime(eTag).getTime();
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Cannot parse " + M2MConstants.TAG_M2M_LAST_MODIFIED_TIME + " '"
                        + eTag + "' of resource on path '" + path + "'", e);
            }
        }
        sendSuccessResponse(manager, path, resource, indication, StatusCode.STATUS_CREATED, withRepresentation, eTag,
                lastModified);
    }

    public void sendDeleteSuccessResponse(Indication indication) throws IOException, XoException {
        Response response = indication.createSuccessResponse(StatusCode.STATUS_OK);
        SclLogger.logResponseConfirm(response, StatusCode.STATUS_OK, (XoObject) null, null);
        response.send();
    }

    public void sendMethodNotAllowed(SclManager manager, String path, Indication indication, boolean createAllowed,
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

    public void sendUnsuccessResponse(SclManager manager, Indication indication, StatusCode statusCode, String message)
            throws XoException, IOException {
        Response response = indication.createUnsuccessResponse(statusCode);
        XoObject errorInfo = null;
        try {
            errorInfo = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
            errorInfo.setNameSpace(M2MConstants.PREFIX_M2M);
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, statusCode.name());
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, message);
            try {
                response.setRepresentation(errorInfo);
            } catch (M2MException e) {
                response.setRepresentationWithDefaultMediaType(errorInfo);
            }

            SclLogger.logResponseConfirm(response, statusCode, errorInfo, null);
            response.send();
        } finally {
            if (errorInfo != null) {
                errorInfo.free(true);
            }
        }
    }

    public static String getParentPath(String path) {
        return path.substring(0, path.lastIndexOf('/', path.length() - 1));
    }

    public static String getGrandParentPath(String path) {
        int slashIndex = path.lastIndexOf('/', path.length() - 1);
        if (slashIndex >= 0) {
            slashIndex = path.lastIndexOf('/', slashIndex - 1);
            if (slashIndex >= 0) {
                return path.substring(0, slashIndex);
            }
        }
        return null;
    }

    public String normalizeLocalPath(SclManager manager, String tagName, String contextPath, String path, int slashManagement)
            throws M2MException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("normalizeLocalPath: (tagName:" + tagName + ") (contextPath: " + contextPath + ") (path:" + path
                    + ") (slashManagement: " + slashManagement + ")");
        }
        String normalizedPath = normalizePath(tagName, contextPath, path, slashManagement);
        if (path.charAt(0) == '/') {
            String appPath = manager.getM2MContext().getApplicationPath();
            if (!normalizedPath.startsWith(appPath)) {
                throw new M2MException(tagName + " is not a valid local path: " + path, StatusCode.STATUS_BAD_REQUEST);
            }
            // In this case, remove the scl.app contextPath
            normalizedPath = normalizedPath.substring(appPath.length());
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("normalizeLocalPath result: " + normalizedPath);
        }
        return normalizedPath;
    }

    public String getAndCheckSubURI(SclManager manager, String path, XoObject representation, String tagName, int mode,
            String targetPath) throws M2MException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("getAndCheckSubURI: (path: " + path + ") (tagName:" + tagName + ") (mode: " + mode + ") (targetPath:"
                    + targetPath + ")");
        }
        String attribute = representation.getStringAttribute(tagName);
        URI uri = checkURI(attribute, tagName, mode);
        if (uri.isAbsolute()) {
            throw new M2MException(tagName + " is absolute but only relative URI are supported: " + attribute,
                    StatusCode.STATUS_BAD_REQUEST);
        }
        String realPath = normalizeLocalPath(manager, tagName, targetPath, attribute, Constants.ID_SLASH_MGMT_LEAVE);
        if (!realPath.startsWith(targetPath)) {
            throw new M2MException(tagName + " is not a sub URI of the targeted path (refPath: " + targetPath
                    + ") (computedPath: " + realPath + ") (receivedPath: " + attribute + ")", StatusCode.STATUS_BAD_REQUEST);
        }
        String normalizedPath = realPath.substring(targetPath.length());
        representation.setStringAttribute(tagName, normalizedPath);
        if (normalizedPath.endsWith(M2MConstants.URI_SEP)) {
            if (LOG.isDebugEnabled()) {
                LOG.debug("getAndCheckSubURI result: " + normalizedPath.substring(0, normalizedPath.length() - 1));
            }
            return normalizedPath.substring(0, normalizedPath.length() - 1);
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("getAndCheckSubURI result: " + normalizedPath);
        }
        return normalizedPath;
    }

    public String getAndCheckAccessRightID(SclManager manager, XoObject representation, String tagName, int mode,
            String targetPath) throws M2MException {
        String attribute = representation.getStringAttribute(tagName);
        String realPath = null;
        URI accessRightID = checkURI(attribute, tagName, mode);
        if (accessRightID != null) {
            if (accessRightID.isAbsolute()) {
                throw new M2MException(tagName + " is absolute but only relative URI are supported: " + attribute,
                        StatusCode.STATUS_BAD_REQUEST);
            }
            realPath = normalizeLocalPath(manager, tagName, targetPath, attribute, Constants.ID_SLASH_MGMT_REMOVE);
            if (manager.getDefaultResourceId(realPath) != Constants.ID_RES_ACCESS_RIGHT) {
                throw new M2MException(tagName + " is not pointing to a valid path for an accessRight resource: " + attribute
                        + " (" + realPath + ")", StatusCode.STATUS_BAD_REQUEST);
            }
            representation.setStringAttribute(tagName, realPath);
        }
        return realPath;
    }

    public String checkAPoCPath(SclManager manager, String path, XoObject apocPath, String tagName, String targetPath)
            throws M2MException {
        // m2m:APoCPath from ong:t_xml_obj
        // {
        // m2m:path XoString { } // (xmlType: xsd:anyURI)
        // m2m:accessRightID XoString { } // (optional) (xmlType: xsd:anyURI)
        // m2m:searchStrings m2m:SearchStrings { } // (optional)
        // }
        if (LOG.isDebugEnabled()) {
            LOG.debug("checkAPoCPath: (path: " + path + ") (tagName:" + tagName + ") (targetPath:" + targetPath + ")");
        }
        String subPath = getAndCheckSubURI(manager, path, apocPath, M2MConstants.TAG_M2M_PATH, Constants.ID_MODE_REQUIRED,
                targetPath);
        getAndCheckAccessRightID(manager, apocPath, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                targetPath);
        return subPath;
    }

    public Set getAndCheckAPoCPaths(SclManager manager, String path, XoObject representation, String tagName, int mode,
            String targetPath) throws M2MException {
        // m2m:APoCPaths from ong:t_xml_obj
        // {
        // m2m:aPoCPath m2m:APoCPath { list wraphidden } // (list size: [0, infinity[)
        // }
        if (LOG.isDebugEnabled()) {
            LOG.debug("getAndCheckAPoCPaths: (path: " + path + ") (tagName:" + tagName + ") (mode: " + mode + ") (targetPath:"
                    + targetPath + ")");
        }
        Set subPaths = null;
        XoObject apocPaths = representation.getXoObjectAttribute(tagName);
        checkMode(apocPaths, tagName, mode);
        if (apocPaths != null) {
            String subPath = null;
            subPaths = new HashSet();
            List apocPath = apocPaths.getXoObjectListAttribute(M2MConstants.TAG_M2M_A_PO_C_PATH);
            Iterator it = apocPath.iterator();
            XoObject apocPathObj = null;
            while (it.hasNext()) {
                apocPathObj = (XoObject) it.next();
                subPath = checkAPoCPath(manager, path, apocPathObj, M2MConstants.TAG_M2M_A_PO_C_PATH, targetPath);
                if (subPaths.contains(subPath)) {
                    throw new M2MException("Two distincts aPoCPaths point to the same sub-path: " + subPath,
                            StatusCode.STATUS_BAD_REQUEST);
                }
                subPaths.add(subPath);
            }
        }
        return subPaths;
    }

    public void createStringOptional(XoObject resource, XoObject representation, String tagName) {
        resource.setStringAttribute(tagName, representation.getStringAttribute(tagName));
    }

    public void updateStringOptional(XoObject resource, XoObject representation, String tagName) {
        resource.setStringAttribute(tagName, representation.getStringAttribute(tagName));
    }

    public void createStringMandatory(XoObject resource, XoObject representation, String tagName, String defaultValue) {
        String value = representation.getStringAttribute(tagName);
        if (value == null) {
            value = defaultValue;
        }
        resource.setStringAttribute(tagName, value);
    }

    public void updateStringMandatory(XoObject resource, XoObject representation, String tagName, String defaultValue) {
        String value = representation.getStringAttribute(tagName);
        if (value == null) {
            value = defaultValue;
        }
        resource.setStringAttribute(tagName, value);
    }

    public void createXoObjectOptional(XoObject resource, XoObject representation, String tagName) {
        XoObject optional = representation.getXoObjectAttribute(tagName);
        resource.setXoObjectAttribute(tagName, optional);
        if (optional != null) {
            representation.setXoObjectAttribute(tagName, null);
        }
    }

    public void updateXoObjectOptional(XoObject resource, XoObject representation, String tagName) {
        XoObject oldOptional = resource.getXoObjectAttribute(tagName);
        if (oldOptional != null) {
            oldOptional.free(true);
        }
        createXoObjectOptional(resource, representation, tagName);
    }

    public void createXoObjectMandatory(SclManager manager, XoObject resource, XoObject representation, String tagName)
            throws XoException {
        XoObject mandatory = representation.getXoObjectAttribute(tagName);
        if (mandatory == null) {
            resource.setXoObjectAttribute(tagName, manager.getXoService().newXmlXoObject(tagName));
        } else {
            representation.setXoObjectAttribute(tagName, null);
            resource.setXoObjectAttribute(tagName, mandatory);
        }
    }

    public void updateXoObjectMandatory(SclManager manager, XoObject resource, XoObject representation, String tagName)
            throws XoException {
        resource.getXoObjectAttribute(tagName).free(true);
        createXoObjectMandatory(manager, resource, representation, tagName);
    }

    public boolean setMaxNrOfInstances(SclManager manager, XoObject resource, long maxNrOfInstances, boolean useDefault) {
        SclConfig config = manager.getSclConfig();
        long computedMaxNrOfInstances = maxNrOfInstances;
        if (computedMaxNrOfInstances == -1L) {
            if (config.getDefaultMaxNrOfInstances() > 0L) {
                if (useDefault) {
                    computedMaxNrOfInstances = config.getDefaultMaxNrOfInstances();
                } else {
                    computedMaxNrOfInstances = config.getMaxMaxNrOfInstances();
                }
            }
        } else if (computedMaxNrOfInstances < config.getMinMaxNrOfInstances()) {
            computedMaxNrOfInstances = config.getMinMaxNrOfInstances();
        } else if (computedMaxNrOfInstances > config.getMaxMaxNrOfInstances()) {
            computedMaxNrOfInstances = config.getMaxMaxNrOfInstances();
        }
        if (computedMaxNrOfInstances != -1L) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, String.valueOf(computedMaxNrOfInstances));
        } else {
            resource.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, null);
        }

        return computedMaxNrOfInstances != maxNrOfInstances;
    }

    public boolean setMaxByteSize(SclManager manager, XoObject resource, long maxByteSize, boolean useDefault) {
        SclConfig config = manager.getSclConfig();
        long computedMaxByteSize = maxByteSize;

        if (computedMaxByteSize == -1L) {
            if (config.getDefaultMaxByteSize() > 0L) {
                if (useDefault) {
                    computedMaxByteSize = config.getDefaultMaxByteSize();
                } else {
                    computedMaxByteSize = config.getMaxMaxByteSize();
                }
            }
        } else if (computedMaxByteSize < config.getMinMaxByteSize()) {
            computedMaxByteSize = config.getMinMaxByteSize();
        } else if (computedMaxByteSize > config.getMaxMaxByteSize()) {
            computedMaxByteSize = config.getMaxMaxByteSize();
        }
        if (computedMaxByteSize != -1L) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_MAX_BYTE_SIZE, String.valueOf(computedMaxByteSize));
        } else {
            resource.setStringAttribute(M2MConstants.TAG_M2M_MAX_BYTE_SIZE, null);
        }

        return computedMaxByteSize != maxByteSize;
    }

    public boolean setMaxInstanceAge(SclManager manager, XoObject resource, long maxInstanceAge, boolean useDefault) {
        SclConfig config = manager.getSclConfig();
        long computedMaxInstanceAge = maxInstanceAge;
        if (computedMaxInstanceAge == -1L) {
            if (config.getDefaultMaxInstanceAge() > 0L) {
                if (useDefault) {
                    computedMaxInstanceAge = config.getDefaultMaxInstanceAge();
                } else {
                    computedMaxInstanceAge = config.getMaxMaxInstanceAge();
                }
            }
        } else if (computedMaxInstanceAge < config.getMinMaxInstanceAge()) {
            computedMaxInstanceAge = config.getMinMaxInstanceAge();
        } else if (computedMaxInstanceAge > config.getMaxMaxInstanceAge()) {
            computedMaxInstanceAge = config.getMaxMaxInstanceAge();
        }
        if (computedMaxInstanceAge != -1L) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_MAX_INSTANCE_AGE, String.valueOf(computedMaxInstanceAge));
        } else {
            resource.setStringAttribute(M2MConstants.TAG_M2M_MAX_INSTANCE_AGE, null);
        }

        return computedMaxInstanceAge != maxInstanceAge;
    }

    public Date setExpirationTime(SclManager manager, XoObject resource, long now, Date expirationTime) {
        SclConfig config = manager.getSclConfig();
        Date computedExpirationTime = expirationTime;
        if (computedExpirationTime == null) {
            if (config.getDefaultExpirationDuration() > 0L) {
                computedExpirationTime = new Date(now + config.getDefaultExpirationDuration());
            } else {
                computedExpirationTime = new Date(FormatUtils.MAX_DATE);
            }
        } else if ((computedExpirationTime.getTime() - now) < config.getMinExpirationDuration()) {
            computedExpirationTime = new Date(now + config.getMinExpirationDuration());
        } else if ((computedExpirationTime.getTime() - now) > config.getMaxExpirationDuration()) {
            computedExpirationTime = new Date(now + config.getMaxExpirationDuration());
        }
        resource.setStringAttribute(M2MConstants.TAG_M2M_EXPIRATION_TIME,
                FormatUtils.formatDateTime(computedExpirationTime, manager.getTimeZone()));
        return computedExpirationTime;
    }

    public void updateLastModifiedTime(SclManager manager, XoObject resource, Date now) {
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME,
                FormatUtils.formatDateTime(now, manager.getTimeZone()));
    }

    public XoObject generateNamedReferenceCollection(String logId, SclManager manager, SclResource controller,
            URI requestingEntity, String path, XoObject resource, Map children, String tagName)
            throws UnsupportedEncodingException, XoException {
        Iterator it = children.entrySet().iterator();
        XoService xoService = manager.getXoService();
        XoObject namedReferenceCollection = xoService.newXmlXoObject(tagName);
        resource.setXoObjectAttribute(tagName, namedReferenceCollection);
        List namedReferenceCollectionList = namedReferenceCollection
                .getXoObjectListAttribute(M2MConstants.TAG_M2M_NAMED_REFERENCE);
        int pathLength = path.length();
        String subPath = null;
        XoObject referenceToNamedResource = null;
        Entry entry = null;
        byte[] rawXoObject = null;
        XoObject xoObject = null;
        while (it.hasNext()) {
            entry = (Entry) it.next();
            subPath = ((String) entry.getKey());
            rawXoObject = (byte[]) entry.getValue();
            xoObject = manager.getXoService().readBinaryXmlXoObject(rawXoObject);
            try {
                controller.checkRights(logId, manager, subPath, xoObject, requestingEntity, M2MConstants.FLAG_DISCOVER);
                subPath = subPath.substring(pathLength + 1, subPath.length());

                referenceToNamedResource = xoService.newXmlXoObject(M2MConstants.TAG_M2M_NAMED_REFERENCE);
                referenceToNamedResource.setStringAttribute(XoObject.ATTR_VALUE, URIUtils.encodePath(subPath)
                        + M2MConstants.URI_SEP);
                referenceToNamedResource.setStringAttribute(M2MConstants.ATTR_ID, subPath);
                namedReferenceCollectionList.add(referenceToNamedResource);
            } catch (IOException e) {
                LOG.error("IOException occurred while building namedReferenceCollection", e);
            } catch (StorageException e) {
                LOG.error("StorageException occurred while building namedReferenceCollection", e);
            } catch (XoException e) {
                LOG.error("XoException occurred while building namedReferenceCollection", e);
            } catch (M2MException e) {
                // Right not granted
            } finally {
                xoObject.free(true);
            }
        }
        return namedReferenceCollection;
    }

    public void getAndCheckAccessRight(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            String flag) throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        String accessRightID = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRightID == null) {
            // Check default access rights
            checkDefaultAccessRight(logId, manager, path, requestingEntity);
        } else {
            retrieveAndCheckPermissionFromAccessRightID(logId, manager, path, requestingEntity, flag, accessRightID);
        }
    }

    public void fixAPoCPath(SclManager manager, String path, XoObject apocPath) {
        // m2m:APoCPath from ong:t_xml_obj
        // {
        // m2m:path XoString { } // (xmlType: xsd:anyURI)
        // m2m:accessRightID XoString { } // (optional) (xmlType: xsd:anyURI)
        // m2m:searchStrings m2m:SearchStrings { } // (optional)
        // }
        String appPath = manager.getM2MContext().getApplicationPath();
        String oldPath = apocPath.getStringAttribute(M2MConstants.TAG_M2M_PATH);
        String oldAccessRightID = apocPath.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        apocPath.setStringAttribute(M2MConstants.TAG_M2M_PATH, appPath + path + M2MConstants.URI_SEP + oldPath);
        if (oldAccessRightID != null) {
            apocPath.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + oldAccessRightID);
        }
    }

    public void getAndFixAPoCPaths(SclManager manager, String path, XoObject resource, String tagName) {
        // m2m:APoCPaths from ong:t_xml_obj
        // {
        // m2m:aPoCPath m2m:APoCPath { list wraphidden } // (list size: [0, infinity[)
        // }
        XoObject apocPaths = resource.getXoObjectAttribute(tagName);
        if (apocPaths != null) {
            List apocPath = apocPaths.getXoObjectListAttribute(M2MConstants.TAG_M2M_A_PO_C_PATH);
            Iterator it = apocPath.iterator();
            XoObject apocPathObj = null;
            while (it.hasNext()) {
                apocPathObj = (XoObject) it.next();
                fixAPoCPath(manager, path, apocPathObj);
            }
        }
    }

    public static Map createStorageConfiguration(XoObject resource, String tagName) throws M2MException {
        Map configuration = null;
        XoObject storageConfiguration = resource.getXoObjectAttribute(tagName);
        if (storageConfiguration != null) {
            List paramList = storageConfiguration.getXoObjectListAttribute(M2MConstants.TAG_ACY_PARAM);
            if (paramList.size() > 0) {
                configuration = new HashMap();
                Iterator it = paramList.iterator();
                XoObject param = null;
                while (it.hasNext()) {
                    param = (XoObject) it.next();
                    configuration.put(param.getStringAttribute(M2MConstants.ATTR_NAME),
                            param.getStringAttribute(M2MConstants.ATTR_VAL));
                }
            }
        }
        return configuration;
    }

    private boolean checkAll(String logId, XoObject permissionHolders) {
        boolean allowed = permissionHolders.getXoObjectAttribute(M2MConstants.TAG_M2M_ALL) != null;
        if (allowed && LOG.isInfoEnabled()) {
            LOG.info(logId + ": permission matched with all");
        }
        return allowed;
    }

    private boolean checkHolderRefs(String logId, XoObject permissionHolders, String requestingEntity) {
        boolean allowed = false;
        XoObject holderRefs = permissionHolders.getXoObjectAttribute(M2MConstants.TAG_M2M_HOLDER_REFS);
        if (holderRefs != null) {
            List holderRefsList = holderRefs.getStringListAttribute(M2MConstants.TAG_M2M_HOLDER_REF);
            allowed = holderRefsList.contains(requestingEntity);
        }
        if (allowed && LOG.isInfoEnabled()) {
            LOG.info(logId + ": permission matched with holderRef");
        }
        return allowed;
    }

    private boolean checkDomains(String logId, XoObject permissionHolders, String requestingEntity, URI requestingEntityUri) {
        boolean allowed = false;
        XoObject domains = permissionHolders.getXoObjectAttribute(M2MConstants.TAG_M2M_DOMAINS);
        if (domains != null) {
            List domainsList = domains.getStringListAttribute(M2MConstants.TAG_M2M_DOMAIN);
            Iterator domainsListIt = domainsList.iterator();
            String domain = null;
            URI domainUri = null;
            while (!allowed && domainsListIt.hasNext()) {
                domain = (String) domainsListIt.next();
                try {
                    domainUri = new URI(domain);
                    if (domainUri.getPath() != null && requestingEntity.startsWith(domain)) {
                        allowed = true;
                        if (LOG.isInfoEnabled()) {
                            LOG.info(logId + ": permission matched with domain");
                        }
                    } else if (requestingEntityUri.getHost() != null
                            && requestingEntityUri.getHost().endsWith(domainUri.getHost())) {
                        allowed = true;
                        if (LOG.isInfoEnabled()) {
                            LOG.info(logId + ": permission matched with domain suffix");
                        }
                    }
                } catch (URISyntaxException e) {
                    LOG.error("Invalid domain found in accessRight: " + domain, e);
                }
            }
        }
        return allowed;
    }

    private boolean checkAppIDorSclID(String logId, XoObject permissionHolders, String requestingEntity) {
        boolean allowed = false;
        int slashIndex = requestingEntity.lastIndexOf('/', requestingEntity.length() - 2);
        if (slashIndex >= 0) {
            int slashIndex2 = requestingEntity.lastIndexOf('/', slashIndex - 1);
            if (slashIndex2 >= 0) {
                try {
                    if (requestingEntity.regionMatches(true, slashIndex2 + 1, M2MConstants.RES_APPLICATIONS, 0, slashIndex
                            - slashIndex2 - 1)) {
                        // Check applicationID
                        String appID = null;
                        if (requestingEntity.charAt(requestingEntity.length() - 1) == '/') {
                            appID = URLDecoder.decode(
                                    requestingEntity.substring(slashIndex + 1, requestingEntity.length() - 1),
                                    M2MConstants.ENC_UTF8);
                        } else {
                            appID = URLDecoder.decode(requestingEntity.substring(slashIndex + 1), M2MConstants.ENC_UTF8);
                        }
                        XoObject appIDs = permissionHolders.getXoObjectAttribute(M2MConstants.TAG_M2M_APPLICATION_I_DS);
                        if (appIDs != null) {
                            List appIDsList = appIDs.getStringListAttribute(M2MConstants.TAG_M2M_APPLICATION_I_D);
                            allowed = appIDsList.contains(appID);
                            if (allowed && LOG.isInfoEnabled()) {
                                LOG.info(logId + ": permission matched with appID");
                            }
                        }
                    } else if (requestingEntity.regionMatches(true, slashIndex2 + 1, M2MConstants.RES_SCLS, 0, slashIndex
                            - slashIndex2 - 1)) {
                        // Check sclID
                        String sclID = null;
                        if (requestingEntity.charAt(requestingEntity.length() - 1) == '/') {
                            sclID = URLDecoder.decode(
                                    requestingEntity.substring(slashIndex + 1, requestingEntity.length() - 1),
                                    M2MConstants.ENC_UTF8);
                        } else {
                            sclID = URLDecoder.decode(requestingEntity.substring(slashIndex + 1), M2MConstants.ENC_UTF8);
                        }
                        XoObject sclIDs = permissionHolders.getXoObjectAttribute(M2MConstants.TAG_M2M_SCL_I_DS);
                        if (sclIDs != null) {
                            List sclIDsList = sclIDs.getStringListAttribute(M2MConstants.TAG_M2M_SCL_I_D);
                            allowed = sclIDsList.contains(sclID);
                            if (allowed && LOG.isInfoEnabled()) {
                                LOG.info(logId + ": permission matched with sclID");
                            }
                        }
                    }
                } catch (UnsupportedEncodingException e) {
                    LOG.error(M2MConstants.ENC_UTF8 + " is not supported by the current JVM", e);
                }
            }
        }
        return allowed;
    }

    public void checkPermissions(String logId, SclManager manager, URI requestingEntity, String flag, XoObject permissions)
            throws M2MException {
        boolean discover = false;
        String localSclBase = manager.getM2MContext().createLocalUri(requestingEntity, Constants.PATH_ROOT).toString();
        boolean allowed = localSclBase.equals(requestingEntity.toString());
        List permissionsList = permissions.getXoObjectListAttribute(M2MConstants.TAG_M2M_PERMISSION);
        Iterator it = permissionsList.iterator();
        XoObject permission = null;
        XoObject permissionHolders = null;
        String requestingEntityStr = null;
        XoObject permissionFlags = null;
        List permissionFlagsList = null;
        while (!allowed && it.hasNext()) {
            permission = (XoObject) it.next();
            permissionHolders = permission.getXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_HOLDERS);
            if (permissionHolders != null) {
                requestingEntityStr = requestingEntity.toString();
                allowed = checkAll(logId, permissionHolders);
                if (!allowed) {
                    allowed = checkHolderRefs(logId, permissionHolders, requestingEntityStr);
                }
                if (!allowed) {
                    allowed = checkDomains(logId, permissionHolders, requestingEntityStr, requestingEntity);
                }
                if (!allowed) {
                    allowed = checkAppIDorSclID(logId, permissionHolders, requestingEntityStr);
                }
            }
            if (allowed) {
                // Check rights
                permissionFlags = permission.getXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_FLAGS);
                if (permissionFlags != null) {
                    permissionFlagsList = permissionFlags.getStringListAttribute(M2MConstants.TAG_M2M_FLAG);
                    discover = discover || (permissionFlagsList.size() != 0);
                    if (discover && LOG.isInfoEnabled()) {
                        LOG.info(logId + ": \"DISCOVER\" right granted from permission");
                    }
                    if (flag.equals(M2MConstants.FLAG_DISCOVER)) {
                        allowed = discover;
                    } else {
                        allowed = permissionFlagsList.contains(flag);
                    }
                    if (LOG.isInfoEnabled()) {
                        if (allowed) {
                            LOG.info(logId + ": \"" + flag + "\" right granted from permission");
                        } else {
                            LOG.info(logId + ": \"" + flag + "\" right not granted from permission. Check next persmission");
                        }
                    }
                }
            }
        }
        if (!allowed) {
            if (discover) {
                if (LOG.isInfoEnabled()) {
                    LOG.info(logId + ": requestingEntity has not \"" + flag + "\" right, send PERMISSION DENIED");
                }
                throw new M2MException(flag + " access is required", StatusCode.STATUS_PERMISSION_DENIED);
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(logId + ": requestingEntity has not \"" + flag + "\" right neither \"DISCOVER\" right, send NOT FOUND");
            }
            throw new M2MException("Resource does not exist", StatusCode.STATUS_NOT_FOUND);
        }
    }

    public void retrieveAndCheckPermissionFromAccessRightID(String logId, SclManager manager, String path,
            URI requestingEntity, String flag, String accessRightID) throws UnsupportedEncodingException, StorageException,
            XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(logId + ": Check if requestingEntity \"" + requestingEntity.toString() + "\" has \"" + flag
                    + "\" right permission against accessRight on path " + accessRightID);
        }
        // Check default access rights
        XoObject accessRight = null;
        try {
            accessRight = manager.getXoResource(accessRightID);
            if (accessRight == null) {
                LOG.error(logId + ": Cannot retrieve accessRight on path " + accessRightID + ". Use default rights instead");
                checkDefaultAccessRight(logId, manager, path, requestingEntity);
            } else {
                XoObject permissions = accessRight.getXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSIONS);
                checkPermissions(logId, manager, requestingEntity, flag, permissions);
            }
        } finally {
            if (accessRight != null) {
                accessRight.free(true);
            }
        }
    }

    public void checkDefaultAccessRight(String logId, SclManager manager, String path, URI requestingEntity)
            throws M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(logId + ": Check if requestingEntity \"" + requestingEntity.toString() + "\" is matching default rights");
        }
        // Check default access rights
        String requestingEntityStr = requestingEntity.toString();
        String localSclBase = null;
        if (requestingEntityStr.charAt(requestingEntityStr.length() - 1) == '/') {
            localSclBase = manager.getM2MContext().createLocalUri(requestingEntity, M2MConstants.URI_SEP).toString();
        } else {
            localSclBase = manager.getM2MContext().createLocalUri(requestingEntity, Constants.PATH_ROOT).toString();
        }
        if (!localSclBase.equals(requestingEntityStr)) {
            String localResourceUri = manager.getM2MContext().createLocalUri(requestingEntity, path).toString();
            if (path.startsWith(Constants.PATH_SCLS) && path.length() > Constants.PATH_SCLS.length()) {
                if (LOG.isInfoEnabled()) {
                    LOG.info(logId + ": check default rights for a <scl> using local uri: " + localResourceUri);
                }
                if (!localResourceUri.startsWith(requestingEntityStr)) {
                    // <scl> permission
                    if (LOG.isInfoEnabled()) {
                        LOG.info(logId + ": requestingEntity is not matching default rights, send NOT FOUND");
                    }
                    throw new M2MException("Resource does not exist", StatusCode.STATUS_NOT_FOUND);
                }
            } else if (path.startsWith(Constants.PATH_APPLICATIONS) && path.length() > Constants.PATH_APPLICATIONS.length()) {
                if (LOG.isInfoEnabled()) {
                    LOG.info(logId + ": check default rights for an <application> using local uri: " + localResourceUri);
                }
                if (!localResourceUri.startsWith(requestingEntityStr)) {
                    // <application> permission
                    if (LOG.isInfoEnabled()) {
                        LOG.info(logId + ": requestingEntity is not matching default rights, send NOT FOUND");
                    }
                    throw new M2MException("Resource does not exist", StatusCode.STATUS_NOT_FOUND);
                }
            } else {
                // <sclBase> permission
                if (LOG.isInfoEnabled()) {
                    LOG.info(logId + ": this is not a <scl> nor an <application> path and requesting is not the local sclBase");
                    LOG.info(logId + ": requestingEntity is not matching default rights, send NOT FOUND");
                }
                throw new M2MException("Resource does not exist", StatusCode.STATUS_NOT_FOUND);
            }
        }
    }

    public FilterCriteria getFilterCriteria(Indication indication) throws M2MException {
        switch (filterCriteriaMode) {
        case Constants.ID_NO_FILTER_CRITERIA_MODE:
            return null;
        case Constants.ID_FILTER_CRITERIA_MODE:
            return indication.getFilterCriteria();
        case Constants.ID_CONTENT_INSTANCE_FILTER_CRITERIA_MODE:
            return indication.getContentInstanceFilterCriteria();
        }
        return null;
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, XoException, StorageException, M2MException {
        sendMethodNotAllowed(manager, path, indication, false, true, updateRequestIndication != null,
                deleteRequestIndication != null);
    }

    public void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws IOException, XoException, StorageException, M2MException {
        SclLogger.logRequestIndication(retrieveRequestIndication, retrieveResponseConfirm, indication, null, 0);

        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": Check " + M2MConstants.FLAG_READ + " right on " + path);
        }
        checkRights(indication.getTransactionId(), manager, path, resource, indication.getRequestingEntity(),
                M2MConstants.FLAG_READ);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": " + M2MConstants.FLAG_READ + " right granted on " + path);
        }

        String eTag = null;
        long lastModified = -1;
        if (hasLastModifiedTag) {
            eTag = resource.getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME);

            try {
                lastModified = FormatUtils.parseDateTime(eTag).getTime();
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Cannot parse " + M2MConstants.TAG_M2M_LAST_MODIFIED_TIME + " '"
                        + eTag + "' of resource on path '" + path + "'", e);
            }
        }

        if (indication.checkConditions(eTag, lastModified, Indication.CONDITION_READ)) {
            sendSuccessResponse(manager, path, resource, indication, StatusCode.STATUS_OK, true, eTag, lastModified);
        }
    }

    public void doUpdateIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, XoException, StorageException, M2MException {
        if (updateRequestIndication != null) {
            SclLogger.logRequestIndication(updateRequestIndication, updateResponseConfirm, indication, null,
                    Constants.ID_LOG_REPRESENTATION);

            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": Check " + M2MConstants.FLAG_WRITE + " right on " + path);
            }
            checkRights(indication.getTransactionId(), manager, path, resource, indication.getRequestingEntity(),
                    M2MConstants.FLAG_WRITE);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": " + M2MConstants.FLAG_WRITE + " right granted on " + path);
            }

            String eTag = null;
            long lastModified = -1;
            if (hasLastModifiedTag) {
                eTag = resource.getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME);

                try {
                    lastModified = FormatUtils.parseDateTime(eTag).getTime();
                } catch (ParseException e) {
                    LOG.error(indication.getTransactionId() + ": Cannot parse " + M2MConstants.TAG_M2M_LAST_MODIFIED_TIME
                            + " '" + eTag + "' of resource on path '" + path + "'", e);
                }
            }

            if (indication.checkConditions(eTag, lastModified, Indication.CONDITION_WRITE)) {
                XoObject representation = indication.getRepresentation(null);
                checkRepresentationNotNull(representation);
                boolean sendRepresentation = false;
                Date now = new Date();
                try {
                    sendRepresentation = updateResource(indication.getTransactionId(), manager, path, resource, indication,
                            representation, now);
                } finally {
                    representation.free(true);
                }
                sendSuccessResponse(manager, path, resource, indication, StatusCode.STATUS_OK, sendRepresentation, eTag,
                        lastModified);

                if (hasSubscriptionsOnResource) {
                    SubsManager subscriptions = (SubsManager) manager.getM2MContext().getAttribute(
                            path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS);
                    if (subscriptions != null) {
                        subscriptions.resourceUpdated(resource, sendRepresentation, now.getTime());
                    }
                }
            }
        } else {
            sendMethodNotAllowed(manager, path, indication, collection, true, false, deleteRequestIndication != null);
        }
    }

    public void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        if (deleteRequestIndication != null) {
            SclLogger.logRequestIndication(deleteRequestIndication, deleteResponseConfirm, indication, null, 0);

            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": Check " + M2MConstants.FLAG_DELETE + " right on " + path);
            }
            checkRights(indication.getTransactionId(), manager, path, resource, indication.getRequestingEntity(),
                    M2MConstants.FLAG_DELETE);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": " + M2MConstants.FLAG_DELETE + " right granted on " + path);
            }

            SclTransaction transaction = new SclTransaction(manager.getStorageContext());

            if (parentResourceController != null) {
                Date now = new Date();
                String parentPath = getParentPath(path);
                XoObject parentResource = null;

                try {
                    parentResource = manager.getXoResource(parentPath);
                    // parentResource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME,
                    // FormatUtils.formatDateTime(now, manager.getTimeZone()));
                    // transaction.updateResource(parentPath, parentResource, null);
                    parentResourceController.deleteChildResource(indication.getTransactionId(), manager, parentPath,
                            parentResource, resource, now, transaction);

                    deleteResource(indication.getTransactionId(), manager, path, resource, transaction);

                    transaction.execute();

                    sendDeleteSuccessResponse(indication);

                    if (parentHasSubscriptionsOnResource) {
                        SubsManager subscriptions = (SubsManager) manager.getM2MContext().getAttribute(
                                parentPath + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS);
                        if (subscriptions != null) {
                            subscriptions.resourceUpdated(parentResource, false, now.getTime());
                        }
                    }
                } finally {
                    if (parentResource != null) {
                        parentResource.free(true);
                    }
                }
            } else {
                deleteResource(indication.getTransactionId(), manager, path, resource, transaction);

                transaction.execute();

                sendDeleteSuccessResponse(indication);
            }
        } else {
            sendMethodNotAllowed(manager, path, indication, collection, true, updateRequestIndication != null, false);
        }
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on " + path
                + " is not implemented yet");
    }

    public void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on " + path
                + " is not implemented yet");
    }

    public void doUpdateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on " + path
                + " is not implemented yet");
    }

    public void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on " + path
                + " is not implemented yet");
    }

    public void timeout(SclManager manager, String path, String timerId, M2MSession session, Serializable info)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        throw new UnsupportedOperationException();
    }

    public void sessionExpired(SclManager manager, String path, M2MSession session) {
        throw new UnsupportedOperationException();
    }

    public void updatePermissionType(SclManager manager, String path, XoObject resource, List parent, int index,
            XoObject attribute, Indication indication, PartialAccessor partialAccessor, Iterator subPathIt, int method,
            boolean writable, String tagName) throws IOException, ParseException, StorageException, XoException, M2MException {
        if (subPathIt.hasNext()) {
            sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on \"" + path
                    + "\" is not implemented");
        } else {
            SclLogger.logRequestIndication(updateRequestIndication, updateResponseConfirm, indication, null,
                    Constants.ID_LOG_REPRESENTATION);

            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": Check " + M2MConstants.FLAG_WRITE + " right on " + path);
            }
            checkRights(indication.getTransactionId(), manager, path, resource, indication.getRequestingEntity(),
                    M2MConstants.FLAG_WRITE);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": " + M2MConstants.FLAG_WRITE + " right granted on " + path);
            }

            String eTag = null;
            long lastModified = -1;
            eTag = resource.getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME);

            try {
                lastModified = FormatUtils.parseDateTime(eTag).getTime();
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Cannot parse " + M2MConstants.TAG_M2M_LAST_MODIFIED_TIME + " '"
                        + eTag + "' of resource on path '" + path + "'", e);
            }

            if (indication.checkConditions(eTag, lastModified, Indication.CONDITION_WRITE)) {
                Date now = new Date();
                switch (method) {
                case Constants.ID_MD_CREATE:
                    sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on \""
                            + path + "\" is not implemented");
                    break;
                case Constants.ID_MD_UPDATE:
                    sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on \""
                            + path + "\" is not implemented");
                    break;
                case Constants.ID_MD_DELETE:
                    attribute.free(true);
                    parent.remove(index);

                    updateLastModifiedTime(manager, resource, now);

                    // Save resource
                    // TODO should be done by resource as this is specific
                    Collection searchAttributes = new ArrayList();
                    searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                            .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
                    searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
                    manager.getStorageContext().update(path, resource.saveBinary(), searchAttributes);
                    sendSuccessResponse(manager, path, resource, indication, StatusCode.STATUS_OK, false, eTag, lastModified);
                    break;
                default:
                    throw new M2MException("Use of an unknown method: " + method, StatusCode.STATUS_INTERNAL_SERVER_ERROR);
                }

                if (hasSubscriptionsOnResource) {
                    SubsManager subscriptions = (SubsManager) manager.getM2MContext().getAttribute(
                            path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS);
                    if (subscriptions != null) {
                        subscriptions.resourceUpdated(resource, false, now.getTime());
                    }
                }
            }
        }
    }

    public void getAndUpdatePermissionListType(SclManager manager, String path, XoObject resource, XoObject parent,
            Indication indication, PartialAccessor partialAccessor, Iterator subPathIt, int method, boolean writable,
            String tagName, int mode) throws IOException, ParseException, StorageException, XoException, M2MException {
        XoObject attribute = parent.getXoObjectAttribute(tagName);
        checkMode(attribute, tagName, mode);

        if (subPathIt.hasNext()) {
            if (attribute == null) {
                sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_FOUND, "Resource on path \"" + path
                        + "\" does not exist");
            } else {
                // TODO fetch and continue
                List permissionsList = attribute.getXoObjectListAttribute(M2MConstants.TAG_M2M_PERMISSION);
                String permissionId = (String) subPathIt.next();
                Iterator it = permissionsList.iterator();
                XoObject permission = null;
                boolean found = false;
                int index = 0;
                while (it.hasNext() && !found) {
                    permission = (XoObject) it.next();
                    if (permissionId.equals(permission.getStringAttribute(M2MConstants.ATTR_M2M_ID))) {
                        found = true;
                    } else {
                        ++index;
                    }
                }
                if (found) {
                    updatePermissionType(manager, path, resource, permissionsList, index, permission, indication,
                            partialAccessor, subPathIt, method, writable, M2MConstants.TAG_M2M_PERMISSION);
                } else {
                    sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_FOUND, "Resource on path \"" + path
                            + "\" does not exist");
                }
            }
        } else {
            SclLogger.logRequestIndication(updateRequestIndication, updateResponseConfirm, indication, null,
                    Constants.ID_LOG_REPRESENTATION);

            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": Check " + M2MConstants.FLAG_WRITE + " right on " + path);
            }
            checkRights(indication.getTransactionId(), manager, path, resource, indication.getRequestingEntity(),
                    M2MConstants.FLAG_WRITE);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": " + M2MConstants.FLAG_WRITE + " right granted on " + path);
            }

            String eTag = null;
            long lastModified = -1;
            eTag = resource.getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME);

            try {
                lastModified = FormatUtils.parseDateTime(eTag).getTime();
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Cannot parse " + M2MConstants.TAG_M2M_LAST_MODIFIED_TIME + " '"
                        + eTag + "' of resource on path '" + path + "'", e);
            }

            if (indication.checkConditions(eTag, lastModified, Indication.CONDITION_WRITE)) {
                Date now = new Date();
                switch (method) {
                case Constants.ID_MD_CREATE:
                    if (attribute == null) {
                        sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_FOUND, "Resource on path \"" + path
                                + "\" does not exist");
                    } else {
                        XoObject representation = indication.getRepresentation(null);
                        checkRepresentationNotNull(representation);
                        boolean sendRepresentation = false;
                        try {
                            // Check representation
                            checkRepresentation(representation, M2MConstants.TAG_M2M_PERMISSION);
                            checkPermissionType(representation, M2MConstants.TAG_M2M_PERMISSION);

                            List permissionsList = attribute.getXoObjectListAttribute(M2MConstants.TAG_M2M_PERMISSION);

                            // TODO maybe avoid the copy
                            XoObject permission = representation.copy(true);
                            permission.clearNameSpaces();
                            // Check id
                            String id = permission.getStringAttribute(M2MConstants.ATTR_M2M_ID);
                            if (id == null) {
                                // Should check for collision
                                id = UUID.randomUUID(16);
                                permission.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
                            } else {
                                Iterator it = permissionsList.iterator();
                                while (it.hasNext()) {
                                    XoObject otherPermission = (XoObject) it.next();
                                    if (id.equals(otherPermission.getStringAttribute(M2MConstants.ATTR_M2M_ID))) {
                                        throw new M2MException("A <permission> already exists with the id " + id,
                                                StatusCode.STATUS_CONFLICT);
                                    }
                                }
                            }
                            permissionsList.add(permission);

                            updateLastModifiedTime(manager, resource, now);

                            // Save resource
                            // TODO should be done by resource as this is specific (here it is specific to <accessRight>)
                            Collection searchAttributes = new ArrayList();
                            searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_ACCESS_RIGHT));
                            XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
                            List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
                            Iterator it = searchStringList.iterator();
                            while (it.hasNext()) {
                                searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
                            }
                            searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                                    .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
                            searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));

                            manager.getStorageContext().update(path, resource.saveBinary(), searchAttributes);
                        } finally {
                            representation.free(true);
                        }
                        sendSuccessResponse(manager, path, resource, indication, StatusCode.STATUS_CREATED, sendRepresentation,
                                eTag, lastModified);
                    }
                    break;
                case Constants.ID_MD_UPDATE:
                    sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on \""
                            + path + "\" is not implemented");
                    break;
                case Constants.ID_MD_DELETE:
                    sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_IMPLEMENTED, "Partial addressing on \""
                            + path + "\" is not implemented");
                    break;
                default:
                    throw new M2MException("Use of an unknown method: " + method, StatusCode.STATUS_INTERNAL_SERVER_ERROR);
                }

                if (hasSubscriptionsOnResource) {
                    SubsManager subscriptions = (SubsManager) manager.getM2MContext().getAttribute(
                            path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS);
                    if (subscriptions != null) {
                        subscriptions.resourceUpdated(resource, false, now.getTime());
                    }
                }
            }
        }
    }

    // /////////////////////////////////////////////////////////////////////////
    // Generic tests
    // /////////////////////////////////////////////////////////////////////////
    public static void checkRepresentationNotNull(XoObject representation) throws M2MException {
        if (representation == null) {
            throw new M2MException("Received a request without body or with an unsupported body representation",
                    StatusCode.STATUS_BAD_REQUEST);
        }
    }

    public static void checkRepresentation(XoObject representation, String tagName) throws M2MException {
        if (representation == null) {
            throw new M2MException("Received a request without body or with an unsupported body representation",
                    StatusCode.STATUS_BAD_REQUEST);
        } else if (!tagName.equals(representation.getName())) {
            throw new M2MException("Bad representation. Expected " + tagName + " and received " + representation.getName(),
                    StatusCode.STATUS_BAD_REQUEST);
        }
    }

    public static String normalizePath(String tagName, String contextPath, String path, int slashManagement)
            throws M2MException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("normalizePath: (tagName:" + tagName + ") (contextPath: " + contextPath + ") (path:" + path
                    + ") (slashManagement: " + slashManagement + ")");
        }
        if (contextPath == null || contextPath.charAt(0) != '/') {
            throw new M2MException("Invalid contextPath used in normalization: " + contextPath,
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR);
        }
        StringBuffer normalizedPath = new StringBuffer();
        StringReader reader = new StringReader(path);
        try {
            int lastSlash = 0;
            if (reader.readCurrent() == '/') {
                normalizedPath.append('/');
                reader.skipOffset(1);
            } else if (contextPath.endsWith(M2MConstants.URI_SEP)) {
                normalizedPath.append(contextPath);
            } else {
                normalizedPath.append(contextPath.substring(0, contextPath.lastIndexOf('/') + 1));
            }
            String tmpPath = null;
            while (!reader.isTerminated()) {
                tmpPath = reader.readUntil('/');
                if (tmpPath.length() > 0 && !".".equals(tmpPath)) {
                    if ("..".equals(tmpPath)) {
                        // TODO fix IS2T
                        // lastSlash = normalizedPath.lastIndexOf(M2MConstants.URI_SEP, normalizedPath.length() - 2);
                        lastSlash = normalizedPath.length() - 2;
                        while (lastSlash >= 0) {
                            if (normalizedPath.charAt(lastSlash) == '/') {
                                break;
                            }
                            --lastSlash;
                        }
                        if (lastSlash == -1) {
                            throw new M2MException(tagName + " is not pointing to a valid path: " + path,
                                    StatusCode.STATUS_BAD_REQUEST);
                        }
                        normalizedPath.setLength(lastSlash + 1);
                    } else {
                        normalizedPath.append(tmpPath);
                        normalizedPath.append('/');
                    }
                }
                if (!reader.isTerminated()) {
                    reader.skipOffset(1);
                }
            }
        } catch (ParseException e) {
            throw new M2MException("Cannot decode path: " + e.getMessage(), StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        }
        switch (slashManagement) {
        case Constants.ID_SLASH_MGMT_LEAVE:
            if (path.charAt(path.length() - 1) != '/') {
                normalizedPath.setLength(normalizedPath.length() - 1);
            }
            break;
        case Constants.ID_SLASH_MGMT_ADD:
            break;
        case Constants.ID_SLASH_MGMT_REMOVE:
            normalizedPath.setLength(normalizedPath.length() - 1);
            break;
        default:
            break;
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("normalizePath result: " + normalizedPath.toString());
        }
        return normalizedPath.toString();
    }

    public static void checkMode(Object attribute, String tagName, int mode) throws M2MException {
        switch (mode) {
        case Constants.ID_MODE_FORBIDDEN:
            if (attribute != null) {
                throw new M2MException("Received representation declares a " + tagName + " attribute which is not authorized",
                        StatusCode.STATUS_BAD_REQUEST);
            }
            break;
        case Constants.ID_MODE_OPTIONAL:
            // OK
            break;
        case Constants.ID_MODE_REQUIRED:
            if (attribute == null) {
                throw new M2MException("Received representation does not declare a " + tagName
                        + " attribute which is mandatory", StatusCode.STATUS_BAD_REQUEST);
            }
            break;
        }
    }

    public static URI checkURI(String attribute, String tagName, int mode) throws M2MException {
        checkMode(attribute, tagName, mode);
        URI uri = null;
        try {
            if (attribute != null) {
                uri = new URI(attribute);
            }
        } catch (URISyntaxException e) {
            throw new M2MException(tagName + " attribute is not a valid URI: " + attribute, StatusCode.STATUS_BAD_REQUEST, e);
        }
        return uri;
    }

    public static int checkPermissionFlagType(String flag, String tagName) throws M2MException {
        if (M2MConstants.FLAG_CREATE.equals(flag)) {
            return Constants.ID_FLAG_CREATE;
        } else if (M2MConstants.FLAG_DELETE.equals(flag)) {
            return Constants.ID_FLAG_DELETE;
        } else if (M2MConstants.FLAG_DISCOVER.equals(flag)) {
            return Constants.ID_FLAG_DISCOVER;
        } else if (M2MConstants.FLAG_READ.equals(flag)) {
            return Constants.ID_FLAG_READ;
        } else if (M2MConstants.FLAG_WRITE.equals(flag)) {
            return Constants.ID_FLAG_WRITE;
        } else {
            throw new M2MException("Invalid " + tagName + " value: " + flag, StatusCode.STATUS_BAD_REQUEST);
        }
    }

    public static void checkPermissionType(XoObject permission, String tagName) throws M2MException {
        // m2m:PermissionType from ong:t_xml_obj
        // {
        // m2m:id XoString { embattr } // (optional) (xmlType: xsd:anyURI)
        // m2m:permissionFlags m2m:PermissionFlagListType { }
        // m2m:permissionHolders m2m:PermissionHolderType { }
        // }
        getAndCheckPermissionFlagListType(permission, M2MConstants.TAG_M2M_PERMISSION_FLAGS, Constants.ID_MODE_REQUIRED);
        getAndCheckPermissionHolderType(permission, M2MConstants.TAG_M2M_PERMISSION_HOLDERS, Constants.ID_MODE_REQUIRED);
    }

    public static void checkFilterCriteriaType(XoObject filterCriteria, String tagName, FilterCriteria fc) throws M2MException {
        // m2m:FilterCriteriaType from ong:t_xml_obj
        // {
        // ifModifiedSince XoString { } // (optional) (xmlType: xsd:dateTime)
        // ifUnmodifiedSince XoString { } // (optional) (xmlType: xsd:dateTime)
        // ifMatch XoString { list } // (xmlType: xsd:string) (list size: [0, infinity[)
        // ifNoneMatch XoString { list } // (xmlType: xsd:string) (list size: [0, infinity[)
        // }
        Date dateValue = getAndCheckDateTime(filterCriteria, M2MConstants.TAG_IF_MODIFIED_SINCE, Constants.ID_MODE_OPTIONAL,
                -1L);
        if (dateValue != null) {
            fc.setIfModifiedSince(dateValue);
        }
        dateValue = getAndCheckDateTime(filterCriteria, M2MConstants.TAG_IF_UNMODIFIED_SINCE, Constants.ID_MODE_OPTIONAL, -1L);
        if (dateValue != null) {
            fc.setIfUnmodifiedSince(dateValue);
        }
        // TODO stage 2 and 3 does not match on this definition
        // List listValue = getAndCheckStringList(filterCriteria, M2MConstants.TAG_IF_MATCH);
        // if (listValue != null) {
        // String[] arrayValue = null;
        // arrayValue = new String[listValue.size()];
        // listValue.toArray(arrayValue);
        // Arrays.sort(arrayValue);
        // fc.setIfMatch(arrayValue);
        // }
        List listValue = getAndCheckStringList(filterCriteria, M2MConstants.TAG_IF_NONE_MATCH);
        if (listValue != null) {
            String[] arrayValue = null;
            arrayValue = new String[listValue.size()];
            listValue.toArray(arrayValue);
            Arrays.sort(arrayValue);
            fc.setIfNoneMatch(arrayValue);
        }
    }

    public static int checkAPoCHandling(String aPoCHandling, String tagName) throws M2MException {
        if (M2MConstants.APOC_HANDLING_SHALLOW.equals(aPoCHandling)) {
            return Constants.ID_APOC_HANDLING_SHALLOW;
        } else if (M2MConstants.APOC_HANDLING_DEEP.equals(aPoCHandling)) {
            return Constants.ID_APOC_HANDLING_DEEP;
        } else {
            throw new M2MException("Invalid " + tagName + " value: " + aPoCHandling, StatusCode.STATUS_BAD_REQUEST);
        }
    }

    public static void checkPermissionListType(XoObject permissionList, String tagName) throws M2MException {
        List permission = permissionList.getXoObjectListAttribute(M2MConstants.TAG_M2M_PERMISSION);
        Iterator it = permission.iterator();
        XoObject permissionObj = null;
        while (it.hasNext()) {
            permissionObj = (XoObject) it.next();
            checkPermissionType(permissionObj, M2MConstants.TAG_M2M_PERMISSION);
        }
    }

    public static void getAndCheckXoObjectMode(XoObject representation, String tagName, int mode) throws M2MException {
        checkMode(representation.getXoObjectAttribute(tagName), tagName, mode);
    }

    public static void getAndCheckStringNonEmpty(XoObject representation, String tagName, int mode) throws M2MException {
        String value = representation.getStringAttribute(tagName);
        checkMode(value, tagName, mode);
        if (value != null && "".equals(value)) {
            throw new M2MException("Received representation declares a " + tagName + " attribute which has an empty value",
                    StatusCode.STATUS_BAD_REQUEST);
        }
    }

    public static void getAndCheckStringMode(XoObject representation, String tagName, int mode) throws M2MException {
        checkMode(representation.getStringAttribute(tagName), tagName, mode);
    }

    public static String getAndCheckRelativePath(XoObject representation, String tagName, int mode) throws M2MException {
        String attribute = representation.getStringAttribute(tagName);
        URI relativePath = checkURI(attribute, tagName, mode);
        if (attribute != null) {
            if (relativePath.isAbsolute()) {
                throw new M2MException(tagName + " is absolute but only relative URI are supported: " + attribute,
                        StatusCode.STATUS_PERMISSION_DENIED);
            }
            if (attribute.charAt(0) == '/') {
                throw new M2MException(tagName + " is an absolute path with is not authorized: " + attribute,
                        StatusCode.STATUS_PERMISSION_DENIED);
            }
            if (!attribute.endsWith(M2MConstants.URI_SEP)) {
                attribute += M2MConstants.URI_SEP;
                representation.setStringAttribute(tagName, attribute);
            }
        }
        return attribute;
    }

    public static URI getAndCheckReachableURI(XoObject representation, String tagName, int mode) throws M2MException {
        URI uri = getAndCheckURI(representation, tagName, mode);
        if (!uri.isAbsolute() && (uri.getPath() == null || uri.getPath().charAt(0) != '/')) {
            throw new M2MException("URI must be absolute, or start with a slash to be reachable. Received: " + uri.toString(),
                    StatusCode.STATUS_BAD_REQUEST);
        }
        return uri;
    }

    public static URI getAndCheckURI(XoObject representation, String tagName, int mode) throws M2MException {
        String attribute = representation.getStringAttribute(tagName);
        return checkURI(attribute, tagName, mode);
    }

    public static int getAndCheckXmimeBase64(XoObject representation, String tagName, int mode) throws M2MException {
        XoObject base64 = representation.getXoObjectAttribute(tagName);
        int size = -1;
        checkMode(base64, tagName, mode);
        if (base64 != null) {
            getAndCheckStringNonEmpty(base64, M2MConstants.TAG_XMIME_CONTENT_TYPE, Constants.ID_MODE_REQUIRED);
            size = getAndCheckBuffer(base64, XoObject.ATTR_VALUE, Constants.ID_MODE_REQUIRED);
        }
        return size;
    }

    public static int getAndCheckBuffer(XoObject representation, String tagName, int mode) throws M2MException {
        byte[] attribute = representation.getBufferAttribute(tagName);
        int size = -1;
        checkMode(attribute, tagName, mode);
        if (attribute != null) {
            size = attribute.length;
        }
        return size;
    }

    public static long getAndCheckLong(XoObject representation, String tagName, int mode) throws M2MException {
        String attribute = representation.getStringAttribute(tagName);
        checkMode(attribute, tagName, mode);
        long value = -1L;
        if (attribute != null) {
            try {
                value = Long.parseLong(attribute);
            } catch (NumberFormatException e) {
                throw new M2MException(tagName + " attribute is not a valid xsd:long: " + attribute,
                        StatusCode.STATUS_BAD_REQUEST, e);
            }
        }
        return value;
    }

    public static Integer getAndCheckInt(XoObject representation, String tagName, int mode) throws M2MException {
        String attribute = representation.getStringAttribute(tagName);
        checkMode(attribute, tagName, mode);
        if (attribute != null) {
            try {
                return Integer.valueOf(attribute);
            } catch (NumberFormatException e) {
                throw new M2MException(tagName + " attribute is not a valid xsd:int: " + attribute,
                        StatusCode.STATUS_BAD_REQUEST, e);
            }
        }
        return null;
    }

    public static Boolean getAndCheckBoolean(XoObject representation, String tagName, int mode) throws M2MException {
        String attribute = representation.getStringAttribute(tagName);
        checkMode(attribute, tagName, mode);
        if (attribute != null) {
            if (M2MConstants.BOOLEAN_FALSE.equals(attribute)) {
                return Boolean.FALSE;
            } else if (M2MConstants.BOOLEAN_TRUE.equals(attribute)) {
                return Boolean.TRUE;
            } else {
                throw new M2MException(tagName + " attribute is not a valid xsd:boolean: " + attribute,
                        StatusCode.STATUS_BAD_REQUEST);
            }
        }
        return null;
    }

    public static Date getAndCheckDateTime(XoObject representation, String tagName, int mode, long minValue)
            throws M2MException {
        String attribute = representation.getStringAttribute(tagName);
        checkMode(attribute, tagName, mode);
        Date dateAttribute = null;
        if (attribute != null) {
            try {
                dateAttribute = FormatUtils.parseDateTime(attribute);
                if (minValue != -1L && dateAttribute.getTime() < minValue) {
                    throw new M2MException(tagName + " attribute value is too short: " + attribute,
                            StatusCode.STATUS_BAD_REQUEST);
                }
            } catch (ParseException e) {
                throw new M2MException(tagName + " attribute is not a valid xsd:dateTime: " + attribute,
                        StatusCode.STATUS_BAD_REQUEST, e);
            }
        }
        return dateAttribute;
    }

    public static void getAndCheckDomainListType(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:DomainListType from ong:t_xml_obj
        // {
        // m2m:domain XoString { list } // (xmlType: xsd:anyURI) (list size: [0, infinity[)
        // }
        XoObject domainList = representation.getXoObjectAttribute(tagName);
        checkMode(domainList, tagName, mode);
        if (domainList != null) {
            List holderRef = domainList.getStringListAttribute(M2MConstants.TAG_M2M_DOMAIN);
            Iterator it = holderRef.iterator();
            String domainObj = null;
            while (it.hasNext()) {
                domainObj = (String) it.next();
                checkURI(domainObj, M2MConstants.TAG_M2M_DOMAIN, Constants.ID_MODE_REQUIRED);
            }
        }
    }

    public static void getAndCheckApplicationIDs(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:ApplicationIDs from ong:t_xml_obj
        // {
        // m2m:applicationID XoString { list } // (xmlType: xsd:string) (list size: [0, infinity[)
        // }
        XoObject applicationIDs = representation.getXoObjectAttribute(tagName);
        checkMode(applicationIDs, tagName, mode);
        if (applicationIDs != null) {
            List holderRef = applicationIDs.getStringListAttribute(M2MConstants.TAG_M2M_APPLICATION_I_D);
            Iterator it = holderRef.iterator();
            String applicationIDObj = null;
            while (it.hasNext()) {
                applicationIDObj = (String) it.next();
                checkURI(applicationIDObj, M2MConstants.TAG_M2M_APPLICATION_I_D, Constants.ID_MODE_REQUIRED);
            }
        }
    }

    public static void getAndCheckSclIDs(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:SclIDs from ong:t_xml_obj
        // {
        // m2m:sclID XoString { list } // (xmlType: xsd:string) (list size: [0, infinity[)
        // }
        XoObject sclIDs = representation.getXoObjectAttribute(tagName);
        checkMode(sclIDs, tagName, mode);
        if (sclIDs != null) {
            List holderRef = sclIDs.getStringListAttribute(M2MConstants.TAG_M2M_SCL_I_D);
            Iterator it = holderRef.iterator();
            String sclIDObj = null;
            while (it.hasNext()) {
                sclIDObj = (String) it.next();
                checkURI(sclIDObj, M2MConstants.TAG_M2M_SCL_I_D, Constants.ID_MODE_REQUIRED);
            }
        }
    }

    public static void getAndCheckHolderRefListType(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:HolderRefListType from ong:t_xml_obj
        // {
        // m2m:holderRef XoString { list } // (xmlType: xsd:anyURI) (list size: [0, infinity[)
        // }
        XoObject holderRefList = representation.getXoObjectAttribute(tagName);
        checkMode(holderRefList, tagName, mode);
        if (holderRefList != null) {
            List holderRef = holderRefList.getStringListAttribute(M2MConstants.TAG_M2M_HOLDER_REF);
            Iterator it = holderRef.iterator();
            String holderRefObj = null;
            while (it.hasNext()) {
                holderRefObj = (String) it.next();
                checkURI(holderRefObj, M2MConstants.TAG_M2M_HOLDER_REF, Constants.ID_MODE_REQUIRED);
            }
        }
    }

    public static void getAndCheckPermissionHolderType(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:PermissionHolderType from ong:t_xml_obj
        // {
        // m2m:holderRefs m2m:HolderRefListType { } // (optional)
        // m2m:applicationIDs m2m:ApplicationIDs { } // (optional)
        // m2m:sclIDs m2m:SclIDs { } // (optional)
        // m2m:all m2m:AnonAll { } // (optional)
        // m2m:domains m2m:DomainListType { } // (optional)
        // }
        XoObject permissionHolder = representation.getXoObjectAttribute(tagName);
        checkMode(permissionHolder, tagName, mode);
        if (permissionHolder != null) {
            getAndCheckHolderRefListType(permissionHolder, M2MConstants.TAG_M2M_HOLDER_REFS, Constants.ID_MODE_OPTIONAL);
            getAndCheckApplicationIDs(permissionHolder, M2MConstants.TAG_M2M_APPLICATION_I_DS, Constants.ID_MODE_OPTIONAL);
            getAndCheckSclIDs(permissionHolder, M2MConstants.TAG_M2M_SCL_I_DS, Constants.ID_MODE_OPTIONAL);
            getAndCheckDomainListType(permissionHolder, M2MConstants.TAG_M2M_DOMAINS, Constants.ID_MODE_OPTIONAL);
        }
    }

    public static void getAndCheckPermissionFlagListType(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:PermissionFlagListType from ong:t_xml_obj
        // {
        // m2m:flag XoString { list } // (xmlType: m2m:PermissionFlagType) (list size: [0, infinity[) (enum: READ WRITE DISCOVER
        // DELETE CREATE )
        // }
        XoObject permissionFlagList = representation.getXoObjectAttribute(tagName);
        checkMode(permissionFlagList, tagName, mode);
        if (permissionFlagList != null) {
            List flag = permissionFlagList.getStringListAttribute(M2MConstants.TAG_M2M_FLAG);
            Iterator it = flag.iterator();
            String flagObj = null;
            while (it.hasNext()) {
                flagObj = (String) it.next();
                checkPermissionFlagType(flagObj, M2MConstants.TAG_M2M_FLAG);
            }
        }
    }

    public static void getAndCheckPermissionListType(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:PermissionListType from ong:t_xml_obj
        // {
        // m2m:permission m2m:PermissionType { list wraphidden } // (list size: [0, infinity[)
        // }
        XoObject permissionList = representation.getXoObjectAttribute(tagName);
        checkMode(permissionList, tagName, mode);
        if (permissionList != null) {
            checkPermissionListType(permissionList, tagName);
        }
    }

    public static void getAndCheckContactInfo(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:ContactInfo from ong:t_xml_obj
        // {
        // m2m:contactURI XoString { } // (xmlType: xsd:anyURI)
        // m2m:other XoString { } // (xmlType: xsd:anyType)
        // }
        XoObject contactInfo = representation.getXoObjectAttribute(tagName);
        checkMode(contactInfo, tagName, mode);
        if (contactInfo != null) {
            getAndCheckURI(contactInfo, M2MConstants.TAG_M2M_CONTACT_U_R_I, Constants.ID_MODE_REQUIRED);
            getAndCheckStringMode(contactInfo, M2MConstants.TAG_M2M_OTHER, Constants.ID_MODE_REQUIRED);
        }
    }

    public static int getAndCheckOnlineStatus(XoObject representation, String tagName, int mode) throws M2MException {
        String onlineStatus = representation.getStringAttribute(tagName);
        checkMode(onlineStatus, tagName, mode);
        if (onlineStatus != null) {
            if (M2MConstants.ONLINE_STATUS_ONLINE.equals(onlineStatus)) {
                return Constants.ID_ONLINE_STATUS_ONLINE;
            } else if (M2MConstants.ONLINE_STATUS_OFFLINE.equals(onlineStatus)) {
                return Constants.ID_ONLINE_STATUS_OFFLINE;
            } else if (M2MConstants.ONLINE_STATUS_NOT_REACHABLE.equals(onlineStatus)) {
                return Constants.ID_ONLINE_STATUS_NOT_REACHABLE;
            }
            throw new M2MException("Invalid " + tagName + " value: " + onlineStatus, StatusCode.STATUS_BAD_REQUEST);
        }
        return Constants.ID_ENUM_UNDEFINED_VALUE;
    }

    public static int getAndCheckChannelType(XoObject representation, String tagName, int mode) throws M2MException {
        String channelType = representation.getStringAttribute(tagName);
        checkMode(channelType, tagName, mode);
        if (channelType != null) {
            if (M2MConstants.CHANNEL_TYPE_LONG_POLLING.equals(channelType)) {
                return Constants.ID_CHANNEL_TYPE_LONG_POLLING;
            }
            throw new M2MException("Invalid " + tagName + " value: " + channelType, StatusCode.STATUS_BAD_REQUEST);
        }
        return Constants.ID_ENUM_UNDEFINED_VALUE;
    }

    public static void getAndCheckAnyURIList(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:AnyURIList from ong:t_xml_obj
        // {
        // reference XoString { list } // (xmlType: xsd:anyURI) (list size: [0, infinity[)
        // }
        XoObject anyURIList = representation.getXoObjectAttribute(tagName);
        checkMode(anyURIList, tagName, mode);
        if (anyURIList != null) {
            List anyURI = anyURIList.getStringListAttribute(M2MConstants.TAG_REFERENCE);
            Iterator it = anyURI.iterator();
            String anyURIObj = null;
            while (it.hasNext()) {
                anyURIObj = (String) it.next();
                checkURI(anyURIObj, M2MConstants.TAG_REFERENCE, Constants.ID_MODE_REQUIRED);
            }
        }
    }

    public static int getAndCheckMgmtProtocolType(XoObject representation, String tagName, int mode) throws M2MException {
        String mgmtProtocolType = representation.getStringAttribute(tagName);
        checkMode(mgmtProtocolType, tagName, mode);
        if (mgmtProtocolType != null) {
            if (M2MConstants.MGMT_PROTOCOL_TYPE_OMA_DM.equals(mgmtProtocolType)) {
                return Constants.ID_MGMT_PROTOCOL_TYPE_OMA_DM;
            } else if (M2MConstants.MGMT_PROTOCOL_TYPE_BBF.equals(mgmtProtocolType)) {
                return Constants.ID_MGMT_PROTOCOL_TYPE_BBF;
            } else if (M2MConstants.MGMT_PROTOCOL_TYPE_TR_069.equals(mgmtProtocolType)) {
                return Constants.ID_MGMT_PROTOCOL_TYPE_TR_069;
            }
            throw new M2MException("Invalid " + tagName + " value: " + mgmtProtocolType, StatusCode.STATUS_BAD_REQUEST);
        }
        return Constants.ID_ENUM_UNDEFINED_VALUE;
    }

    public static void getAndCheckIntegrityValResults(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:IntegrityValResults from ong:t_xml_obj
        // {
        // ivalResults XoString { } // (xmlType: xsd:long)
        // signedIvalResult XoString { } // (xmlType: xsd:long)
        // secureTimeStamp XoString { } // (xmlType: xsd:dateTime)
        // }
        XoObject integrityValResults = representation.getXoObjectAttribute(tagName);
        checkMode(integrityValResults, tagName, mode);
        if (integrityValResults != null) {
            getAndCheckLong(representation, M2MConstants.TAG_IVAL_RESULTS, Constants.ID_MODE_REQUIRED);
            getAndCheckLong(representation, M2MConstants.TAG_SIGNED_IVAL_RESULTS, Constants.ID_MODE_REQUIRED);
            getAndCheckDateTime(representation, M2MConstants.TAG_SECURE_TIME_STAMP, Constants.ID_MODE_REQUIRED, -1L);
        }
    }

    public static FilterCriteria getAndCheckFilterCriteriaType(XoObject representation, String tagName, int mode)
            throws M2MException {
        FilterCriteria fc = null;
        XoObject filterCriteria = representation.getXoObjectAttribute(tagName);
        checkMode(filterCriteria, tagName, mode);
        if (filterCriteria != null) {
            fc = new FilterCriteria();
            checkFilterCriteriaType(filterCriteria, tagName, fc);
        }
        return fc;
    }

    public static ContentInstanceFilterCriteria getAndCheckContentInstanceFilterCriteriaType(XoObject representation,
            String tagName, int mode) throws M2MException {
        // m2m:ContentInstanceFilterCriteriaType from m2m:FilterCriteriaType
        // {
        // creator XoString { } // (optional) (xmlType: xsd:anyURI)
        // searchString XoString { list } // (xmlType: xsd:string) (list size: [0, infinity[)
        // createdSince XoString { } // (optional) (xmlType: xsd:dateTime)
        // createdUntil XoString { } // (optional) (xmlType: xsd:dateTime)
        // sizeFrom XoString { } // (optional) (xmlType: xsd:int)
        // sizeUntil XoString { } // (optional) (xmlType: xsd:int)
        // contentType XoString { list } // (xmlType: xsd:string) (list size: [0, infinity[)
        // metaDataOnly XoString { } // (optional) (xmlType: xsd:boolean)
        // attributeAccessor XoString { } // (optional) (xmlType: xsd:anyURI)
        // }
        ContentInstanceFilterCriteria fc = null;
        XoObject contentInstanceFilterCriteria = representation.getXoObjectAttribute(tagName);
        checkMode(contentInstanceFilterCriteria, tagName, mode);
        if (contentInstanceFilterCriteria != null) {
            fc = new ContentInstanceFilterCriteria();
            String xsiType = contentInstanceFilterCriteria.getStringAttribute(M2MConstants.ATTR_XSI_TYPE);
            if (!M2MConstants.TYPE_M2M_CONTENT_INSTANCE_FILTER_CRITERIA_TYPE.equals(xsiType)) {
                throw new M2MException(tagName + " must be of type "
                        + M2MConstants.TYPE_M2M_CONTENT_INSTANCE_FILTER_CRITERIA_TYPE, StatusCode.STATUS_BAD_REQUEST);
            }
            checkFilterCriteriaType(contentInstanceFilterCriteria, tagName, fc);
            // TODO stage 2 and 3 have not the same definition for filter criteria
            // URI creator = getAndCheckURI(contentInstanceFilterCriteria, M2MConstants.TAG_CREATOR,
            // Constants.ID_MODE_OPTIONAL);
            // if (creator != null) {
            // fc.put(M2MConstants.TAG_CREATOR, creator);
            // }
            List listValue = getAndCheckStringList(contentInstanceFilterCriteria, M2MConstants.TAG_SEARCH_STRING);
            String[] arrayValue = null;
            if (listValue != null) {
                arrayValue = new String[listValue.size()];
                listValue.toArray(arrayValue);
                Arrays.sort(arrayValue);
                fc.setSearchString(arrayValue);
            }
            Date dateValue = getAndCheckDateTime(contentInstanceFilterCriteria, M2MConstants.TAG_CREATED_SINCE,
                    Constants.ID_MODE_OPTIONAL, -1L);
            if (dateValue != null) {
                fc.setCreatedAfter(dateValue);
            }
            dateValue = getAndCheckDateTime(contentInstanceFilterCriteria, M2MConstants.TAG_CREATED_UNTIL,
                    Constants.ID_MODE_OPTIONAL, -1L);
            if (dateValue != null) {
                fc.setCreatedBefore(dateValue);
            }
            Integer intValue = getAndCheckInt(contentInstanceFilterCriteria, M2MConstants.TAG_SIZE_FROM,
                    Constants.ID_MODE_OPTIONAL);
            if (intValue != null) {
                fc.setSizeFrom(intValue);
            }
            intValue = getAndCheckInt(contentInstanceFilterCriteria, M2MConstants.TAG_SIZE_UNTIL, Constants.ID_MODE_OPTIONAL);
            if (intValue != null) {
                fc.setSizeUntil(intValue);
            }
            listValue = getAndCheckStringList(contentInstanceFilterCriteria, M2MConstants.TAG_CONTENT_TYPE);
            arrayValue = null;
            if (listValue != null) {
                arrayValue = new String[listValue.size()];
                listValue.toArray(arrayValue);
                Arrays.sort(arrayValue);
                fc.setContentType(arrayValue);
            }
            Boolean metaDataOnly = getAndCheckBoolean(contentInstanceFilterCriteria, M2MConstants.TAG_META_DATA_ONLY,
                    Constants.ID_MODE_OPTIONAL);
            if (metaDataOnly != null) {
                fc.setMetaDataOnly(metaDataOnly);
            }
            String attributeAccessor = getAndCheckRelativePath(contentInstanceFilterCriteria,
                    M2MConstants.TAG_ATTRIBUTE_ACCESSOR, Constants.ID_MODE_OPTIONAL);
            if (attributeAccessor != null) {
                fc.setAttributeAccessor(attributeAccessor);
            }
        }
        return fc;
    }

    public static int getAndCheckAPoCHandling(XoObject representation, String tagName, int mode) throws M2MException {
        String aPoCHandling = representation.getStringAttribute(tagName);
        checkMode(aPoCHandling, tagName, mode);
        if (aPoCHandling != null) {
            checkAPoCHandling(aPoCHandling, tagName);
        }
        return Constants.ID_ENUM_UNDEFINED_VALUE;
    }

    public static URI getAndCheckLongPollingChannelData(XoObject representation, String tagName, int mode) throws M2MException {
        // m2m:LongPollingChannelData from m2m:ChannelData
        // {
        // m2m:longPollingURI XoString { } // (xmlType: xsd:anyURI)
        // }
        XoObject longPollingChannelData = representation.getXoObjectAttribute(tagName);
        checkMode(longPollingChannelData, tagName, mode);
        if (longPollingChannelData != null) {
            String xsiType = longPollingChannelData.getStringAttribute(M2MConstants.ATTR_XSI_TYPE);
            if (!M2MConstants.TYPE_M2M_LONG_POLLING_CHANNEL_DATA.equals(xsiType)) {
                throw new M2MException(tagName + " must be of type " + M2MConstants.TYPE_M2M_LONG_POLLING_CHANNEL_DATA,
                        StatusCode.STATUS_BAD_REQUEST);
            }
            return getAndCheckURI(longPollingChannelData, M2MConstants.TAG_M2M_LONG_POLLING_U_R_I, Constants.ID_MODE_REQUIRED);
        }
        return null;
    }

    public static List getAndCheckStringList(XoObject representation, String tagName) {
        List result = null;
        List listString = representation.getStringListAttribute(tagName);
        if (!listString.isEmpty()) {
            result = new ArrayList();
            Iterator it = listString.iterator();
            while (it.hasNext()) {
                result.add(it.next());
            }
        }
        return result;
    }

    public static void getAndCheckStorageConfiguration(XoObject representation, String tagName, int mode) throws M2MException {
        XoObject storageConfiguration = representation.getXoObjectAttribute(tagName);
        checkMode(storageConfiguration, tagName, mode);
        if (storageConfiguration != null) {
            List paramList = storageConfiguration.getXoObjectListAttribute(M2MConstants.TAG_ACY_PARAM);
            Iterator it = paramList.iterator();
            XoObject param = null;
            while (it.hasNext()) {
                param = (XoObject) it.next();
                getAndCheckStringMode(param, M2MConstants.ATTR_NAME, Constants.ID_MODE_REQUIRED);
                getAndCheckStringMode(param, M2MConstants.ATTR_VAL, Constants.ID_MODE_REQUIRED);
            }
        }
    }
}
