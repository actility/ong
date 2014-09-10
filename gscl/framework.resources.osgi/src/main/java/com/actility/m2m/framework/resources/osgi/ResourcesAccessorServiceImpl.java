/*
 * Copyright   Actility, SA. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 *
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.framework.resources.osgi;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public class ResourcesAccessorServiceImpl implements ResourcesAccessorService {
    public InputStream getResourceAsStream(Bundle bundle, String path) throws IOException {
        if ((bundle.getEntry(path) != null)) {
            return bundle.getEntry(path).openStream();
        }
        throw new FileNotFoundException("Specified resource does not exist (symbolicName: " + bundle.getSymbolicName()
                + ") (path: " + path + ")");
    }

    public String getProperty(BundleContext context, String propertyName) {
        return context.getProperty(propertyName);
    }
}
