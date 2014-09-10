package com.actility.m2m.maven.plugin.apu;

import java.io.IOException;
import java.io.Writer;

/**
 *
 * Copyright 2004 The Apache Software Foundation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


/**
 * Holds the data of a jar Control.
 * <p/>
 * Controls are processed according to the
 * {@link <a href="http://java.sun.com/j2se/1.4/docs/guide/jar/jar.html">Jar file specification.</a>}. Specifically, a Control
 * element consists of a set of attributes and sections. These sections in turn may contain attributes. Note in particular that
 * this may result in Control lines greater than 72 bytes being wrapped and continued on the next line. If an application can
 * not handle the continuation mechanism, it is a defect in the application, not this task.
 *
 * @since Ant 1.4
 */
public class Control {

//    /**
//     * The max length of a line in a Control ?
//     */
//    private static final int MAX_LINE_LENGTH = 72;

    private static final String EOL = "\n";

    private static final String FD_PACKAGE = "Package";
    private static final String FD_SOURCE = "Source";
    private static final String FD_VERSION = "Version";
    private static final String FD_SECTION = "Section";
    private static final String FD_PRIORITY = "Priority";
    private static final String FD_ARCHITECTURE = "Architecture";
    private static final String FD_ESSENTIAL = "Essential";
    private static final String FD_DEPENDS = "Depends";
    private static final String FD_INSTALLED_SIZE = "Installed-Size";
    private static final String FD_MAINTAINER = "Maintainer";
    private static final String FD_DESCRIPTION = "Description";
    private static final String FD_HOMEPAGE = "Homepage";

    private String packageName;
    private String source;
    private String version;
    private String section;
    private String priority;
    private String architecture;
    private String essential;
    private String depends;
    private String installedSize;
    private String maintainer;
    private String description;
    private String homepage;

    public String getPackageName() {
        return packageName;
    }

    public void setPackageName(String packageName) {
        this.packageName = packageName;
    }

    public String getSource() {
        return source;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public String getSection() {
        return section;
    }

    public void setSection(String section) {
        this.section = section;
    }

    public String getPriority() {
        return priority;
    }

    public void setPriority(String priority) {
        this.priority = priority;
    }

    public String getArchitecture() {
        return architecture;
    }

    public void setArchitecture(String architecture) {
        this.architecture = architecture;
    }

    public String getEssential() {
        return essential;
    }

    public void setEssential(String essential) {
        this.essential = essential;
    }

    public String getDepends() {
        return depends;
    }

    public void setDepends(String depends) {
        this.depends = depends;
    }

    public String getInstalledSize() {
        return installedSize;
    }

    public void setInstalledSize(String installedSize) {
        this.installedSize = installedSize;
    }

    public String getMaintainer() {
        return maintainer;
    }

    public void setMaintainer(String maintainer) {
        this.maintainer = maintainer;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getHomepage() {
        return homepage;
    }

    public void setHomepage(String homepage) {
        this.homepage = homepage;
    }

    public void dump(Writer writer) throws IOException {
        writer.write(FD_PACKAGE + ": " + packageName + "\n");
        if (source != null) {
            writer.write(FD_SOURCE + ": " + source + "\n");
        }
        writer.write(FD_VERSION + ": " + version + "\n");
        if (section != null) {
            writer.write(FD_SECTION + ": " + section + "\n");
        }
        if (priority != null) {
            writer.write(FD_PRIORITY + ": " + priority + "\n");
        }
        writer.write(FD_ARCHITECTURE + ": " + architecture + "\n");
        if (essential != null) {
            writer.write(FD_ESSENTIAL + ": " + essential + "\n");
        }
        if (depends != null) {
            writer.write(FD_DEPENDS + ": " + depends + "\n");
        }
        if (installedSize != null) {
            writer.write(FD_INSTALLED_SIZE + ": " + installedSize + "\n");
        }
        writer.write(FD_MAINTAINER + ": " + maintainer + "\n");
        writer.write(FD_DESCRIPTION + ": " + description + "\n");
        if (homepage != null) {
            writer.write(FD_HOMEPAGE + ": " + homepage + "\n");
        }
    }
}
