package com.actility.m2m.song.binding.http.jni.impl;

import java.io.IOException;

import com.actility.m2m.http.client.ni.HttpClientHandler;
import com.actility.m2m.http.client.ni.HttpClientTransaction;

public class ResponseHandlerImpl implements HttpClientHandler {
    private boolean responded = false;

    public ResponseHandlerImpl() {
        System.err.println("Creating new ResponseHandlerImpl");
    }

    synchronized public void waitForResponse() {
        System.err.println("ResponseHandlerImpl: waiting for the response");
        while (!responded) {
            try {
                wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    synchronized public void doClientResponse(HttpClientTransaction transaction) throws IOException {
        System.err.println("ResponseHandlerImpl: response has arrived");
        responded = true;
        notify();
    }

    public void reset() {
        responded = false;
    }
}
