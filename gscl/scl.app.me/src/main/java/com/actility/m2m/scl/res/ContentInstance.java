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
 * id $Id: ContentInstance.java 4306 2013-02-18 10:22:20Z JReich $
 * author $Author: JReich $
 * version $Revision: 4306 $
 * lastrevision $Date: 2013-02-18 11:22:20 +0100 (Mon, 18 Feb 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-02-18 11:22:20 +0100 (Mon, 18 Feb 2013) $
 */

package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.ContentInstanceFilterCriteria;
import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.Response;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.PartialAccessor;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * M2M Content Instance. Defines a place to store a value like a temperature measurement, a device state, an applicative
 * value...
 *
 * <pre>
 * m2m:ContentInstance from ong:t_xml_obj
 * {
 *     m2m:id    XoString    { embattr } // (optional) (xmlType: xsd:NMTOKEN)
 *     href    XoString    { embattr } // (optional) (xmlType: xsd:anyURI)
 *     m2m:creationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:lastModifiedTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:delayTolerance    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:searchStrings    m2m:SearchStrings    { } // (optional)
 *     m2m:contentTypes    m2m:ContentTypes    { } // (optional)
 *     m2m:contentSize    XoString    { } // (optional) (xmlType: xsd:long)
 *     m2m:content    m2m:Content    { } // (optional)
 * }
 * alias m2m:ContentInstance with m2m:contentInstance
 * </pre>
 */
public final class ContentInstance extends SclResource {
    private static final Logger LOG = OSGiLogger.getLogger(ContentInstance.class, BundleLogger.getStaticLogger());

    private static final ContentInstance INSTANCE = new ContentInstance();

    public static ContentInstance getInstance() {
        return INSTANCE;
    }

    private ContentInstance() {
        // Cannot be instantiated
        super(false, "contentInstanceRetrieveRequestIndication", "contentInstanceRetrieveResponseConfirm", null, null,
                "contentInstanceDeleteRequestIndication", "contentInstanceDeleteResponseConfirm",
                Constants.ID_NO_FILTER_CRITERIA_MODE, true, false, ContentInstances.getInstance(), false, false);
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        String containerPath = getGrandParentPath(path);
        XoObject container = null;
        try {
            container = manager.getXoResource(containerPath);
            Container.getInstance().checkRights(logId, manager, containerPath, container, requestingEntity, flag);
        } finally {
            if (container != null) {
                container.free(true);
            }
        }
    }

    /**
     * <pre>
     * id:                0  (response M*)
     * href:              NP (response O)
     * contentTypes:      NP (response M)
     * contentSize:       NP (response M)
     * creationTime:      NP (response M)
     * lastModifiedTime:  NP (response M)
     * delayTolerance:    O  (response O)
     * searchStrings:     O  (response O)
     * content:           M  (response M)
     * </pre>
     */
    public boolean createResource(SclManager manager, String path, XoObject resource, Map config, String id, String href,
            Date creationDate, String creationTime, XoObject representation, SclTransaction transaction) throws XoException,
            M2MException {
        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_CONTENT_INSTANCE);
        getAndCheckStringMode(representation, M2MConstants.ATTR_HREF, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTENT_TYPES, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTENT_SIZE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckDateTime(representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE, Constants.ID_MODE_OPTIONAL, -1L);
        int contentSize = getAndCheckContent(representation, M2MConstants.TAG_M2M_CONTENT, Constants.ID_MODE_REQUIRED);
        if (contentSize <= 0) {
            throw new M2MException(M2MConstants.TAG_M2M_CONTENT + " cannot be empty", StatusCode.STATUS_BAD_REQUEST);
        }

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setNameSpace(M2MConstants.PREFIX_XMIME);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        resource.setStringAttribute(M2MConstants.ATTR_HREF, href);
        XoObject contentTypes = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_TYPES);
        resource.setXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT_TYPES, contentTypes);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE, String.valueOf(contentSize));
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE);
        createXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_CONTENT);

        // Compress oBIX content if any
        XoObject contentElem = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT);
        String contentType = contentElem.getStringAttribute(M2MConstants.TAG_XMIME_CONTENT_TYPE);
        contentTypes.getStringListAttribute(M2MConstants.TAG_M2M_CONTENT_TYPE).add(contentType);
        if (contentElem.containsAttribute(M2MConstants.TAG_M2M_TEXT_CONTENT)) {
            contentElem.setBufferAttribute(M2MConstants.TAG_M2M_BINARY_CONTENT,
                    contentElem.getStringAttribute(M2MConstants.TAG_M2M_TEXT_CONTENT).getBytes());
            contentElem.setStringAttribute(M2MConstants.TAG_M2M_TEXT_CONTENT, null);
        }
        if (contentType.startsWith(M2MConstants.MT_APPLICATION_XML_OBIX) && manager.getXoService().isXmlSupported()) {
            contentElem.xoBufferXmlToBinary(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        } else if (contentType.startsWith(M2MConstants.MT_APPLICATION_EXI_OBIX) && manager.getXoService().isExiSupported()) {
            contentElem.xoBufferExiToBinary(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        }

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCE);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        if (searchStrings != null) {
            List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        }
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
        document.setContent(resource.saveBinary());

        transaction.createResource(config, document);

        return false;
    }

    /**
     * <pre>
     * id:                (response M*)
     * href:              (response O)
     * contentTypes:      (response M)
     * contentSize:       (response M)
     * creationTime:      (response M)
     * lastModifiedTime:  (response M)
     * delayTolerance:    (response O)
     * searchStrings:     (response O)
     * content:           (response M)
     * </pre>
     */
    public void createResourceFromBytes(SclManager manager, String path, XoObject resource, Map config, String id, String href,
            Date creationDate, String creationTime, String contentType, String characterEncoding, byte[] content,
            SclTransaction transaction) throws M2MException, XoException {
        // Check representation
        if (content.length == 0) {
            throw new M2MException("<contentInstance> content cannot be empty", StatusCode.STATUS_BAD_REQUEST);
        }
        if (contentType == null) {
            throw new M2MException("<contentInstance> must thave a content-type", StatusCode.STATUS_BAD_REQUEST);
        }

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setNameSpace(M2MConstants.PREFIX_XMIME);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        resource.setStringAttribute(M2MConstants.ATTR_HREF, href);
        XoObject contentTypes = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_TYPES);
        resource.setXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT_TYPES, contentTypes);
        contentTypes.getStringListAttribute(M2MConstants.TAG_M2M_CONTENT_TYPE).add(contentType);

        int semiColonIndex = contentType.indexOf(';');
        String mediaTypeStr = contentType;
        if (semiColonIndex != -1) {
            mediaTypeStr = contentType.substring(0, semiColonIndex);
        }
        if (mediaTypeStr.equals("text/plain")
                || mediaTypeStr.equals("application/xml")
                || mediaTypeStr.equals("application/json")
                || (mediaTypeStr.startsWith("application/vnd") && (mediaTypeStr.endsWith("+xml") || mediaTypeStr
                        .endsWith("+json")))) {
            // textContent
            try {
                content = new String(content, characterEncoding).getBytes();
            } catch (UnsupportedEncodingException e) {
                throw new M2MException("Cannot build a string from raw content and encoding " + characterEncoding,
                        StatusCode.STATUS_BAD_REQUEST);
            }
        }
        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE, String.valueOf(content.length));
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

        XoObject contentElem = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT);
        resource.setXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT, contentElem);
        contentElem.setStringAttribute(M2MConstants.TAG_XMIME_CONTENT_TYPE, contentType);
        contentElem.setBufferAttribute(M2MConstants.TAG_M2M_BINARY_CONTENT, content);

        // Compress oBIX content if any
        if (contentType.startsWith(M2MConstants.MT_APPLICATION_XML_OBIX) && manager.getXoService().isXmlSupported()) {
            contentElem.xoBufferXmlToBinary(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        } else if (contentType.startsWith(M2MConstants.MT_APPLICATION_EXI_OBIX) && manager.getXoService().isXmlSupported()
                && manager.getXoService().isExiSupported()) {
            contentElem.xoBufferExiToBinary(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        }

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCE);
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
        document.setContent(resource.saveBinary());
        transaction.createResource(config, document);
    }

    /**
     * <pre>
     * id:                M  (response M*)
     * href:              O  (response O)
     * contentTypes:      M  (response M)
     * contentSize:       M  (response M)
     * creationTime:      M  (response M)
     * lastModifiedTime:  M  (response M)
     * delayTolerance:    O  (response O)
     * searchStrings:     O  (response O)
     * content:           M  (response M)
     * </pre>
     */
    public boolean createResourceFromExternalContentInstance(SclManager manager, String path, XoObject resource, Map config,
            String id, String href, XoObject representation, SclTransaction transaction) throws XoException, M2MException {
        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_CONTENT_INSTANCE);
        getAndCheckStringMode(representation, M2MConstants.ATTR_HREF, Constants.ID_MODE_OPTIONAL);
        // TODO contentTypes list
        getAndCheckLong(representation, M2MConstants.TAG_M2M_CONTENT_SIZE, Constants.ID_MODE_REQUIRED);
        Date creationDate = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_REQUIRED,
                -1L);
        getAndCheckDateTime(representation, M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, Constants.ID_MODE_REQUIRED, -1L);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_M2M_SEARCH_STRINGS, Constants.ID_MODE_OPTIONAL);
        getAndCheckDateTime(representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE, Constants.ID_MODE_OPTIONAL, -1L);
        int contentSize = getAndCheckContent(representation, M2MConstants.TAG_M2M_CONTENT, Constants.ID_MODE_REQUIRED);
        if (contentSize <= 0) {
            throw new M2MException(M2MConstants.TAG_M2M_CONTENT + " cannot be empty", StatusCode.STATUS_BAD_REQUEST);
        }

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setNameSpace(M2MConstants.PREFIX_XMIME);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        resource.setStringAttribute(M2MConstants.ATTR_HREF, href);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_CONTENT_TYPES);
        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_CONTENT_SIZE, null);
        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_CREATION_TIME, null);
        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, null);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE);
        createXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_CONTENT);

        // Compress oBIX content if any
        XoObject contentElem = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT);
        String contentType = contentElem.getStringAttribute(M2MConstants.TAG_XMIME_CONTENT_TYPE);
        if (contentElem.containsAttribute(M2MConstants.TAG_M2M_TEXT_CONTENT)) {
            contentElem.setBufferAttribute(M2MConstants.TAG_M2M_BINARY_CONTENT,
                    contentElem.getStringAttribute(M2MConstants.TAG_M2M_TEXT_CONTENT).getBytes());
            contentElem.setStringAttribute(M2MConstants.TAG_M2M_TEXT_CONTENT, null);
        }
        if (contentType.startsWith(M2MConstants.MT_APPLICATION_XML_OBIX) && manager.getXoService().isXmlSupported()) {
            contentElem.xoBufferXmlToBinary(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        } else if (contentType.startsWith(M2MConstants.MT_APPLICATION_EXI_OBIX) && manager.getXoService().isXmlSupported()
                && manager.getXoService().isExiSupported()) {
            contentElem.xoBufferExiToBinary(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        }

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCE);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        if (searchStrings != null) {
            List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        }
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
        document.setContent(resource.saveBinary());
        transaction.createResource(config, document);

        return false;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) {
        throw new UnsupportedOperationException();
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <contentInstance> resource on path: " + path);
        }
        XoObject grandParentResource = null;
        try {
            grandParentResource = manager.getXoResource(getGrandParentPath(path));
            Map storageConfig = createStorageConfiguration(grandParentResource, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);
            if (storageConfig != null) {
                transaction.deleteResource(storageConfig, path);
            } else {
                transaction.deleteResource(path);
            }
        } finally {
            if (grandParentResource != null) {
                grandParentResource.free(true);
            }
        }
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws StorageException, XoException, M2MException {
        deleteResource(logId, manager, path, transaction);
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(String logId, SclManager manager, URI requestingEntity, String path,
            XoObject resource, FilterCriteria filterCriteria, Set supported) throws XoException {
        // TODO prepare according to contentType
        ContentInstanceFilterCriteria cifc = (ContentInstanceFilterCriteria) filterCriteria;
        XoObject content = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT);
        String contentType = content.getStringAttribute(M2MConstants.TAG_XMIME_CONTENT_TYPE);
        if (cifc != null && cifc.getMetaDataOnly() != null && cifc.getMetaDataOnly() == Boolean.TRUE) {
            content.setBufferAttribute(M2MConstants.TAG_M2M_BINARY_CONTENT, null);
        } else {
            if (contentType.startsWith(M2MConstants.MT_APPLICATION_XML_OBIX) && manager.getXoService().isXmlSupported()) {
                content.xoBufferBinaryToXml(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
            } else if (contentType.startsWith(M2MConstants.MT_APPLICATION_EXI_OBIX) && manager.getXoService().isXmlSupported()
                    && manager.getXoService().isExiSupported()) {
                content.xoBufferBinaryToExi(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
            }
            int semiColonIndex = contentType.indexOf(';');
            String mediaTypeStr = contentType;
            if (semiColonIndex != -1) {
                mediaTypeStr = contentType.substring(0, semiColonIndex);
            }
            if (mediaTypeStr.equals("text/plain")
                    || mediaTypeStr.equals("application/xml")
                    || mediaTypeStr.equals("application/json")
                    || (mediaTypeStr.startsWith("application/vnd") && (mediaTypeStr.endsWith("+xml") || mediaTypeStr
                            .endsWith("+json")))) {
                // textContent
                content.setStringAttribute(M2MConstants.TAG_M2M_TEXT_CONTENT,
                        new String(content.getBufferAttribute(M2MConstants.TAG_M2M_BINARY_CONTENT)));
                content.setBufferAttribute(M2MConstants.TAG_M2M_BINARY_CONTENT, null);
            }
        }
    }

    public boolean filterMatches(XoObject resource, FilterCriteria filterCriteria) {
        ContentInstanceFilterCriteria cift = (ContentInstanceFilterCriteria) filterCriteria;
        boolean matches = true;
        if (cift != null) {
            matches = filterMatchesOnLastModifiedTime(resource, filterCriteria)
                    && filterMatchesOnCreationTime(resource, filterCriteria) && filterMatchesOnContentSize(resource, cift)
                    && filterMatchesOnContentType(resource, cift);
        }
        return matches;
    }

    public boolean filterMatchesOnContentSize(XoObject resource, ContentInstanceFilterCriteria cift) {
        String attrValue = resource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE);
        int intValue = -1;
        if (attrValue != null) {
            try {
                intValue = Integer.parseInt(attrValue);
                if (cift.getSizeFrom() != null && cift.getSizeFrom().intValue() > intValue) {
                    return false;
                }
                if (cift.getSizeUntil() != null && cift.getSizeUntil().intValue() < intValue) {
                    return false;
                }
            } catch (NumberFormatException e) {
                LOG.error("Cannot parse " + M2MConstants.TAG_M2M_CONTENT_SIZE + " '" + attrValue + "' of resource", e);
                return false;
            }
        }
        return true;
    }

    public boolean filterMatchesOnContentType(XoObject resource, ContentInstanceFilterCriteria cift) {
        XoObject contentTypes = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT_TYPES);
        if (contentTypes != null) {
            List contentTypesList = contentTypes.getStringListAttribute(M2MConstants.TAG_M2M_CONTENT_TYPE);
            String[] contentType = cift.getContentType();
            if (contentType != null) {
                int i = 0;
                for (i = 0; i < contentType.length; ++i) {
                    if (contentTypesList.contains(contentType[i])) {
                        break;
                    }
                }
                if (i == contentType.length) {
                    return false;
                }
            }
        }
        return true;
    }

    public void sendRawContent(SclManager manager, String path, XoObject resource, Indication indication) throws IOException,
            M2MException, XoException {
        XoObject contentBase64 = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT);
        String contentType = contentBase64.getStringAttribute(M2MConstants.TAG_XMIME_CONTENT_TYPE);
        byte[] rawBytes = null;

        if (contentType.startsWith(M2MConstants.MT_APPLICATION_XML_OBIX) && manager.getXoService().isXmlSupported()) {
            contentBase64.xoBufferBinaryToXml(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        } else if (contentType.startsWith(M2MConstants.MT_APPLICATION_EXI_OBIX) && manager.getXoService().isXmlSupported()
                && manager.getXoService().isExiSupported()) {
            contentBase64.xoBufferBinaryToExi(M2MConstants.TAG_M2M_BINARY_CONTENT, true);
        }
        rawBytes = contentBase64.getBufferAttribute(M2MConstants.TAG_M2M_BINARY_CONTENT);
        String lastModifiedTimeStr = resource.getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME);
        URI resourceURI = manager.getM2MContext().createLocalUri(indication.getTargetID(), path + "/content");
        Response response = indication.createSuccessResponse(StatusCode.STATUS_OK);
        response.setResouceURI(resourceURI);
        if (lastModifiedTimeStr != null) {
            response.setETag(lastModifiedTimeStr);
            try {
                Date lastModifiedTime = FormatUtils.parseDateTime(lastModifiedTimeStr);
                response.setLastModified(lastModifiedTime.getTime());
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Cannot parse " + M2MConstants.TAG_M2M_LAST_MODIFIED_TIME + " '"
                        + lastModifiedTimeStr + "' of resource on path '" + path + "'", e);
            }
        }
        response.setRawContent(rawBytes, contentType);
        SclLogger.logResponseConfirm(response, StatusCode.STATUS_OK, rawBytes, contentType);
        response.send();
    }

    public void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        if (partialAccessor.getAttributeId() == Constants.ID_ATTR_CONTENT && partialAccessor.getSubPath() == null) {
            SclLogger.logRequestIndication("contentInstanceRetrieveRequestIndication",
                    "contentInstanceRetrieveResponseConfirm", indication, null, 0);

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

            sendRawContent(manager, path, resource, indication);
        } else {
            super.doRetrieveIndication(manager, path, resource, indication, partialAccessor);
        }
    }

    public int appendDiscoveryURIs(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            URI targetID, String appPath, String[] searchStrings, List discoveryURIs, int remainingURIs) throws IOException,
            StorageException, XoException {
        int urisCount = remainingURIs;
        try {
            checkRights(logId, manager, path, resource, requestingEntity, M2MConstants.FLAG_DISCOVER);
            if (urisCount > 0) {
                discoveryURIs.add(appPath + URIUtils.encodePath(path));
            }
            --urisCount;
        } catch (M2MException e) {
            // Right is not granted
        }
        return urisCount;
    }
}
