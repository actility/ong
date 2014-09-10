package com.actility.maven.plugin.cocoon;

import java.util.Map;

import org.apache.maven.artifact.Artifact;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugin.logging.Log;
import org.codehaus.plexus.util.StringUtils;

public class FirmwareEntryConfiguration {
    private static final char[] TOKENS = new char[] { ',', '"' };

    protected Log log;
    protected Artifact artifact;
    protected String location;
    protected String id;
    protected String namespace;
    protected String name;
    protected String installationType;
    protected String version;
    protected boolean comment;
    protected boolean oneTime;

    public FirmwareEntryConfiguration(Log log, Map<String, Artifact> nameTodependency, String[] args)
            throws MojoExecutionException {
        this.log = log;
        if (args.length != 8) {
            throw new MojoExecutionException("Firmware entry configuration has a wrong number of arguments: "
                    + StringUtils.join(args, ":"));
        }
        artifact = nameTodependency.get(args[0] + "~" + args[1] + "~" + args[2]);
        if (artifact == null) {
            throw new MojoExecutionException("Firmware dependency does not exist: " + StringUtils.join(args, ":"));
        }
        location = args[3];
        id = args[4];
        namespace = args[5];
        name = args[6];
        installationType = args[7];
        comment = false;
        oneTime = false;

        version = MavenUtils.getOSGiVersion(artifact);
    }

    public FirmwareEntryConfiguration(Log log) throws MojoExecutionException {
        this.log = log;
    }

    public Artifact getArtifact() {
        return artifact;
    }

    public String getLocation() {
        return location;
    }

    public String getId() {
        return id;
    }

    public String getNamespace() {
        return namespace;
    }

    public String getName() {
        return name;
    }

    public String getInstallationType() {
        return installationType;
    }

    public boolean isComment() {
        return comment;
    }

    public boolean isOneTime() {
        return oneTime;
    }

    public String getVersion() {
        return version;
    }

    public String getDestFile() {
        StringBuffer destFileName = new StringBuffer();

        String versionString = null;
        versionString = "-" + artifact.getVersion();

        String classifierString = "";

        if (StringUtils.isNotEmpty(artifact.getClassifier())) {
            classifierString = "-" + artifact.getClassifier();
        }
        if (location != null) {
            destFileName.append(location);
            if (!location.endsWith("/")) {
                destFileName.append('/');
            }
        }
        destFileName.append(artifact.getArtifactId()).append(versionString);
        destFileName.append(classifierString).append(".");
        destFileName.append(artifact.getArtifactHandler().getExtension());

        return destFileName.toString();
    }
}
