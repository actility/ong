package com.actility.ong.installer.module.config;

public final class GsclConfig {
    // private final URI acsUrl;
    private final boolean httpProxy;
    private final String httpProxyHost;
    private final int httpProxyPort;

    public GsclConfig(/* String acsUrl, */boolean httpProxy, String httpProxyHost, int httpProxyPort) {
        // this.acsUrl = URI.create(acsUrl);
        this.httpProxy = httpProxy;
        this.httpProxyHost = httpProxyHost;
        this.httpProxyPort = httpProxyPort;
    }

    // public URI getAcsUrl() {
    // return acsUrl;
    // }

    public boolean isHttpProxy() {
        return httpProxy;
    }

    public String getHttpProxyHost() {
        return httpProxyHost;
    }

    public int getHttpProxyPort() {
        return httpProxyPort;
    }

    @Override
    public String toString() {
        StringBuffer rep = new StringBuffer("[<GsclConfig> ");
        rep.append("(httpProxy:").append(this.httpProxy).append(") ");
        rep.append("(httpProxyHost:").append(this.httpProxyHost).append(") ");
        rep.append("(httpProxyPort:").append(this.httpProxyPort).append(") ");
        rep.append("]");
        return rep.toString();
    }
}
