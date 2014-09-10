
/*
* Copyright (C) Actility, SA. All Rights Reserved.
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
*/

#ifndef	RTL_HTBL_H
#define	RTL_HTBL_H

#define	HTBL_KEY_STRING		1 // key is (char *) type
#define	HTBL_KEY_UINT		2 // key is (unsigned int) type

#define	HTBL_KEY_INCLUDED	4 // key is stored in user data (not duplicate)
#define	HTBL_FREE_DATA		8 // user data is freed with free(3) if removed

#define	HTBL_USE_MUTEX		32// set a mutex on the htbl


// CENTOS gcc 4.1 HTBL_KEY_UINT|HTBL_KEY_INCLUDED does not work with key values
// greater than INT_MAX


/* hashtbl.c */
void *rtl_htblNew(unsigned int size);
void *rtl_htblCreate(unsigned int size, unsigned int (*hashfunc)(void *));
void *rtl_htblCreateSpec(unsigned int size, unsigned int (*hashfunc)(void *),unsigned int flags);
int rtl_htblGetFlags(void *hashtbl);
int rtl_htblGetCount(void *hashtbl);
int rtl_htblSetRmovFunc(void *hashtbl, void (*rmovfunc)(void *h,char *k,void *data));
int rtl_htblSetKeyOffset(void *hashtbl, int offset); // only if HTBL_KEY_INCLUDED


// rtl_htblNew(sz) == rtl_htblCreate(sz,NULL)
// rtl_htblCreate(sz,fct) == rtl_htblCreateSpec(sz,fct,0)
// if hashfunc is NULL very basic hash functions are used :
//	static unsigned int def_hashfunc(void *key)	// for HTBL_KEY_STRING
//	{
//		unsigned int hash=0;
//		unsigned char *k = (unsigned char *)key;
//		while(*k) hash+=*k++;
//		return hash;
//	}

//	static 	unsigned int uint_hashfunc(void *key)	// HTBL_KEY_UINT
//	{
//		return (unsigned int)key;
//	}
//
// rmovfunc is called each time a key is removed on user data
// use offsetof(t_data,strkey) to set parameter offset of rtl_htblSetKeyOffset

/* locked functions */
void rtl_htblDestroy(void *hashtbl);
int rtl_htblInsert(void *hashtbl, void *key, void *data);
int rtl_htblAdd(void *hashtbl, void *data); // only if HTBL_KEY_INCLUDED
int rtl_htblRemove(void *hashtbl, void *key);
void *rtl_htblGet(void *hashtbl, void *key);
int rtl_htblResize(void *hashtbl, unsigned int size);
void rtl_htblDump(void *hashtbl, void (*fctdump)(char *key, void *data));
int rtl_htblWalk(void *htbl, int (*fct)(void *h,char *key, void *data, void *udata), void *udata);

// rtl_htblAdd(h,d) == rtl_htblInsert(h,NULL,d)
// if fctwalk returns a non 0 value, walk is stopped and rtl_htblWalk returns 
// this value.
// rtl_htblRemove() can be used in fctwalk, not in fctdump

#endif
