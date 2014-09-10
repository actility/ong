package com.actility.m2m.song.binding.http.jni.impl;

import java.util.Collection;
import java.util.HashMap;

public class HttpClientStatsImpl {

    private long requestSent;
    private long responseCompleted;
    private HashMap statusCodes = new HashMap();

    public HttpClientStatsImpl() {
        requestSent = 0;
        responseCompleted = 0;
    }

    synchronized public long getRequestSent() {
        return requestSent;
    }

    synchronized public long getResponseCompleted() {
        return responseCompleted;
    }

    synchronized public Collection getStatusCodeEntries() {
        return statusCodes.entrySet();
    }

    synchronized public void incRequestSent() {
        requestSent++;
    }

    synchronized public void incResponseCompletion(int responseRawStatusCode) {
        responseCompleted++;

        Integer key = new Integer(responseRawStatusCode);
        Integer value = (Integer) statusCodes.get(key);
        int newValue = 1;
        if (value != null) {
            newValue += value.intValue();
        }
        statusCodes.put(key, new Integer(newValue));
    }
}
