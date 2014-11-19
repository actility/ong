package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Date;

import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.driver.Transaction;
import com.actility.m2m.storage.driver.sqlite.impl.DocumentImpl;
import com.actility.m2m.storage.driver.sqlite.impl.SQLiteRequestExecutor;

public class RunnableMain implements Runnable {

    SQLiteRequestExecutor storage;
    Transaction transaction = null;

    public RunnableMain(SQLiteRequestExecutor storage) {
        this.storage = storage;
    }

    public void run() {
        System.out.println("Launch:" + Thread.currentThread().getName());
        try {
            transaction = storage.beginTransaction();
        } catch (StorageException e) {
            junit.framework.Assert.fail(e.getMessage());
        }
        if (transaction != null) {
            for (int j = 0; j < 10; j++) {
                String docName = "/m2m/applications/APP_0000000" + Thread.currentThread().getName() + j;
                Document doc = new DocumentImpl(null, docName);
                doc.setAttribute("creationTime", new Date());
                try {
                    junit.framework.Assert.assertTrue(transaction.create(null, doc));
                } catch (StorageException e) {
                    junit.framework.Assert.fail("create operation on " + docName + " failed: " + e);
                }
            }
            try {
                transaction.commitTransaction();
            } catch (StorageException e) {
                junit.framework.Assert.fail(e.getMessage());
            }
        }
        System.out.println("Done Thread:" + Thread.currentThread().getName());
    }

    public Transaction getTransaction() {
        return transaction;
    }

    public void setTransaction(Transaction transaction) {
        this.transaction = transaction;
    }
}
