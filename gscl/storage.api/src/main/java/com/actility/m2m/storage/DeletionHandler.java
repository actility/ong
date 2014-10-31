package com.actility.m2m.storage;

/**
 * Callback class in order for an application to be notified of a document deletion
 */
public interface DeletionHandler {
    /**
     * Callback that notifies an application of a document deletion
     *
     * @param document The document that is deleted
     */
    public void deleted(Document document);
}
