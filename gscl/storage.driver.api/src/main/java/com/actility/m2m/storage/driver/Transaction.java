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
 * id $Id: Transaction.java 8031 2014-03-07 17:31:11Z JReich $
 * author $Author: JReich $
 * version $Revision: 8031 $
 * lastrevision $Date: 2014-03-07 18:31:11 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 18:31:11 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.storage.driver;

import java.sql.SQLException;
import java.util.Collection;
import java.util.Map;

import com.actility.m2m.storage.StorageException;


public interface Transaction {

	/**
	 * Raises an exception if transaction cannot be committed
	 * @throws SQLException if transaction cannot be committed
	 */
	public void commitTransaction() throws StorageException;

	/**
	 * Raises an exception if transaction cannot be rollbacked
	 * @throws SQLException if transaction cannot be rollbacked
	 */
	public void rollbackTransaction() throws StorageException;

	/**
	 * Updates a existing resource. Raises an an exception if the document
	 * does not exist.
	 * @param path the full path of the resource (relative to a root)
	 * @param rawDocument new value to set in the resource
	 * @param config storage configuration to apply
	 * @throws SQLException if the document does not exist
	 * @throws StorageException
	 */
	public void update(String path, byte[] rawDocument, Map config) throws StorageException;

	/**
	 * Updates a existing resource. Replaces all the searchAttributes unless any
	 * searchAttributes are not specified. In that case, searchAttributes are
	 * left untouched. Raises an an exception if the document does not exist.
	 * @param path the full path of the resource (relative to a root)
	 * @param rawDocument new value to set in the resource
	 * @param searchAttributes a collection containing instances of com.actility.util.Pair
	 * @param config storage configuration to apply
	 * @throws StorageException if the document does not exist
	 */
	public void update(String path, byte[]rawDocument, Collection searchAttributes, Map config) throws StorageException;

	/**
	 * Creates a new resource. The created document has path/doc for path.
	 * By default No search attributes are defined. Raises an exception if
	 * the document already exists.
	 * @param path the full path of the resource (relative to a root)
	 * @param rawDocument a dictionary of keys
	 * @param config storage configuration to apply
	 * @throws StorageException if the document already exists
	 */
	public void create(String path, byte[] rawDocument, Map config) throws StorageException ;

	/**
	 * Creates a new resource. The created document has path/doc for path.
	 * By default No search attributes are defined. Raises an exception if
	 * the document already exists.
	 * @param path the full path of the resource (relative to a root)
	 * @param rawDocument a dictionary of keys
	 * @param searchAttributes a dictionary of keys
	 * @param config storage configuration to apply
	 * @throws StorageException if the document already exists
	 */
	public void create(String path, byte[] rawDocument, Collection searchAttributes, Map config) throws StorageException;

	/**
	 * Erases a resource. Raises an an exception if the document does not exist.
	 * @param path the path of the resource (relative to a root)
	 * @param config storage configuration to apply
	 * @throws StorageException if the document does not exist
	 */
	public void delete(String path, Map config) throws StorageException;

	/**
	 * Erases a resource. If cascading is specified to oneLevel, a resource and
	 * all its subresources are deleted. Raises an an exception if the document
	 * does not exist.
	 * @param path the the path of the resource (relative to a root)
	 * @param cascade a constant ONE_LEVEL or NONE
	 * @param config storage configuration to apply
	 * @throws StorageException if the document does not exist
	 */
	public void delete(String path, int cascade, Map config) throws StorageException;
}
