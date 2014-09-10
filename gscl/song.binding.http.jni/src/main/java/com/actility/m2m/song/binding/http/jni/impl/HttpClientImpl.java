package com.actility.m2m.song.binding.http.jni.impl;

import com.actility.m2m.http.client.ni.HttpClient;
import com.actility.m2m.http.client.ni.HttpClientNiService;
import com.actility.m2m.http.client.ni.HttpClientTransaction;
import com.actility.m2m.util.UUID;

public class HttpClientImpl implements HttpClient {

    private long clientPtr;
    private String proxyUsername;
    private String proxyPassword;
    private String proxyHost;
    private int proxyPort;
    private int maxSocketsPerHost;
    private int maxTotalSockets;
    private boolean tcpNoDelay;
    private long requestTimeout;
    private long connectionTimeout;
    private HttpClientNiService httpClientNiService;
    private JHttpClients httpClientNi;
    private HttpClientStatsImpl statModule;

    public HttpClientImpl(HttpClientStatsImpl statModule, HttpClientNiService httpClientNiService, long clientPtr,
            JHttpClients httpClientNi) {
        this.statModule = statModule;
        this.httpClientNiService = httpClientNiService;
        this.clientPtr = clientPtr;
        this.httpClientNi = httpClientNi;
    }

    public HttpClientTransaction createTransaction(String method, String url) {
        String id = UUID.randomUUID(16);
        return createTransaction(id, method, url);
    }

    public HttpClientTransaction createTransaction(String id, String method, String url) {
        HttpClientTransactionImpl transaction = new HttpClientTransactionImpl(this.statModule, httpClientNi, clientPtr, id);
        long transactionPtr = httpClientNi.createTransaction(clientPtr, transaction, id, method, url);
        transaction.setTransactionPtr(transactionPtr);
        return transaction;
    }

    public HttpClient copy() {
        HttpClientImpl theCopy = (HttpClientImpl) httpClientNiService.createHttpClient();
        theCopy.setConnectionTimeout(connectionTimeout);
        theCopy.setMaxSocketsPerHost(maxSocketsPerHost);
        theCopy.setMaxTotalSockets(maxTotalSockets);
        theCopy.setProxy(proxyHost, proxyPort);
        theCopy.setProxyCredentials(proxyUsername, proxyPassword);
        theCopy.setRequestTimeout(requestTimeout);
        theCopy.setTcpNoDelay(tcpNoDelay);
        return theCopy;
    }

    public void setProxyCredentials(String proxyUsername, String proxyPassword) {
        this.proxyUsername = proxyUsername;
        this.proxyPassword = proxyPassword;
        this.httpClientNi.setProxyCredentials(clientPtr, proxyUsername, proxyPassword);
    }

    public void setProxy(String proxyHost, int proxyPort) {
        this.proxyHost = proxyHost;
        this.proxyPort = proxyPort;
        this.httpClientNi.setProxy(clientPtr, proxyHost, proxyPort);
    }

    public void setMaxSocketsPerHost(int maxSocketsPerHost) {
        this.maxSocketsPerHost = maxSocketsPerHost;
        this.httpClientNi.setMaxSocketsPerHost(clientPtr, maxSocketsPerHost);
    }

    public void setMaxTotalSockets(int maxTotalSockets) {
        this.maxTotalSockets = maxTotalSockets;
        this.httpClientNi.setMaxTotalSockets(clientPtr, maxTotalSockets);
    }

    public void setTcpNoDelay(boolean tcpNoDelay) {
        this.tcpNoDelay = tcpNoDelay;
        this.httpClientNi.setTcpNoDelay(clientPtr, tcpNoDelay);
    }

    public void setRequestTimeout(long requestTimeout) {
        this.requestTimeout = requestTimeout;
        this.httpClientNi.setRequestTimeout(clientPtr, requestTimeout);
    }

    public void setConnectionTimeout(long connectionTimeout) {
        this.connectionTimeout = connectionTimeout;
        this.httpClientNi.setConnectionTimeout(clientPtr, connectionTimeout);
    }

}
