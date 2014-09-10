package com.actility.maven.plugin.cocoon;

import org.apache.maven.artifact.Artifact;
import org.apache.maven.artifact.versioning.ArtifactVersion;
import org.apache.maven.artifact.versioning.OverConstrainedVersionException;
import org.apache.maven.plugin.MojoExecutionException;
import org.codehaus.plexus.util.StringUtils;

public class MavenUtils {

    public static final String getOSGiVersion(Artifact artifact) throws MojoExecutionException {
        try {
            ArtifactVersion artifactVersion = artifact.getSelectedVersion();
            String res = String.valueOf(artifactVersion.getMajorVersion()) + "."
                    + String.valueOf(artifactVersion.getMinorVersion()) + "."
                    + String.valueOf(artifactVersion.getIncrementalVersion());
            if (StringUtils.isNotEmpty(artifactVersion.getQualifier())) {
                res += "." + artifactVersion.getQualifier();
            }
            return res;
        } catch (OverConstrainedVersionException e) {
            throw new MojoExecutionException("Cannot build version for dependency " + artifact.getGroupId() + ":"
                    + artifact.getArtifactId());
        }
    }
}
