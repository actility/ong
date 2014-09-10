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

package com.actility.m2m.m2m.impl;

import java.text.ParseException;
import java.util.HashMap;
import java.util.Map;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MUtils;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.util.ArrayUtils;
import com.actility.m2m.util.CharacterUtils;
import com.actility.m2m.util.StringReader;
import com.actility.m2m.xo.XoService;

public class M2MUtilsImpl implements M2MUtils {

    private static final long L_ACCEPT_STOP_1 = CharacterUtils.lowMask(" ,;");
    private static final long H_ACCEPT_STOP_1 = CharacterUtils.highMask(" ,;");

    private static final long L_ACCEPT_STOP_2 = CharacterUtils.lowMask(" =");
    private static final long H_ACCEPT_STOP_2 = CharacterUtils.highMask(" =");

    private final String[] availableXoMediaTypes;

    public M2MUtilsImpl(XoService xoService) {
        String[] availableMediaTypes = null;
        if (xoService.isXmlSupported() && xoService.isExiSupported()) {
            availableMediaTypes = new String[] { M2MConstants.MT_APPLICATION_XML, M2MConstants.MT_APPLICATION_EXI };
        } else if (xoService.isXmlSupported()) {
            availableMediaTypes = new String[] { M2MConstants.MT_APPLICATION_XML };
        } else if (xoService.isExiSupported()) {
            availableMediaTypes = new String[] { M2MConstants.MT_APPLICATION_EXI };
        }
        this.availableXoMediaTypes = availableMediaTypes;
    }

    public String[] getAvailableXoMediaTypes() {
        return availableXoMediaTypes;
    }

    private Map extractAccept(String accept) throws M2MException {
        Map mediaTypeToWeight = new HashMap();
        try {
            StringReader reader = new StringReader(accept);
            char current = '0';
            boolean added = false;
            do {
                added = false;
                reader.skip(' ');
                String mediaType = reader.readUntil(L_ACCEPT_STOP_1, H_ACCEPT_STOP_1);
                if (!reader.isTerminated()) {
                    current = reader.readCurrent();
                    if (current == ' ') {
                        reader.skip(' ');
                    }
                    if (!reader.isTerminated()) {
                        current = reader.readCurrentAndSkip();
                        while (current == ';') {
                            reader.skip(' ');
                            String param = reader.readUntil(L_ACCEPT_STOP_2, H_ACCEPT_STOP_2);
                            reader.skip(' ');
                            reader.checkCurrentAndSkip('=');
                            reader.skip(' ');
                            String value = reader.readUntil(L_ACCEPT_STOP_1, H_ACCEPT_STOP_1);
                            reader.skip(' ');
                            if (!reader.isTerminated()) {
                                current = reader.readCurrentAndSkip();
                            } else {
                                current = '0';
                            }
                            if ("q".equals(param)) {
                                added = true;
                                mediaTypeToWeight.put(mediaType, Double.valueOf(value));
                            }
                        }
                        if ((reader.isTerminated() || current == ',') && !added) {
                            mediaTypeToWeight.put(mediaType, new Double(1));
                        }
                    } else {
                        mediaTypeToWeight.put(mediaType, new Double(1));
                    }
                } else {
                    mediaTypeToWeight.put(mediaType, new Double(1));
                }
            } while (current == ',');
        } catch (ParseException e) {
            throw new M2MException("Cannot parse accept header: " + accept, StatusCode.STATUS_BAD_REQUEST, e);
        }

        return mediaTypeToWeight;
    }

    public String getAcceptedXoMediaType(String accept) throws M2MException {
        return getAcceptedMediaType(accept, availableXoMediaTypes);
    }

    public String getAcceptedMediaType(String accept, String[] acceptedMediaTypes) throws M2MException {
        double bestWeight = -1;
        String mediaType = null;

        if (accept != null) {
            Map mediaTypeToWeight = extractAccept(accept);
            for (int i = 0; i < acceptedMediaTypes.length; ++i) {
                Double weight = (Double) mediaTypeToWeight.get(acceptedMediaTypes[i]);
                if (weight != null && weight.doubleValue() > bestWeight) {
                    bestWeight = weight.doubleValue();
                    mediaType = acceptedMediaTypes[i];
                } else {
                    int index = acceptedMediaTypes[i].indexOf('/');
                    if (index != -1) {
                        weight = (Double) mediaTypeToWeight.get(acceptedMediaTypes[i].substring(0, index + 1) + "*");
                        if (weight != null && weight.doubleValue() > bestWeight) {
                            bestWeight = weight.doubleValue();
                            mediaType = acceptedMediaTypes[i];
                        } else {
                            weight = (Double) mediaTypeToWeight.get("*/*");
                            if (weight != null && weight.doubleValue() > bestWeight) {
                                bestWeight = weight.doubleValue();
                                mediaType = acceptedMediaTypes[i];
                            }
                        }
                    } else {
                        weight = (Double) mediaTypeToWeight.get("*/*");
                        if (weight != null && weight.doubleValue() > bestWeight) {
                            bestWeight = weight.doubleValue();
                            mediaType = acceptedMediaTypes[i];
                        }
                    }
                }
            }
        } else {
            mediaType = acceptedMediaTypes[0];
        }
        if (mediaType == null) {
            throw new M2MException("Only following media types are supported: "
                    + ArrayUtils.toString(acceptedMediaTypes, "[ ", " ]", ", "), StatusCode.STATUS_NOT_ACCEPTABLE);
        }

        return mediaType;
    }
}
