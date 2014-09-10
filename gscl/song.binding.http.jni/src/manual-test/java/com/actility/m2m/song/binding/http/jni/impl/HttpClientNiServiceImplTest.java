package com.actility.m2m.song.binding.http.jni.impl;

import java.io.UnsupportedEncodingException;
import java.util.Iterator;
import java.util.Map.Entry;

import junit.framework.TestCase;

import com.actility.m2m.http.client.ni.HttpClient;
import com.actility.m2m.http.client.ni.HttpClientTransaction;

public class HttpClientNiServiceImplTest extends TestCase {

    static final private String MY_ONG_DIRECT_ACCESS_BASE_URI = "http://10.10.12.3:8080/m2m";
    static final private String MY_ONG_BASE_URI = "http://msi-d43d7e700d51.poc1.actility.com:8080/m2m";

    /**
     * Copy a httpclient
     */
    public void testCopyHttpClient() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClient clientCopy = client.copy();
        assertNotNull(clientCopy);

        assertNotSame(client, clientCopy);

        httpClientNiService.stop();
    }

    /**
     * GET request to http://www.example.com/
     */
    public void testSimpleGETRequest() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test1";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            // transaction = client.createTransaction(transId, "GET", "http://10.10.12.216:9091/");
            transaction = client.createTransaction(transId, "GET", "http://10.10.12.65:8181/songBindingHttpTester/myServlet");
            transaction.addRequestHeader("Accept", "text/html");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(200, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * GET request to http://nsc1.actility.com:8088/m2m
     */
    public void testSimpleGETRequestToNsc8088() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-GET-nsc8088";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            transaction = client.createTransaction(transId, "GET", "http://nsc1.actility.com:8088/m2m");
            transaction.addRequestHeader("Accept", "text/html");
            transaction.addRequestHeader("Accept", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vbnNjMS5hY3RpbGl0eS5jb20lM2E4MDg4L20ybS9hcHBsaWNhdGlvbnMvU1VQUE9SVDo=");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(200, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * GET request to http://rms.poc1.actility.com:8080/m2m
     */
    public void testSimpleGETRequestToNscPoc1() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-GET-NscPoc1";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            transaction = client.createTransaction(transId, "GET", "http://rms.poc1.actility.com:8080/m2m");
            transaction.addRequestHeader("Accept", "text/html");
            transaction.addRequestHeader("Accept", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb24vU1VQUE9SVDo=");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(200, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * GET request to MY_ONG_BASE_URI (ONG running on MSI, registered near rms.poc1), through rms.poc1
     */
    public void testSimpleGETRequestWithProxy() throws InterruptedException {

        // of course this is not a normal use case, as far as this lib is intend to be used on ONG
        // but this is to test proxy behavior
        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-GET-Msi-behind-NscPoc1";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        client.setProxy("rms.poc1.actility.com", 8080);
        client.setProxyCredentials("http%3a//rms.poc1.actility.com%3a8080/m2m/applications/SUPPORT", "");

        HttpClientTransaction transaction = null;
        try {
            transaction = client.createTransaction(transId, "GET", MY_ONG_BASE_URI);
            // transaction.addRequestHeader("Accept", "application/xml");
            // transaction.addRequestHeader("Content-Type", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb25zL1NZU1RFTTo=");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();

        assertEquals(200, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * GET request to MY_ONG_DIRECT_ACCESS_BASE_URI/applications (ONG running on MSI, registered near rms.poc1), to retrieve
     * applications
     */
    public void testSimpleGETRequestToMsi() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-GET-msi";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            transaction = client.createTransaction(transId, "GET", MY_ONG_DIRECT_ACCESS_BASE_URI + "/applications/");
            transaction.addRequestHeader("Content-Length", "0");

            transaction.addRequestHeader("Content-Type", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb25zL1NZU1RFTTo=");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();

        assertEquals(200, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * POST request to MY_ONG_DIRECT_ACCESS_BASE_URI/applications/test.song.binding.http.jni (ONG running on MSI, registered
     * near rms.poc1), to create the new application
     */
    public void testSimplePOSTRequestToMsi() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-POST-msi";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            String body = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    + "<m2m:application xmlns:m2m=\"http://uri.etsi.org/m2m\" appId=\"test.song.binding.http.jni\">"
                    + "    <m2m:accessRightID>/m2m/accessRights/Locadmin_AR1</m2m:accessRightID>" + "</m2m:application>";
            byte[] content = body.getBytes();
            transaction = client.createTransaction(transId, "POST", MY_ONG_DIRECT_ACCESS_BASE_URI + "/applications/");
            transaction.addRequestHeader("Content-Length", Integer.toString(content.length));

            transaction.addRequestHeader("Content-Type", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb25zL1NZU1RFTTo=");

            transaction.setRequestBody(content);
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(201, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * PUT request to MY_ONG_DIRECT_ACCESS_BASE_URI/applications/test.song.binding.http.jni (ONG running on MSI, registered near
     * rms.poc1), to update the previously created application
     */
    public void testSimplePUTRequestToMsi() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-PUT-msi";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            String body = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    + "<m2m:application xmlns:m2m=\"http://uri.etsi.org/m2m\">"
                    + "<m2m:accessRightID>/m2m/accessRights/Locadmin_AR1</m2m:accessRightID>" + "</m2m:application>";

            byte[] content = body.getBytes();
            transaction = client.createTransaction(transId, "PUT", MY_ONG_DIRECT_ACCESS_BASE_URI
                    + "/applications/test.song.binding.http.jni");
            transaction.addRequestHeader("Content-Length", Integer.toString(content.length));

            transaction.addRequestHeader("Content-Type", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb25zL1NZU1RFTTo=");

            transaction.setRequestBody(content);
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(200, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * DELETE request to MY_ONG_DIRECT_ACCESS_BASE_URI/applications/test.song.binding.http.jni (ONG running on MSI, registered
     * near rms.poc1), to delete the previously created application
     */
    public void testSimpleDELETERequestToMsi() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-DELETE-msi";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            transaction = client.createTransaction(transId, "DELETE", MY_ONG_DIRECT_ACCESS_BASE_URI
                    + "/applications/test.song.binding.http.jni");
            transaction.addRequestHeader("Content-Length", "0");

            transaction.addRequestHeader("Content-Type", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb25zL1NZU1RFTTo=");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(204, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * POST request to MY_ONG_DIRECT_ACCESS_BASE_URI (ONG running on MSI, registered near rms.poc1), with no content, for a
     * re-targeting "toggle" operation on a plug
     */
    public void testEmptyPOSTRequestToMsi() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-toggle-msi";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            transaction = client.createTransaction(transId, "POST", MY_ONG_DIRECT_ACCESS_BASE_URI
                    + "/applications/APP_00137a00000080ea.1/retargeting2/0x0006.0x02.ope");
            transaction.addRequestHeader("Content-Length", "0");

            transaction.addRequestHeader("Content-Type", "application/xml");
            transaction.addRequestHeader("Authorization",
                    "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb25zL1NZU1RFTTo=");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(201, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }

    /**
     * POST request to an unknown host, and check that the request complete with the appropriate code.
     */
    public void testRequestToUnknownHost() throws InterruptedException {

        HttpClientNiServiceImpl httpClientNiService = new HttpClientNiServiceImpl(true, new HttpClientStatsImpl());
        ResponseHandlerImpl respHandler = new ResponseHandlerImpl();
        String transId = "req-test-unknown-host";

        httpClientNiService.init(respHandler);
        httpClientNiService.setTraceLevel(10);
        httpClientNiService.start();

        HttpClient client = httpClientNiService.createHttpClient();
        assertNotNull(client);

        HttpClientTransaction transaction = null;
        try {
            transaction = client.createTransaction(transId, "GET", "http://unknown.actility.com/m2m");
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }
        try {
            transaction.sendRequest();
        } catch (Exception e) {
            e.printStackTrace();
            assertTrue(false);
        }

        respHandler.waitForResponse();
        assertEquals(404, transaction.getResponseStatusCode());
        System.out.println("result received: ");
        System.out.println(transaction.getProtocol() + " " + transaction.getResponseStatusCode() + " "
                + transaction.getResponseStatusText());
        Iterator headerIter = transaction.getResponseHeaders();
        while (headerIter.hasNext()) {
            Entry elt = (Entry) headerIter.next();
            System.out.println((String) elt.getKey() + ": " + (String) elt.getValue());
        }
        System.out.println();
        if (transaction.getResponseContentLength() > 0) {
            try {
                System.out.println(new String(transaction.getResponseBody(), "UTF8"));
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        httpClientNiService.stop();
    }
}
