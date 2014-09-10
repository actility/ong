package com.actility.m2m.song.binding.http.jni.impl;

import java.util.Iterator;

import com.actility.m2m.http.client.ni.HttpClientException;
import com.actility.m2m.http.client.ni.HttpClientTransaction;

public class HttpClientTransactionImpl implements HttpClientTransaction {

    private long clientPtr;
    private long transactionPtr;
    private JHttpClients httpClientNi;
    private String id;
    private HttpClientStatsImpl statModule;

    public HttpClientTransactionImpl(HttpClientStatsImpl statModule, JHttpClients httpClientNi, long clientPtr, String id) {
        this.statModule = statModule;
        this.httpClientNi = httpClientNi;
        this.clientPtr = clientPtr;
        this.id = id;
        this.transactionPtr = 0;
    }

    protected void setTransactionPtr(long transactionPtr) {
        this.transactionPtr = transactionPtr;
    }

    public void addRequestHeader(String name, String value) throws HttpClientException {
        this.httpClientNi.addRequestHeader(transactionPtr, name, value);
    }

    public void setRequestBody(byte[] content) throws HttpClientException {
        this.httpClientNi.setRequestBody(transactionPtr, content);
    }

    public void setRequestProxy(String host, int port) throws HttpClientException {
        this.httpClientNi.setRequestProxy(transactionPtr, host, port);
    }

    public void sendRequest() throws HttpClientException {
        statModule.incRequestSent();
        this.httpClientNi.sendRequest(transactionPtr);
    }

    public int getResponseStatusCode() {
        return this.httpClientNi.getResponseStatusCode(transactionPtr);
    }

    public int getResponseRawStatusCode() {
        return this.httpClientNi.getResponseRawStatusCode(transactionPtr);
    }

    public String getResponseStatusText() {
        return this.httpClientNi.getResponseReasonPhrase(transactionPtr);
    }

    public String getResponseHeader(String name) {
        return this.httpClientNi.getResponseHeader(transactionPtr, name);
    }

    public Iterator getResponseHeaders() {
        return new ResponseHeaderIteratorImpl(httpClientNi, transactionPtr);
    }

    public int getResponseContentLength() {
        return this.httpClientNi.getResponseContentLength(transactionPtr);
    }

    public byte[] getResponseBody() {
        return this.httpClientNi.getResponseBody(transactionPtr);
    }

    public String getRemoteAddress() {
        return this.httpClientNi.getRemoteAddress(transactionPtr);
    }

    public int getRemotePort() {
        return this.httpClientNi.getRemotePort(transactionPtr);
    }

    public String getLocalAddress() {
        return this.httpClientNi.getLocalAddress(transactionPtr);
    }

    public int getLocalPort() {
        return this.httpClientNi.getLocalPort(transactionPtr);
    }

    public String getProtocol() {
        return this.httpClientNi.getProtocol(transactionPtr);
    }

    public String getId() {
        return id;
    }

    public void release() {
        httpClientNi.releaseTransaction(clientPtr, transactionPtr);
    }

}
