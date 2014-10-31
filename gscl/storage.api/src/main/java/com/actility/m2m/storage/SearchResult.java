package com.actility.m2m.storage;

import java.util.Iterator;

/**
 * Holds the result of a search operation on the storage
 */
public interface SearchResult {
    /**
     * Gets an iterator on documents that matches the search operation
     *
     * @return The iterator on documents that matches the search operation
     */
    public Iterator/* <Document> */getResults();

    /**
     * Closes the search result.
     * <p>
     * After this method call, the search result is invalidated and therefore cannot be iterated
     *
     * @throws StorageException If any problem occurs while closing the search result
     */
    public void close() throws StorageException;
}
