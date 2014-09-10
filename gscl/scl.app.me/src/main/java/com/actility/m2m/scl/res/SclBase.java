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
 * id $Id: SclBase.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.res;

import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * M2M Service Capability Layer. This is the root resource which represents an M2M network.
 */
public final class SclBase extends SclResource implements VolatileResource, SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(SclBase.class, BundleLogger.getStaticLogger());

    private static final SclBase INSTANCE = new SclBase();

    public static SclBase getInstance() {
        return INSTANCE;
    }

    private SclBase() {
        // Cannot be instantiated
        super(false, "sclBaseRetrieveRequestIndication", "sclBaseRetrieveResponseConfirm", "sclBaseUpdateRequestIndication",
                "sclBaseUpdateResponseConfirm", null, null, Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, null, false,
                false);
    }

    // m2m:SclBase from ong:t_xml_obj
    // {
    // m2m:accessRightID XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:searchStrings m2m:SearchStrings { } // (optional)
    // m2m:creationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:lastModifiedTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:aPocHandling XoString { } // (optional) (xmlType: m2m:APocHandling) (enum: SHALLOW DEEP )
    // m2m:sclsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:applicationsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:containersReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:groupsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:accessRightsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:subscriptionsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:discoveryReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // }
    // alias m2m:SclBase with m2m:sclBase

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws ParseException,
            StorageException, XoException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <sclBase>: " + path);
        }
        resource.setStringAttribute(M2MConstants.TAG_M2M_A_POC_HANDLING, null);

        Collection searchAttributes = new ArrayList();
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        Iterator it = searchStringList.iterator();
        while (it.hasNext()) {
            searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
        }
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME))));
        transaction.updateResource(path, resource, searchAttributes);
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        getAndCheckAccessRight(logId, manager, path, resource, requestingEntity, flag);
    }

    public void createResource(SclManager manager, String path, SclTransaction transaction) throws XoException {
        XoObject resource = null;
        try {
            resource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SCL_BASE);
            Date now = new Date();
            String creationTime = FormatUtils.formatDateTime(now, manager.getTimeZone());

            // Update resource
            resource.setNameSpace(M2MConstants.PREFIX_M2M);
            resource.setXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS,
                    manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SEARCH_STRINGS));
            resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

            // Create sub-resources
            Scls.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SCLS, now, creationTime,
                    null, transaction);
            Applications.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS,
                    now, creationTime, null, transaction);
            Containers.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_CONTAINERS, now,
                    creationTime, null, transaction);
            AccessRights.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_ACCESS_RIGHTS,
                    now, creationTime, null, transaction);
            Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                    transaction);

            // Save resource
            Collection searchAttributes = new ArrayList();
            XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
            List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            Iterator it = searchStringList.iterator();
            while (it.hasNext()) {
                searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
            }
            searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, now));
            searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
            transaction.createResource(path, resource, searchAttributes);
        } finally {
            if (resource != null) {
                resource.free(true);
            }
        }
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // aPoCHandling: O (response O)
        // sclsReference: NP (response M)
        // applicationsReference: NP (response M)
        // containersReference: NP (response M)
        // groupsReference: NP (response M)
        // accessRightsReference: NP (response M)
        // subscriptionsReference: NP (response M)
        // discoveryReference; NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_SCL_BASE);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SCLS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_APPLICATIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_GROUPS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_DISCOVERY_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                indication.getTargetPath());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        // TODO do only if the indication requesting entity is registered to NSCL
        getAndCheckAPoCHandling(representation, M2MConstants.TAG_M2M_A_POC_HANDLING, Constants.ID_MODE_OPTIONAL);

        // Update resource
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        updateLastModifiedTime(manager, resource, now);
        // TODO do only if the indication requesting entity is registered to NSCL
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_A_POC_HANDLING);

        // Save resource
        Collection searchAttributes = new ArrayList();
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

        return false;
    }

    public void updateAPoCHandling(SclManager manager, String path, XoObject resource, String aPoCHandling)
            throws ParseException, StorageException, XoException, M2MException {
        String currentAPoCHandling = resource.getStringAttribute(M2MConstants.TAG_M2M_A_POC_HANDLING);
        boolean modified = false;
        if (aPoCHandling != null) {
            int aPoCHandlingId = checkAPoCHandling(aPoCHandling, M2MConstants.TAG_M2M_A_POC_HANDLING);
            if (aPoCHandlingId != manager.getSclConfig().getAPoCHandling()) {
                manager.getSclConfig().setAPoCHandling(aPoCHandlingId);
                resource.setStringAttribute(M2MConstants.TAG_M2M_A_POC_HANDLING, aPoCHandling);
                modified = true;
            }
        } else if (currentAPoCHandling != null) {
            manager.getSclConfig().setAPoCHandling(Constants.ID_APOC_HANDLING_SHALLOW);
            resource.setStringAttribute(M2MConstants.TAG_M2M_A_POC_HANDLING, null);
            modified = true;
        }
        if (modified) {
            Date now = new Date();
            updateLastModifiedTime(manager, resource, now);
            Collection searchAttributes = new ArrayList();
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
        }
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(SclManager manager, String path, XoObject resource, FilterCriteria filterCriteria,
            Set supported) {
        String appPath = manager.getM2MContext().getApplicationPath();
        String accessRight = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRight != null) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + URIUtils.encodePath(accessRight)
                    + M2MConstants.URI_SEP);
        }
        String encodedPath = URIUtils.encodePath(path);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SCLS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SCLS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_APPLICATIONS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_APPLICATIONS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_CONTAINERS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_GROUPS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_GROUPS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_ACCESS_RIGHTS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_DISCOVERY_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_DISCOVERY + M2MConstants.URI_SEP);
    }

    /**
     * Creates the Service Capability Layer representation of the current Service Capability Layer.
     * <p>
     * This representation is used to register the Service Capability Layer to a remote Service Capability Layer.
     *
     * @return The Remote Service Capability Layer representation of the current Service Capability Layer
     */
    public XoObject createSclRepresentation(SclManager manager, XoObject resource) throws XoException {
        // TODO this could leak
        XoObject scl = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SCL);
        scl.setNameSpace(M2MConstants.PREFIX_M2M);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchString = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        XoObject sclSearchStrings = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        scl.setXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS, sclSearchStrings);
        List sclSearchString = sclSearchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        sclSearchString.addAll(searchString);
        scl.setStringAttribute(M2MConstants.TAG_M2M_A_POC_HANDLING,
                resource.getStringAttribute(M2MConstants.TAG_M2M_A_POC_HANDLING));
        return scl;
    }
}
