package com.actility.m2m.song.binding.http.jni.impl;

import java.util.Iterator;

public class ResponseHeaderIteratorImpl implements Iterator {

    private JHttpClients httpClientNi;
    private long transactionPtr;
    private int index;

    public ResponseHeaderIteratorImpl(JHttpClients httpClientNi, long transactionPtr) {
        this.httpClientNi = httpClientNi;
        this.transactionPtr = transactionPtr;
        this.index = 0;
    }

    public boolean hasNext() {
        return (null != httpClientNi.getNthResponseHeaderName(transactionPtr, index));
    }

    public Object next() {
        String name = httpClientNi.getNthResponseHeaderName(transactionPtr, index);
        String value = httpClientNi.getNthResponseHeaderValue(transactionPtr, index);
        HeaderEntry entry = new HeaderEntry(name, value);
        index++;
        return entry;
    }

    public void remove() {
        // nothing to do here
    }

}
