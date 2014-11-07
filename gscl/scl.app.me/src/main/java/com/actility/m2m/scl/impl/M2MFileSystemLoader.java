package com.actility.m2m.scl.impl;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Confirm;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MContext;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.Request;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.util.FileUtils;
import com.actility.m2m.util.log.OSGiLogger;

public final class M2MFileSystemLoader {
    private static final Logger LOG = OSGiLogger.getLogger(M2MFileSystemLoader.class, BundleLogger.getStaticLogger());

    public static final String AT_FS_LOADER = "fsLoader";

    private final com.actility.m2m.m2m.M2MContext m2mContext;
    private final Map params;

    private final LinkedList creates;
    private final LinkedList updates;
    private final URI requestingEntity;
    private final int resourcesFolderLength;
    private boolean properlyCreated;

    public M2MFileSystemLoader(M2MContext m2mContext, URI nsclUri, URI tpkDevUri, URI gsclCoapBaseUri, URI gsclHttpBaseUri,
            String domainName, File resourcesFolder) throws M2MException {
        this.m2mContext = m2mContext;
        this.params = new HashMap();
        if (nsclUri != null) {
            params.put("NSCL_BASE_URI", nsclUri.toString());
        }
        if (tpkDevUri != null) {
            params.put("TPK_DEV_URI", tpkDevUri.toString());
        }
        if (gsclCoapBaseUri != null) {
            params.put("GSCL_COAP_BASE_URI", gsclCoapBaseUri.toString());
        }
        if (gsclHttpBaseUri != null) {
            params.put("GSCL_HTTP_BASE_URI", gsclHttpBaseUri.toString());
        }
        if (domainName != null) {
            params.put("DOMAIN_NAME", domainName);
        }
        params.put("SCL_CONTEXT_PATH", m2mContext.getContextPath());
        creates = new LinkedList();
        updates = new LinkedList();
        requestingEntity = m2mContext.createLocalUri(URI.create(M2MConstants.URI_SEP), Constants.PATH_ROOT);
        this.resourcesFolderLength = resourcesFolder.getAbsolutePath().length();
        registerResources(resourcesFolder, 0, false);
        Collections.sort(creates);
        Collections.sort(updates);
    }

    private void registerResources(File resourcesFolder, int level, boolean accessRight) {
        if (resourcesFolder.isDirectory()) {
            File[] subFiles = resourcesFolder.listFiles();
            for (int i = 0; i < subFiles.length; ++i) {
                if (subFiles[i].isDirectory()) {
                    registerResources(subFiles[i], level + 1, subFiles[i].getName().equals("accessRights"));
                } else if (subFiles[i].getName().endsWith("_c.xml")) {
                    creates.add(new ResourcePath(level, accessRight, subFiles[i]));
                } else if (subFiles[i].getName().endsWith("_u.xml")) {
                    updates.add(new ResourcePath(level, accessRight, subFiles[i]));
                }
            }
        }
    }

    private void popResource() {
        if (!creates.isEmpty()) {
            creates.removeFirst();
        } else if (!updates.isEmpty()) {
            updates.removeFirst();
        }
    }

    private boolean sendRequest() {
        boolean end = false;
        try {
            if (!creates.isEmpty()) {
                File resourceFile = ((ResourcePath) creates.getFirst()).getResource();
                String path = resourceFile.getAbsolutePath().substring(resourcesFolderLength,
                        resourceFile.getAbsolutePath().length() - resourceFile.getName().length());
                URI targetID = m2mContext.createLocalUri(requestingEntity, path);
                Request request = m2mContext.createRequest(M2MConstants.MD_CREATE, requestingEntity, targetID);
                request.setRawContent(generateBodyFromFile(resourceFile), "application/xml; charset=utf-8");
                request.setAttribute(AT_FS_LOADER, this);
                request.send();
            } else if (!updates.isEmpty()) {
                File resourceFile = ((ResourcePath) updates.getFirst()).getResource();
                String path = resourceFile.getAbsolutePath().substring(resourcesFolderLength,
                        resourceFile.getAbsolutePath().length() - 6)
                        + M2MConstants.URI_SEP;
                if (path.equals("/sclBase/")) {
                    path = M2MConstants.URI_SEP;
                }
                URI targetID = m2mContext.createLocalUri(requestingEntity, path);
                Request request = m2mContext.createRequest(M2MConstants.MD_UPDATE, requestingEntity, targetID);
                request.setRawContent(generateBodyFromFile(resourceFile), "application/xml; charset=utf-8");
                request.setAttribute(AT_FS_LOADER, this);
                request.send();
            } else {
                properlyCreated = true;
                end = true;
            }
        } catch (M2MException e) {
            end = true;
            LOG.error("Cannot send M2M request", e);
        } catch (IOException e) {
            end = true;
            LOG.error("Cannot send M2M request", e);
        }
        return end;
    }

