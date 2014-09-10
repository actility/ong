package com.actility.m2m.song.binding.http.jni.impl;

import org.apache.log4j.Logger;

import com.actility.m2m.http.client.ni.HttpClient;
import com.actility.m2m.http.client.ni.HttpClientHandler;
import com.actility.m2m.http.client.ni.HttpClientNiService;
import com.actility.m2m.song.binding.http.jni.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public class HttpClientNiServiceImpl implements HttpClientNiService {

    private static final Logger LOG = OSGiLogger.getLogger(HttpClientNiServiceImpl.class, BundleLogger.LOG);

    private boolean curlDebugMode;
    private JHttpClients httpClientNi;
    private Thread clientsThread;
    private int traceLevel;
    private HttpClientStatsImpl statModule;

    public HttpClientNiServiceImpl(boolean curlDebugMode, HttpClientStatsImpl statModule) {
        this.curlDebugMode = curlDebugMode;
        this.statModule = statModule;
        httpClientNi = new JHttpClients(this);
    }

    public void start() throws InterruptedException {
        synchronized (httpClientNi) {
            httpClientNi.setTraceLevel(traceLevel);
            clientsThread = new Thread(httpClientNi);
            clientsThread.start();
            httpClientNi.wait();
        }
        if (httpClientNi.e != null) {
            throw httpClientNi.e;
        }
    }

    public void stop() {
        httpClientNi.httpClientsStop();
        try {
            clientsThread.join();
        } catch (InterruptedException e) {
            LOG.error("Thread interrupted while joining song.binding.http.jni native code thread", e);
        }
    }

    public void init(HttpClientHandler handler) {
        httpClientNi.init(handler, statModule);
    }

    public HttpClient createHttpClient() {
        long clientPtr = httpClientNi.createHttpClient(curlDebugMode);
        HttpClientImpl client = new HttpClientImpl(statModule, this, clientPtr, httpClientNi);
        return client;
    }

    public void setTraceLevel(int level) {
        this.traceLevel = level;
    }

    public JHttpClients getHttpClientNi() {
        return httpClientNi;
    }
}
