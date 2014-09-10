package com.actility.ong.installer.module.config;

import java.net.URI;

import com.actility.ong.installer.config.ArchConfig;

public final class OngConfig {
    private final String sshHost;
    private final int sshPort;
    private final String sshLogin;
    private final ArchConfig arch;
    private final long maxLogSize;
    private final String rootAct;
    private final String nsclUri;
    private final String tpkDevUri;
    private final String gsclCoapHost;
    private final int gsclCoapPort;
    private final boolean dev;
    private final String ongName;
    private final String domainName;

    public OngConfig(String sshHost, int sshPort, String sshLogin, ArchConfig arch, long maxLogSize, String rootAct,
            String nsclUri, String tpkDevUri, String gsclCoapHost, int gsclCoapPort, boolean dev, String ongName,
            String domainName) {
        this.sshHost = sshHost;
        this.sshPort = sshPort;
        this.sshLogin = sshLogin;
        this.arch = arch;
        this.maxLogSize = maxLogSize;
        this.rootAct = URI.create((rootAct.endsWith("/")) ? rootAct : rootAct + "/").normalize().toString();
        this.nsclUri = nsclUri;
        this.tpkDevUri = tpkDevUri;
        this.gsclCoapHost = gsclCoapHost;
        this.gsclCoapPort = gsclCoapPort;
        this.dev = dev;
        this.ongName = ongName;
        this.domainName = domainName;
    }

    public String getSshHost() {
        return sshHost;
    }

    public int getSshPort() {
        return sshPort;
    }

    public String getSshLogin() {
        return sshLogin;
    }

    public ArchConfig getArch() {
        return arch;
    }

    public long getMaxLogSize() {
        return maxLogSize;
    }

    public String getRootAct() {
        return rootAct;
    }

    public String getNsclUri() {
        return nsclUri;
    }

    public String getTpkDevUri() {
        return tpkDevUri;
    }

    public String getGsclCoapHost() {
        return gsclCoapHost;
    }

    public int getGsclCoapPort() {
        return gsclCoapPort;
    }

    public boolean isDev() {
        return dev;
    }

    public String getOngName() {
        return ongName;
    }

    public String getDomainName() {
        return domainName;
    }

    @Override
    public String toString() {
        StringBuffer rep = new StringBuffer("[<OngConfig> ");
        rep.append("(sshHost:").append(this.sshHost).append(") ");
        rep.append("(sshPort:").append(this.sshPort).append(") ");
        rep.append("(sshLogin:").append(this.sshLogin).append(") ");
        rep.append("(arch:").append(this.arch.getName()).append(") ");
        rep.append("(maxLogSize:").append(this.maxLogSize).append(") ");
        rep.append("(rootAct:").append(this.rootAct).append(") ");
        rep.append("(nsclUri:").append(this.nsclUri).append(") ");
        rep.append("(tpkDevUri:").append(this.tpkDevUri).append(") ");
        rep.append("(gsclCoapHost:").append(this.gsclCoapHost).append(") ");
        rep.append("(gsclCoapPort:").append(this.gsclCoapPort).append(") ");
        rep.append("(dev:").append(this.dev).append(")");
        rep.append("(ongName:").append(this.ongName).append(") ");
        rep.append("(domainName:").append(this.domainName).append(") ");
        rep.append("]");
        return rep.toString();
    }
}