    public boolean isProperlyCreated() {
        return properlyCreated;
    }

    public boolean populate() {
        return sendRequest();
    }

    public boolean doSuccessConfirm(Confirm confirm) throws M2MException {
        popResource();
        return sendRequest();
    }

    public boolean doUnsuccessConfirm(Confirm confirm) {
        if (confirm.getStatusCode().equals(StatusCode.STATUS_GATEWAY_TIMEOUT)) {
            // Retry
            return sendRequest();
        }
        return true;
    }

    private byte[] generateBodyFromFile(File file) throws IOException {
        String templateBody = FileUtils.readFomFile(file, M2MConstants.ENC_UTF8);
        String body = generateBodyFromTemplate(templateBody, params);
        return body.getBytes(M2MConstants.ENC_UTF8);
    }

    private static String generateBodyFromTemplate(String template, Map context) {
        int dollarIndex = template.indexOf('$');
        if (dollarIndex == -1) {
            return template;
        }
        int offset = 0;
        int beginIndex = 0;
        StringBuffer buffer = new StringBuffer();
        do {
            if (template.length() > dollarIndex + 4 && template.charAt(dollarIndex + 1) == '['
                    && template.charAt(dollarIndex + 2) == 'i' && template.charAt(dollarIndex + 3) == 'f'
                    && template.charAt(dollarIndex + 4) == '(') {
                // if statement
                int endParen = template.indexOf(')', dollarIndex);
                if (endParen != -1) {
                    int endBracket = template.indexOf(']', endParen);
                    if (endBracket != -1) {
                        String param = template.substring(dollarIndex + 5, endParen);
                        buffer.append(template.substring(beginIndex, dollarIndex));
                        offset = endBracket + 1;
                        beginIndex = offset;
                        if (context.get(param) != null) {
                            String subPartTemplate = template.substring(endParen + 2, endBracket);
                            buffer.append(generateBodyFromTemplate(subPartTemplate, context));
                        }
                    } else {
                        offset = endParen;
                    }
                } else {
                    offset = dollarIndex + 4;
                }
            } else if (template.length() > dollarIndex + 1 && template.charAt(dollarIndex + 1) == '{') {
                int endBracket = template.indexOf('}', dollarIndex);
                if (endBracket != -1) {
                    String param = template.substring(dollarIndex + 2, endBracket);
                    String value = (String) context.get(param);
                    buffer.append(template.substring(beginIndex, dollarIndex));
                    offset = endBracket + 1;
                    beginIndex = offset;
                    if (value != null) {
                        buffer.append(value);
                    }
                } else {
                    offset = dollarIndex + 1;
                }
            }
            dollarIndex = template.indexOf('$', offset);
        } while (dollarIndex != -1);

        buffer.append(template.substring(beginIndex, template.length()));

        return buffer.toString();
    }

    public final class ResourcePath implements Comparable {
        private final int level;
        private final boolean accessRight;
        private final File resource;

        public ResourcePath(int level, boolean accessRight, File resource) {
            this.level = level;
            this.accessRight = accessRight;
            this.resource = resource;
        }

        public File getResource() {
            return resource;
        }

        public int compareTo(Object object) {
            if (object.getClass() != ResourcePath.class) {
                return -1;
            }
            ResourcePath resourcePathLevel = (ResourcePath) object;
            if (this.level != resourcePathLevel.level) {
                return this.level - resourcePathLevel.level;
            } else if (accessRight == resourcePathLevel.accessRight) {
                return 0;
            } else if (accessRight) {
                return -1;
            }
            return 1;
        }
    }
}
