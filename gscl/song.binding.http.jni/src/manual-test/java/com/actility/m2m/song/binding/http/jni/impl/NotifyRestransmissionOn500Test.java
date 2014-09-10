package com.actility.m2m.song.binding.http.jni.impl;

import java.io.UnsupportedEncodingException;
import java.util.Iterator;
import java.util.Map.Entry;

import junit.framework.TestCase;

import com.actility.m2m.http.client.ni.HttpClient;
import com.actility.m2m.http.client.ni.HttpClientTransaction;

public class NotifyRestransmissionOn500Test extends TestCase {

    public void testNotifyReEmittedRequests() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();

        httpClientNiService.setTraceLevel(5);
        httpClientNiService.init(respHandler);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        client.setMaxSocketsPerHost(1);
        client.setMaxTotalSockets(1);
        client.setTcpNoDelay(true);

        assertNotNull(client);

        String body = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                + "<m2m:notify xmlns:m2m=\"http://uri.etsi.org/m2m\" xmlns:xmime=\"http://www.w3.org/2005/05/xmlmime\">"
                + "<m2m:statusCode>STATUS_OK</m2m:statusCode><representation xmime:contentType=\"application/xml; charset=utf-8\">PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPG0ybTphcHBsaWNhdGlv\r\n"
                + "bnMgeG1sbnM6bTJtPSJodHRwOi8vdXJpLmV0c2kub3JnL20ybSI+PG0ybTphY2Nlc3NSaWdo\r\n"
                + "dElEPi9tMm0vYWNjZXNzUmlnaHRzL0xvY2FkbWluX0FSLzwvbTJtOmFjY2Vzc1JpZ2h0SUQ+\r\n"
                + "PG0ybTpjcmVhdGlvblRpbWU+MjAxNC0wNS0xNFQxNjozNTozNS4yNDgrMDI6MDA8L20ybTpj\r\n"
                + "cmVhdGlvblRpbWU+PG0ybTpsYXN0TW9kaWZpZWRUaW1lPjIwMTQtMDUtMTRUMTY6MzU6NDIu\r\n"
                + "OTk4KzAyOjAwPC9tMm06bGFzdE1vZGlmaWVkVGltZT48bTJtOmFwcGxpY2F0aW9uQ29sbGVj\r\n"
                + "dGlvbj48bTJtOm5hbWVkUmVmZXJlbmNlIGlkPSJJRUNfNjExMzFfVk0iPklFQ182MTEzMV9W\r\n"
                + "TS88L20ybTpuYW1lZFJlZmVyZW5jZT48bTJtOm5hbWVkUmVmZXJlbmNlIGlkPSJJUFVfS05Y\r\n"
                + "Ij5JUFVfS05YLzwvbTJtOm5hbWVkUmVmZXJlbmNlPjxtMm06bmFtZWRSZWZlcmVuY2UgaWQ9\r\n"
                + "IlNZU1RFTSI+U1lTVEVNLzwvbTJtOm5hbWVkUmVmZXJlbmNlPjwvbTJtOmFwcGxpY2F0aW9u\r\n"
                + "Q29sbGVjdGlvbj48bTJtOmFwcGxpY2F0aW9uQW5uY0NvbGxlY3Rpb24vPjxtMm06c3Vic2Ny\r\n"
                + "aXB0aW9uc1JlZmVyZW5jZT4vbTJtL2FwcGxpY2F0aW9ucy9zdWJzY3JpcHRpb25zLzwvbTJt\r\n"
                + "OnN1YnNjcmlwdGlvbnNSZWZlcmVuY2U+PG0ybTptZ210T2Jqc1JlZmVyZW5jZT4vbTJtL2Fw\r\n"
                + "cGxpY2F0aW9ucy9tZ210T2Jqcy88L20ybTptZ210T2Jqc1JlZmVyZW5jZT48L20ybTphcHBs\r\n"
                + "aWNhdGlvbnM+Cg==</representation><subscriptionReference>http://testerman-ong1.actility.com:8080/m2m/applications/subscriptions/SCL_SUBSCRIBE_APPLICATIONS_003</subscriptionReference></m2m:notify>\n";

