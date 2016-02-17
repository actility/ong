package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Iterator;

import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;


public final class SearchResultImpl implements SearchResult {
    private final Iterator iterator;

    /**
     * Class constructor.
     *
     * @param iterator The iterator on search result
     */
    public SearchResultImpl(Iterator iterator) {
        this.iterator = iterator;
    }

    public Iterator getResults() {
        return iterator;
    }

    public void close() throws StorageException {
    }

}
