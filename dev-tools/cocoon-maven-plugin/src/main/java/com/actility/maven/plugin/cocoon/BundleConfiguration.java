package com.actility.maven.plugin.cocoon;

import java.io.FileInputStream;
import java.io.IOException;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.jar.Attributes;
import java.util.jar.JarInputStream;

import org.apache.maven.artifact.Artifact;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugin.logging.Log;
import org.codehaus.plexus.util.StringUtils;

public class BundleConfiguration extends FirmwareEntryConfiguration {
    private static final char[] TOKENS = new char[] { ',', '"' };

    private int startLevel;
    private boolean toStart;

    public BundleConfiguration(Log log, Map<String, Artifact> nameTodependency, String classifier, String[] args)
            throws MojoExecutionException {
        super(log);
        if (args.length < 3 || args.length > 8) {
            throw new MojoExecutionException("Bundle configuration has a wrong number of arguments: "
                    + StringUtils.join(args, ":"));
        }
        try {
            optional = false;
            if (args.length >= 5) {
                optional = Boolean.parseBoolean(args[4]);
            }
            artifact = nameTodependency.get(args[0] + ":" + args[1]);
            if (artifact == null) {
                artifact = nameTodependency.get(args[0] + ":" + args[1] + ":" + classifier);
                if (artifact == null && !optional) {
                    throw new MojoExecutionException("Bundle dependency does not exist: " + args[0] + ":" + args[1]
                            + ((classifier != null) ? ":" + classifier : ""));
                }
            }
            startLevel = Integer.parseInt(args[2]);
            if (startLevel < 1) {
                throw new MojoExecutionException("Bundle configuration start level is stricly lower than 1: "
                        + StringUtils.join(args, ":"));
            }
            if (args.length >= 4) {
                toStart = Boolean.parseBoolean(args[3]);
            } else {
                toStart = true;
            }
            if (args.length >= 6) {
                comment = Boolean.parseBoolean(args[5]);
            } else {
                comment = false;
            }
            boolean deploymentPlugin = false;
            if (args.length >= 7) {
                deploymentPlugin = Boolean.parseBoolean(args[6]);
            }
            if (args.length >= 8) {
                oneTime = Boolean.parseBoolean(args[7]);
            } else {
                oneTime = false;
            }
            if (artifact != null) {
                // Load manifest read values and check content
                JarInputStream jarStream = new JarInputStream(new FileInputStream(artifact.getFile()));
                Attributes attributes = jarStream.getManifest().getMainAttributes();
                checkManifest(attributes);

                id = attributes.getValue("Bundle-SymbolicName");
                int semicolonIndex = id.indexOf(';');
                if (semicolonIndex != -1) {
                    id = id.substring(0, semicolonIndex);
                }
                namespace = "osgiBundleSymbolicName";
                name = attributes.getValue("Bundle-Name");
                if (deploymentPlugin) {
                    installationType = "deploymentPluginBundle";
                } else {
                    installationType = "osgiBundle";
                }
                version = attributes.getValue("Bundle-Version");
            }
        } catch (NumberFormatException e) {
            throw new MojoExecutionException(
                    "Bundle configuration start level is not a number: " + StringUtils.join(args, ":"), e);
        } catch (IOException e) {
            throw new MojoExecutionException("A problem occured while reading jar from dependency: "
                    + StringUtils.join(args, ":"), e);
        }
    }

    private void checkManifest(Attributes attributes) throws MojoExecutionException {
        String bundleManifest = attributes.getValue("Bundle-ManifestVersion");
        String bundleSymbolicName = attributes.getValue("Bundle-SymbolicName");
        String bundleVersion = attributes.getValue("Bundle-Version");

        String bundleName = attributes.getValue("Bundle-Name");
        String bundleLicense = attributes.getValue("Bundle-License");
        String bundleVendor = attributes.getValue("Bundle-Vendor");

        String exportPackage = attributes.getValue("Export-Package");
        String importPackage = attributes.getValue("Import-Package");
        if (bundleManifest == null) {
            throw new MojoExecutionException("Bundle manifest does not have Bundle-ManifestVersion (" + artifact.getGroupId()
                    + ":" + artifact.getArtifactId() + ")");
        } else if (!"2".equals(bundleManifest)) {
            throw new MojoExecutionException("Bundle manifest does not have Bundle-ManifestVersion equals to 2 ("
                    + artifact.getGroupId() + ":" + artifact.getArtifactId() + "): " + bundleManifest);
        }
        if (bundleSymbolicName == null) {
            throw new MojoExecutionException("Bundle manifest does not have Bundle-SymbolicName (" + artifact.getGroupId()
                    + ":" + artifact.getArtifactId() + ")");
        }
        if (bundleVersion == null) {
            throw new MojoExecutionException("Bundle manifest does not have Bundle-Version(" + artifact.getGroupId() + ":"
                    + artifact.getArtifactId() + ")");
        }

        if (bundleName == null) {
            log.warn("Bundle manifest does not have Bundle-Name (" + artifact.getGroupId() + ":" + artifact.getArtifactId()
                    + ")");
        }
        if (bundleLicense == null) {
            log.warn("Bundle manifest does not have Bundle-License (" + artifact.getGroupId() + ":" + artifact.getArtifactId()
                    + ")");
        }
        if (bundleVendor == null) {
            log.warn("Bundle manifest does not have Bundle-Vendor (" + artifact.getGroupId() + ":" + artifact.getArtifactId()
                    + ")");
        }

        if (exportPackage != null) {
            List<String> exportPackageDecls = buildPackageList(exportPackage);
            for (String exportPackageDecl : exportPackageDecls) {
                log.debug("Check export-package: " + exportPackageDecl);
                if (!exportPackageDecl.matches(".*;[ ]*version[ ]*=.*")
                        && !exportPackageDecl.matches(".*;[ ]*specification-version[ ]*=.*")) {
                    log.warn("Bundle Export-Package declaration does not have version (" + artifact.getGroupId() + ":"
                            + artifact.getArtifactId() + "): " + exportPackageDecl);
                }
            }
        }
        if (importPackage != null) {
            List<String> importPackageDecls = buildPackageList(importPackage);
            for (String importPackageDecl : importPackageDecls) {
                log.debug("Check import-package: " + importPackageDecl);
                if (!importPackageDecl.matches(".*;[ ]*version[ ]*=.*")
                        && !importPackageDecl.matches(".*;[ ]*specification-version[ ]*=.*")) {
                    log.warn("Bundle Import-Package declaration does not have version (" + artifact.getGroupId() + ":"
                            + artifact.getArtifactId() + "): " + importPackageDecl);
                }
            }
        }
    }

    private List<String> buildPackageList(String packages) {
        List<String> result = new ArrayList<String>();
        StringReader reader = new StringReader(packages);
        String readValue = reader.readUntil(TOKENS);
        try {
            while (!reader.isTerminated()) {
                if (reader.readCurrent() == '"') {
                    reader.skipOffset(1);
                    readValue = readValue + "\"";
                    readValue = readValue + reader.readUntil('"');
                    readValue = readValue + "\"";
                    reader.skipOffset(1);
                } else {
                    reader.skipOffset(1);
                    result.add(readValue);
                    readValue = null;
                }
                if (readValue == null) {
                    readValue = reader.readUntil(TOKENS);
                } else {
                    readValue = readValue + reader.readUntil(TOKENS);
                }
            }
        } catch (ParseException e) {
            // Ignore
        }
        if (readValue != null) {
            result.add(readValue);
        }

        return result;
    }

    public int getStartLevel() {
        return startLevel;
    }

    public boolean toStart() {
        return toStart;
    }
}
