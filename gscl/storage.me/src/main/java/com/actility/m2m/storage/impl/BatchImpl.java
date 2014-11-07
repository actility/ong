package com.actility.m2m.storage.impl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import com.actility.m2m.storage.Batch;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.driver.Transaction;
import com.actility.m2m.storage.log.BundleLogger;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

public final class BatchImpl implements Batch {
    private static final Logger LOG = OSGiLogger.getLogger(BatchImpl.class, BundleLogger.LOG);

    private final DriverManager driverManager;
    private final List/* <Modification> */modifications;

    public BatchImpl(DriverManager driverManager) {
        this.driverManager = driverManager;
        this.modifications = new ArrayList/* <Modification> */();
    }

    public void update(Map config, Document document, Condition condition) {
        modifications.add(new Operation(Operation.TYPE_UPDATE, config, document, condition));
    }

    public void partialUpdate(Map config, Document document, byte[] content, List attrOps, Condition condition) {
        modifications.add(new PartialUpdate(config, document, content, attrOps, condition));
    }

    public void create(Map config, Document document) {
        modifications.add(new Operation(Operation.TYPE_CREATE, config, document, null));
    }

    public void delete(Map config, Document document, Condition condition) {
        modifications.add(new Operation(Operation.TYPE_DELETE, config, document, condition));
    }

    public int execute() throws StorageException {
        Profiler.getInstance().trace(">>> Batch modify");
        Map/* <String, Transaction> */transactions = new HashMap/* <String, Transaction> */();
        int result = -1;

        try {
            Iterator it = modifications.iterator();
            while (it.hasNext()) {
                Modification modification = (Modification) it.next();
                Map config = modification.getConfiguration();
                String driverName = driverManager.getDriverName(config);

                Transaction transaction = (Transaction) transactions.get(driverName);

                if (transaction == null) {
                    StorageRequestDriverExecutor driver = driverManager.resolveDriver(driverName);
                    transaction = driver.beginTransaction();
                    transactions.put(driverName, transaction);
                }

                if (modification.getClass() == Operation.class) {
                    Operation operation = (Operation) modification;

                    switch (operation.getType()) {
                    case Operation.TYPE_CREATE:
                        if (Profiler.getInstance().isTraceEnabled()) {
                            Profiler.getInstance().trace("Batch Create path: " + operation.getDocument().getPath());
                        }
                        if (!transaction.create(config, operation.getDocument())) {
                            result = BATCH_CREATE_FAILED;
                            break;
                        }
                        break;
                    case Operation.TYPE_UPDATE:
                        if (Profiler.getInstance().isTraceEnabled()) {
                            Profiler.getInstance().trace("Batch Update path: " + operation.getDocument().getPath());
                        }
                        if (!transaction.update(config, operation.getDocument(), operation.getCondition())) {
                            result = BATCH_UPDATE_FAILED;
                            break;
                        }
                        break;
                    case Operation.TYPE_DELETE:
                        if (Profiler.getInstance().isTraceEnabled()) {
                            Profiler.getInstance().trace("Batch Delete path: " + operation.getDocument().getPath());
                        }
                        if (!transaction.delete(config, operation.getDocument(), operation.getCondition())) {
                            result = BATCH_DELETE_FAILED;
                            break;
                        }
                        break;
                    default:
                        throw new StorageException("Unexpected value in Operation.type: " + operation.getType());
                    }
                } else {
                    PartialUpdate partialUpdate = (PartialUpdate) modification;

                    if (!transaction.partialUpdate(config, partialUpdate.getDocument(), partialUpdate.getContent(),
                            partialUpdate.getAttrOps(), partialUpdate.getCondition())) {
                        result = BATCH_UPDATE_FAILED;
                        break;
                    }
                }
            }

            if (result == -1) {
                it = transactions.values().iterator();
                while (it.hasNext()) {
                    Transaction transaction = (Transaction) it.next();
                    if (result == -1) {
                        result = transaction.prepareTransaction();
                        if (result == BATCH_OK) {
                            result = -1;
                        }
                    }
                }
            }
            if (result == -1) {
                it = transactions.values().iterator();
                while (it.hasNext()) {
                    Transaction transaction = (Transaction) it.next();
                    transaction.commitTransaction();
                }
                result = BATCH_OK;
            }
        } finally {
            if (result != BATCH_OK) {
                Iterator it = transactions.entrySet().iterator();
                while (it.hasNext()) {
                    Entry/* <String, Transaction> */entry = (Entry/* <String, Transaction> */) it.next();
                    try {
                        ((Transaction) entry.getValue()).rollbackTransaction();
                    } catch (Throwable e) {
                        LOG.error("Cannot rollback transaction for driver: " + entry.getKey(), e);
                    }
                }
            }
        }

        Profiler.getInstance().trace("<<< Batch modify");
        return result;
    }
}
