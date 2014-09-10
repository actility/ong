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
 * id $Id: ExtSongBindingService.java 6084 2013-10-15 13:54:23Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6084 $
 * lastrevision $Date: 2013-10-15 15:54:23 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:54:23 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.service.ext;

import java.util.Iterator;

import com.actility.m2m.servlet.song.ext.SongNode;
import com.actility.m2m.servlet.song.service.SongBindingService;

/**
 * Extensions the {@link SongBindingService} used to access internals of the container.
 */
public interface ExtSongBindingService extends SongBindingService {

    /**
     * Gets an iterator on {@link java.util.Map.Entry} mapping the a binding protocol to a {@link SongNode}
     *
     * @return An iterator on entries mapping the binding protocol to a {@link SongNode}
     */
    Iterator getSchemeRoutes();
}
