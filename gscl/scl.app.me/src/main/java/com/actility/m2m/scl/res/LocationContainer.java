package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.ExpirationTimerDeleteOp;
import com.actility.m2m.scl.mem.ExpirationTimerUpdateOp;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * M2M Location Container. Placeholder to store Collections of Content Instances.
 * <p>
 * This resource can be subscribed.
 *
 * <pre>
 * m2m:LocationContainer from m2m:Container
 * {
 *     m2m:locationContainerType    XoString    { shdico } // (xmlType: m2m:LocationContainerType) (enum: LOCATION_SERVER_BASED APPLICATION_GENERATED )
 * }
 * alias m2m:LocationContainer with m2m:locationContainer
 * </pre>
 */
public final class LocationContainer extends SclResource implements VolatileResource, SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(Container.class, BundleLogger.getStaticLogger());

    private static final LocationContainer INSTANCE = new LocationContainer();

    public static LocationContainer getInstance() {
        return INSTANCE;
    }

    private LocationContainer() {
        // Cannot be instantiated
        super(false, "locationContainerRetrieveRequestIndication", "locationContainerRetrieveResponseConfirm",
                "locationContainerUpdateRequestIndication", "locationContainerUpdateResponseConfirm",
                "locationContainerDeleteRequestIndication", "locationContainerDeleteResponseConfirm",
                Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, Containers.getInstance(), true, true);
    }

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws IOException,
            M2MException, StorageException, XoException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <locationContainer>: " + path);
        }
        long now = System.currentTimeMillis();
        Date expirationTime = getAndCheckDateTime(resource, M2MConstants.TAG_M2M_EXPIRATION_TIME, Constants.ID_MODE_OPTIONAL,
                -1L);
        if (expirationTime != null && expirationTime.getTime() > 0) {
            if (expirationTime.getTime() > now) {
                if (expirationTime.getTime() < FormatUtils.MAX_DATE) {
                    transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_LOCATION_CONTAINER,
                            expirationTime.getTime() - now));
                }
                // else infinite timer
            } else {
                // delete resource, it is already expired
                deleteResource("restore", manager, path, transaction);
            }
        }
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        getAndCheckAccessRight(logId, manager, path, resource, requestingEntity, flag);
    }

    public boolean createResource(SclManager manager, String path, XoObject resource, String targetPath, String id,
            Date creationDate, String creationTime, XoObject representation, SclTransaction transaction) throws XoException,
            M2MException {
        boolean modified = false;
        // id: O (response M*)
        // expirationTime: O (response M*)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // maxNrOfInstances: O (response M*)
        // maxByteSize: O (response M*)
        // maxInstanceAge: O (response M*)
        // locationContainerType: M (response M)
        // storageConfiguration: O (response M)
        // contentInstancesReference: NP (response M)
        // subscriptionsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_LOCATION_CONTAINER);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, creationDate.getTime());
        String accessRightId = getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D,
                Constants.ID_MODE_OPTIONAL, targetPath);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        long maxNrOfInstances = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                Constants.ID_MODE_OPTIONAL);
        long maxByteSize = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_BYTE_SIZE, Constants.ID_MODE_OPTIONAL);
        long maxInstanceAge = getAndCheckDuration(representation, M2MConstants.TAG_M2M_MAX_INSTANCE_AGE,
                Constants.ID_MODE_OPTIONAL);
        int locationContainerType = getAndCheckLocationContainerType(representation,
                M2MConstants.TAG_M2M_LOCATION_CONTAINER_TYPE, Constants.ID_MODE_REQUIRED);
        getAndCheckStorageConfiguration(representation, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTENT_INSTANCES_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        Date expirationTime = setExpirationTime(manager, resource, creationDate.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
        if (representation.getXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO) != null) {
            modified = true;
        }
        resource.setXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO,
                manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ANNOUNCE_TO));
        modified = setMaxNrOfInstances(manager, resource, maxNrOfInstances) || modified;
        modified = setMaxByteSize(manager, resource, maxByteSize) || modified;
        modified = setMaxInstanceAge(manager, resource, maxInstanceAge) || modified;
        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_LOCATION_CONTAINER_TYPE, null);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);

        String grandParentPath = getGrandParentPath(path);
        if (manager.getDefaultResourceId(grandParentPath) != Constants.ID_RES_APPLICATION) {
            throw new M2MException("Cannot create a <locationContainer> under a non <application> resource",
                    StatusCode.STATUS_FORBIDDEN);
        }
        if (locationContainerType == Constants.ID_LOCATION_CONTAINER_TYPE_LOCATION_SERVER_BASED) {
            throw new M2MException("Location server based <locationContainer> is only allowed on NSCL",
                    StatusCode.STATUS_FORBIDDEN);
        }

        // Create sub-resources
        ContentInstances.getInstance().createResource(manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_CONTENT_INSTANCES, creationDate, creationTime, transaction);
        Subcontainers.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBCONTAINERS,
                creationDate, creationTime, accessRightId, transaction);
        Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                transaction);

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_LOCATION_CONTAINER);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
        document.setContent(resource.saveBinary());
        transaction.createResource(document);

        transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_CONTAINER, expirationTime
                .getTime() - creationDate.getTime()));

        return modified;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        boolean modified = false;
        // id: NP (response M*)
        // expirationTime: O (response M*)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // maxNrOfInstances: O (response M*)
        // maxByteSize: O (response M*)
        // maxInstanceAge: O (response M*)
        // locationContainerType: NP (response M)
        // storageConfiguration: NP (response M)
        // contentInstancesReference: NP (response M)
        // subscriptionsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_LOCATION_CONTAINER);
        getAndCheckStringMode(representation, M2MConstants.ATTR_M2M_ID, Constants.ID_MODE_FORBIDDEN);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, now.getTime());
        getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                indication.getTargetPath());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        long maxNrOfInstances = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                Constants.ID_MODE_OPTIONAL);
        long maxByteSize = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_BYTE_SIZE, Constants.ID_MODE_OPTIONAL);
        long maxInstanceAge = getAndCheckDuration(representation, M2MConstants.TAG_M2M_MAX_INSTANCE_AGE,
                Constants.ID_MODE_OPTIONAL);
        getAndCheckLocationContainerType(representation, M2MConstants.TAG_M2M_LOCATION_CONTAINER_TYPE,
                Constants.ID_MODE_FORBIDDEN);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTENT_INSTANCES_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        Date expirationTime = setExpirationTime(manager, resource, now.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        updateLastModifiedTime(manager, resource, now);
        if (representation.getXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO) != null) {
            modified = true;
        }
        modified = setMaxNrOfInstances(manager, resource, maxNrOfInstances) || modified;
        modified = setMaxByteSize(manager, resource, maxByteSize) || modified;
        modified = setMaxInstanceAge(manager, resource, maxInstanceAge) || modified;

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_LOCATION_CONTAINER);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
        document.setContent(resource.saveBinary());
        manager.getStorageContext().update(null, document, null);

        new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_CONTAINER, expirationTime.getTime() - now.getTime())
                .postCommit();

        return modified;
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <container> resource on path: " + path);
        }
        ContentInstances.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_CONTENT_INSTANCES, transaction);
        Subcontainers.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBCONTAINERS, transaction);
        Subscriptions.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS, transaction);

        transaction.deleteResource(path);
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws StorageException, XoException, M2MException {
        deleteResource(logId, manager, path, transaction);
        transaction.addTransientOp(new ExpirationTimerDeleteOp(manager, path));
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(String logId, SclManager manager, URI requestingEntity, String path,
            XoObject resource, FilterCriteria filterCriteria, Set supported) {
        String appPath = manager.getM2MContext().getApplicationPath();
        String accessRight = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRight != null) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + URIUtils.encodePath(accessRight)
                    + M2MConstants.URI_SEP);
        }
        String encodedPath = URIUtils.encodePath(path);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCES_REFERENCE, appPath + encodedPath
                + M2MConstants.URI_SEP + M2MConstants.RES_CONTENT_INSTANCES + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBCONTAINERS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SUBCONTAINERS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
        if (supported == null || !supported.contains(Constants.SP_STORAGE_CONF)) {
            resource.getXoObjectAttribute(M2MConstants.TAG_ACY_STORAGE_CONFIGURATION).free(true);
            resource.setXoObjectAttribute(M2MConstants.TAG_ACY_STORAGE_CONFIGURATION, null);
        }
    }

    public void timeout(SclManager manager, String path, String timerId, M2MSession session, Serializable info)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        SclTransaction transaction = new SclTransaction(manager.getStorageContext());
        deleteResource("expiration", manager, path, transaction);
        transaction.addTransientOp(new ExpirationTimerDeleteOp(manager, path));
        transaction.execute();
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
