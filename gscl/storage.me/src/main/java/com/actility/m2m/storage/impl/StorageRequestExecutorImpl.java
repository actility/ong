package com.actility.m2m.storage.impl;

import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageFactory;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.log.BundleLogger;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

public class StorageRequestExecutorImpl implements StorageRequestExecutor {
    private static final Logger LOG = OSGiLogger.getLogger(StorageRequestExecutorImpl.class, BundleLogger.LOG);

    private final DriverManager driverManager;
    private final StorageFactoryImpl storageFactory;

    public StorageRequestExecutorImpl(String defaultDriverName, Map/* <String, StorageRequestDriverExecutor> */drivers) {
        this.driverManager = new DriverManager(defaultDriverName, drivers);
        this.storageFactory = new StorageFactoryImpl(driverManager);
    }

    public void addDriver(StorageRequestDriverExecutor driver, String name) {
        if (LOG.isInfoEnabled()) {
            LOG.info("addDriver: " + name);
        }
        driverManager.addDriver(name, driver);
    }

    public void removeDriver(StorageRequestDriverExecutor driver, String name) {
        if (LOG.isInfoEnabled()) {
            LOG.info("removeDriver: " + name);
        }
        driverManager.removeDriver(name);
    }

    public Document retrieve(Map config, String path, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Retrieve path: " + path);
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }
        Document document = driverExecutor.retrieve(config, path, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Retrieve path: " + path);
        }
        return document;
    }

    public Document retrieve(Map config, Object id, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Retrieve path: " + id);
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }
        Document document = driverExecutor.retrieve(config, id, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Retrieve path: " + id);
        }
        return document;
    }

    public boolean update(Map config, Document document, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Update path: " + document.getPath());
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }

        boolean result = driverExecutor.update(config, document, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Update path: " + document.getPath());
        }
        return result;
    }

    public boolean partialUpdate(Map/* <String, String> */config, Document document, byte[] content,
            List/* <AttributeOperation> */attrOps, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Partial update path: " + document.getPath());
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }

        boolean result = driverExecutor.update(config, document, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Partial update path: " + document.getPath());
        }
        return result;
    }

    public boolean create(Map/* <String, String> */config, Document document) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Create path: " + document.getPath());
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }

        boolean result = driverExecutor.create(config, document);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Create path: " + document.getPath());
        }
        return result;
    }

    public boolean delete(Map config, Document document, int cascade, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Delete path: " + document.getPath());
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }

        boolean result = driverExecutor.delete(config, document, cascade, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Delete path: " + document.getPath());
        }
        return result;
    }

    public boolean delete(Map config, Document document, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Delete path: " + document.getPath());
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }

        boolean result = driverExecutor.delete(config, document, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Delete path: " + document.getPath());
        }
        return result;
    }

    public SearchResult search(Map/* <String, String> */config, String path, int scope, Condition condition, int order,
            int limit, boolean withContent, List/* <String> */withAttributes) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + path);
        }
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }

        SearchResult result = driverExecutor.search(config, path, scope, condition, order, limit, withContent, withAttributes);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + path);
        }
        return result;
    }

    public String reserveSpace(Map config, String path, String docNumber, double docSize) throws StorageException {
        StorageRequestDriverExecutor driverExecutor = driverManager.resolveDriver(config);

        if (driverExecutor == null) {
            throw new StorageException("Storage driver not found: "
                    + ((config != null) ? config.get(CONFIG_DRIVER_NAME) : driverManager.getDefaultDriverName()));
        }

        return driverExecutor.reserveSpace(config, path, docNumber, docSize);
    }

    public StorageFactory getStorageFactory() {
        return storageFactory;
    }
}