        // define an URL to a server that will answer with a 500 response
        String notificationServerUrl = "http://10.10.12.216:9090/";
        int i = 10;
        while (i > 0) {
            respHandler.reset();
            HttpClientTransaction transaction1 = null;
            // String transId1 = "56728e2db105a42a";
            try {
                // transaction1 = client.createTransaction(transId1, "POST", notificationServerUrl);
                transaction1 = client.createTransaction("POST", notificationServerUrl);
                transaction1.addRequestHeader("Authorization",
                        "Basic aHR0cCUzQS8vdGVzdGVybWFuLW9uZzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm06");
                transaction1.addRequestHeader("Content-Length", "1475");
                transaction1.addRequestHeader("User-Agent", "ONG/3.2.7");
                transaction1.addRequestHeader("Content-Type", "application/xml; charset=utf-8");
                transaction1.setRequestBody(body.getBytes());
            } catch (Exception e) {
                e.printStackTrace();
                assertTrue(false);
            }
            try {
                transaction1.sendRequest();
            } catch (Exception e) {
                e.printStackTrace();
                assertTrue(false);
            }

            respHandler.waitForResponse();
            assertEquals(500, transaction1.getResponseStatusCode()); // the use case is to re-emit the notify 10s later on a 500
            // response
            System.out.println("result received: ");
            System.out.println(transaction1.getProtocol() + " " + transaction1.getResponseStatusCode() + " "
                    + transaction1.getResponseStatusText());
            Iterator headerIter = transaction1.getResponseHeaders();
            while (headerIter.hasNext()) {
                Entry elt = (Entry) headerIter.next();
                System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
            }
            System.out.println();
            if (transaction1.getResponseContentLength() > 0) {
                try {
                    System.out.println(new String(transaction1.getResponseBody(), "UTF-8"));
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            }

            transaction1.release();

            System.out.println("*** Request #" + i + " done, sleeping 5s");

            Thread.sleep(5000);
            i--;
        }
        //
        // HttpClientTransaction transaction2 = null;
        // String transId2 = "a2815eb8a99c6933";
        // try {
        // transaction2 = client.createTransaction(transId2, "POST", notificationServerUrl);
        // transaction2.addRequestHeader("Authorization",
        // "Basic aHR0cCUzQS8vdGVzdGVybWFuLW9uZzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm06");
        // transaction2.addRequestHeader("Content-Length", "1475");
        // transaction2.addRequestHeader("User-Agent", "ONG/3.2.7");
        // transaction2.addRequestHeader("Content-Type", "application/xml; charset=utf-8");
        // transaction2.setRequestBody(body.getBytes());
        // } catch (Exception e) {
        // e.printStackTrace();
        // assertTrue(false);
        // }
        // try {
        // transaction2.sendRequest();
        // } catch (Exception e) {
        // e.printStackTrace();
        // assertTrue(false);
        // }
        //
        // respHandler.waitForResponse();
        // assertEquals(500, transaction2.getResponseStatusCode()); // the use case is to re-emit the notify 10s later on a 500
        // // response
        // System.out.println("result received: ");
        // System.out.println(transaction2.getProtocol() + " " + transaction2.getResponseStatusCode() + " "
        // + transaction2.getResponseStatusText());
        // headerIter = transaction2.getResponseHeaders();
        // while (headerIter.hasNext()) {
        // Entry elt = (Entry) headerIter.next();
        // System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        // }
        // System.out.println();
        // if (transaction2.getResponseContentLength() > 0) {
        // try {
        // System.out.println(new String(transaction2.getResponseBody(), "UTF8"));
        // } catch (UnsupportedEncodingException e) {
        // e.printStackTrace();
        // }
        // }
        // System.out.println("Second request done");

        httpClientNiService.stop();
    }
}
