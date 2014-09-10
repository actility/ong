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
/* options.c --
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the dIa library libdIa. Please see
 * README for terms of use.
 */

#include <coap.h>


static coap_list_t *optlist = NULL;


static int
order_opts(void *a, void *b) {
	if (!a || !b)
		return a < b ? -1 : 1;
	if (COAP_OPTION_KEY(*(coap_option *)a) < COAP_OPTION_KEY(*(coap_option *)b))
		return -1;
	return COAP_OPTION_KEY(*(coap_option *)a) == COAP_OPTION_KEY(*(coap_option *)b);
}

static coap_list_t *
new_option_node(unsigned short key, unsigned char *data, unsigned int length) {
	coap_option *option;
	coap_list_t *node;

	option = coap_malloc(sizeof(coap_option) + length);
	if ( !option )
		return NULL;

	COAP_OPTION_KEY(*option) = key;
	COAP_OPTION_LENGTH(*option) = length;
	memcpy(COAP_OPTION_DATA(*option), data, length);

	/* we can pass NULL here as delete function since option is released automatically	*/
	node = coap_new_listnode(option, free);
	if	(!node)
		coap_free(option);
	return node;
}


//	API : handle unordered options

//	List initialization
void coap_init_options () {
	coap_delete_list (optlist);
	optlist	= NULL;
}

//	Add unordered option. It will be ordered by 'order_opts' function
void coap_prepare_option (unsigned short key, void *buf, int len) {
	coap_insert(&optlist, new_option_node(key, buf, len), order_opts);
}

//	Put ordered list into pdu
void coap_add_options(coap_pdu_t *pdu) {
	coap_list_t *option;

	for (option = optlist; option; option = option->next )
	{
		coap_add_option ( pdu, COAP_OPTION_KEY(*(coap_option *)option->data),
			COAP_OPTION_LENGTH(*(coap_option *)option->data),
			COAP_OPTION_DATA(*(coap_option *)option->data) );
	}
}
