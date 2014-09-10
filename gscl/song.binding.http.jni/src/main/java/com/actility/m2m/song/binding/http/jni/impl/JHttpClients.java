package com.actility.m2m.song.binding.http.jni.impl;

import java.io.IOException;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

import com.actility.m2m.http.client.ni.HttpClientHandler;
import com.actility.m2m.song.binding.http.jni.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public final class JHttpClients implements Runnable {

    private static final Level[] LEVELS = { Level.FATAL, Level.ERROR, Level.WARN, Level.INFO, Level.DEBUG, Level.TRACE };

    static {
        try {
            System.loadLibrary("HttpClient");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Failed to load native code library libHttpClient.so.\n" + e);
        }
    }

    private static final Logger LOG = OSGiLogger.getLogger(JHttpClients.class, BundleLogger.LOG);

    public RuntimeException e;
    private HttpClientHandler handler;
    private HttpClientNiServiceImpl httpClientNiService;
    private HttpClientStatsImpl statModule;

    public JHttpClients(HttpClientNiServiceImpl httpClientNiService) {
        this.httpClientNiService = httpClientNiService;
        httpClientsInit();
    }

    public void init(HttpClientHandler handler, HttpClientStatsImpl statModule) {
        this.handler = handler;
        this.statModule = statModule;
    }

    public void doTrace(int level, String message) {
        Level l;
        if (level > 5) {
            level = 5;
        }
        l = LEVELS[level];
        LOG.log(l, message);
    }

    public void doResponseCompletion(Object obj) {
        try {
            HttpClientTransactionImpl transaction = (HttpClientTransactionImpl) obj;

            statModule.incResponseCompletion(transaction.getResponseRawStatusCode());

            handler.doClientResponse(transaction);
        } catch (ClassCastException e) {
            LOG.error("wrong reference on transaction provided", e);
        } catch (IOException e) {
            LOG.error("client failure on response completion", e);
        }
    }

    public native boolean httpClientsInit();

    private native void httpClientsRun();

    public native void httpClientsStop();

    public native void setTraceLevel(int level);

    /**
     * Create a new handler for a new HTTP client.
     * 
     * @param curlDebugEnable tells if debug mode must be enable while creating the client.
     * @return return the pointer value on the native data structure. The issuer have then to provide this identifier for each
     *         query. If 0, client handler cannot be created.
     */
    public native long createHttpClient(boolean curlDebugEnable);

    // client configuration
    public native void setProxyCredentials(long clientPtr, String proxyUsername, String proxyPassword);

    public native void setProxy(long clientPtr, String proxyHost, int proxyPort);

    public native void setMaxSocketsPerHost(long clientPtr, int maxSocketsPerHost);

    public native void setMaxTotalSockets(long clientPtr, int maxTotalSockets);

    public native void setTcpNoDelay(long clientPtr, boolean tcpNoDelay);

    public native void setRequestTimeout(long clientPtr, long requestTimeout);

    public native void setConnectionTimeout(long clientPtr, long connectionTimeout);

    public native String dumpPendingTransactions();

    // transactions
    public native long createTransaction(long clientPtr, Object transactionImpl, String transId, String method, String url);

    public native void releaseTransaction(long clientPtr, long transPtr);

    public native void addRequestHeader(long transPtr, String name, String value);

    public native void setRequestBody(long transPtr, byte[] content);

    public native void setRequestProxy(long transPtr, String host, int port);

    public native void sendRequest(long transPtr);

    public native int getResponseStatusCode(long transPtr);

    public native int getResponseRawStatusCode(long transPtr);

    public native String getResponseReasonPhrase(long transPtr);

    public native String getResponseHeader(long transPtr, String name);

    public native String getNthResponseHeaderName(long transPtr, int index);

    public native String getNthResponseHeaderValue(long transPtr, int index);

    public native int getResponseContentLength(long transPtr);

    public native byte[] getResponseBody(long transPtr);

    public native String getRemoteAddress(long transPtr);

    public native int getRemotePort(long transPtr);

    public native String getLocalAddress(long transPtr);

    public native int getLocalPort(long transPtr);

    public native String getProtocol(long transPtr);

    /**
     * Run the thread for native processing.<br>
     * First it initialize the native layer (httpClientsInit) and then run the loop processing (httpClientsRun).
     */
    public void run() {
        Thread.currentThread().setName("JHttpClients");
        try {
            if (!httpClientsInit()) {
                this.e = new IllegalStateException("Failed to initialize HttpClient stack");
            }
        } catch (RuntimeException e) {
            this.e = e;
        }
        synchronized (this) {
            notify(); // notify listeners that initialization succeeds
        }

        if (e == null) {
            try {
                httpClientsRun();
            } catch (RuntimeException e) {
                LOG.error("Problem while running HttpClient stack", e);
            }
        }

    }
}
