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
 * id $Id: DiaOpt.java 3327 2012-10-22 08:29:34Z JReich $
 * author $Author: JReich $
 * version $Revision: 3327 $
 * lastrevision $Date: 2012-10-22 10:29:34 +0200 (Mon, 22 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-22 10:29:34 +0200 (Mon, 22 Oct 2012) $
 */

package com.actility.m2m.song.binding.coap.jni.impl;

import com.actility.m2m.song.binding.coap.ni.api.DiaOpt;

public final class DiaOptImpl implements DiaOpt {
    String name;
    String val;

    public DiaOptImpl(String n, String v) {
        name = n;
        val = v;
    }
}
