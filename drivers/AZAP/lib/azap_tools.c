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

//************************************************************************
//************************************************************************
//**
//** File : ./lib/azap_tools.c
//**
//** Description  :
//**
//**
//** Date : 11/19/2012
//** Author :  Mathias Louiset, Actility (c), 2012.
//** Modified by :
//**
//** Task :
//**
//** REVISION HISTORY:
//**
//**  Date      Version   Name
//**  --------    -------   ------------
//**
//************************************************************************
//************************************************************************


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "azap_types.h"
#include "azap_tools.h"

/*   ___ _       _              __                    _   _   _
 *  / __| |_ _ _(_)_ _  __ _   / _|___ _ _ _ __  __ _| |_| |_(_)_ _  __ _
 *  \__ \  _| '_| | ' \/ _` | |  _/ _ \ '_| '  \/ _` |  _|  _| | ' \/ _` |
 *  |___/\__|_| |_|_||_\__, | |_| \___/_| |_|_|_\__,_|\__|\__|_|_||_\__, |
 *                     |___/                                        |___/
 */

// WARNING! these methods are not thread safe, and only works because the program is
// mono-threaded.
// before using it in concurrent context, one had to add critical sections

static char str_rep [5][STR_MAX_SIZE];
static azap_uint8 str_rep_i = 0;

/**
 * @brief Build a string reprepentation of a unsigned 8-bits integer.
 * This method defines 5 static buffers to allow using it 5 times
 * at most within the same trace call.
 * @param data the value to represent
 * @param hexprefix add "0x" as prefix
 * @return the resulting string reprepentation.
 */
char *azap_uint8ToString(azap_uint8 data, bool hexprefix)
{
  str_rep_i ++;
  if (str_rep_i >= 5)
  {
    str_rep_i = 0;
  }

  memset(str_rep[str_rep_i], 0, STR_MAX_SIZE);
  if (hexprefix)
  {
    sprintf(str_rep[str_rep_i], "0x%.2x", data);
  }
  else
  {
    sprintf(str_rep[str_rep_i], "%.2x", data);
  }
  return str_rep[str_rep_i];
}

/**
 * @brief Build a string reprepentation of a unsigned 16-bits integer.
 * This method defines 5 static buffers to allow using it 5 times
 * at most within the same trace call.
 * @param data the value to represent
 * @param hexprefix add "0x" as prefix
 * @return the resulting string reprepentation.
 */
char *azap_uint16ToString(azap_uint16 data, bool hexprefix)
{
  str_rep_i ++;
  if (str_rep_i >= 5)
  {
    str_rep_i = 0;
  }

  memset(str_rep[str_rep_i], 0, STR_MAX_SIZE);
  if (hexprefix)
  {
    sprintf(str_rep[str_rep_i], "0x%.4x", data);
  }
  else
  {
    sprintf(str_rep[str_rep_i], "%.4x", data);
  }
  return str_rep[str_rep_i];
}


/**
 * @brief Build a string reprepentation of a extended 802.15.4 IEEE address.
 * This method defines 5 static buffers to allow using it 5 times
 * at most within the same trace call.
 * @param data the byte set to represent
 * @param hexprefix add "0x" as prefix
 * @return the resulting string reprepentation.
 */
char *azap_extAddrToString(azap_uint8 *data, bool hexprefix)
{
  short i;
  char tmp[6];

  str_rep_i ++;
  if (str_rep_i >= 5)
  {
    str_rep_i = 0;
  }

  memset(str_rep[str_rep_i], 0, STR_MAX_SIZE);

  if (data)
  {
    if (hexprefix)
    {
      sprintf(str_rep[str_rep_i], "0x");
    }
    else
    {
      str_rep[str_rep_i][0] = 0;
    }
    // encoding target is big endian
    for (i = IEEE_ADDR_LEN - 1; i >= 0; i--)
    {
      sprintf(tmp, "%.2x", data[i]);
      strcat(str_rep[str_rep_i], tmp);
    }
  }

  return str_rep[str_rep_i];

}

/**
 * @brief Convert a string representation of a extended address into a IEEE address.
 * The resulting IEEE address is here represented by a table of eight usigned 8-bit integer.
 * @param extAddr the result is stored into this parameter if the string can be parsed
 * successfully.
 * @param strRep the string representation of the IEEE address to parse and convert.
 * @return 1 if conversion succeeds, 0 otherwise.
 */
bool stringToAzap_extAddr(azap_uint8 *extAddr, char *strRep)
{
  azap_uint8 i;
  if (!extAddr)
  {
    return false;
  }
  if (!strRep)
  {
    return false;
  }
  if (18 != strlen(strRep))
  {
    return false;
  }
  if (0 != strncmp(strRep, "0x", 2))
  {
    return false;
  }

  strRep += 2;
  for (i = 0; i < IEEE_ADDR_LEN; i++)
  {
    unsigned int val;
    sscanf(strRep, "%2x", &val);
    extAddr[IEEE_ADDR_LEN - i - 1] = (azap_uint8)val;
    strRep += 2;
  }
  return true;
}

/**
 * @brief Build a string reprepentation of the x first bytes.
 * This method defines 5 static buffers to allow using it 5 times
 * at most within the same trace call.
 * @param data the byte set to represent
 * @param len the number of bytes to represent
 * @return the resulting string reprepentation.
 */
char *azap_byteArrayToString(azap_byte *data, azap_uint16 len)
{
  char tmp[6];
  azap_uint16 i;
  azap_uint16 s = 0;

  str_rep_i ++;
  if (str_rep_i >= 5)
  {
    str_rep_i = 0;
  }

  memset(str_rep[str_rep_i], 0, STR_MAX_SIZE);

  if (data && len)
  {
    sprintf(str_rep[str_rep_i], "0x");
    s += 2;
    for (i = 0; (i < len) && (s < STR_MAX_SIZE - 3); i++)
    {
      sprintf(tmp, "%.2x", data[i]);
      strcat(str_rep[str_rep_i], tmp);
      s += 2;
    }
    str_rep[str_rep_i][s] = 0;
  }
  return str_rep[str_rep_i];
}

/**
 * @brief Convert a string representation of a byte array into a byte array.
 * @param data the result is stored into this parameter if the string can be parsed
 * successfully.
 * @param len the size of data.
 * @param strRep the string representation of the byte array to parse and convert.
 * @return the number of read bytes if conversion succeeds, 0 otherwise.
 */
azap_uint16 stringToAzap_byteArray(azap_byte *data, azap_uint16 len, char *strRep)
{
  azap_uint16 l;
  azap_uint16 i;
  if (!data)
  {
    return 0;
  }
  if (!strRep)
  {
    return 0;
  }
  l = ( (strlen(strRep) - 2) / 2);
  if (len < l)
  {
    return 0;
  }
  if (0 != strncmp(strRep, "0x", 2))
  {
    return 0;
  }

  strRep += 2;
  for (i = 0; i < l; i++)
  {
    unsigned int val;
    sscanf(strRep, "%2x", &val);
    data[i] = (azap_byte)val;
    strRep += 2;
  }
  return l;
}


/**
 * @brief Build a string reprepentation of a list if azap_uint8.
 * @param array array of azap_uint8 that constitute this list.
 * @param len number of attributes in the list
 * @return the built string reprepentation
 */
char *azap_uint8ArrayToString(azap_uint8 *array, azap_uint16 len)
{
  char tmp[6];
  azap_uint16 i;
  azap_uint16 s = 0;

  str_rep_i ++;
  if (str_rep_i >= 5)
  {
    str_rep_i = 0;
  }

  memset(str_rep[str_rep_i], 0, STR_MAX_SIZE);

  if (array && len)
  {
    for (i = 0; (i < len) && (s < STR_MAX_SIZE - 5); i++)
    {
      sprintf(tmp, "0x%.2x ", array[i]);
      strcat(str_rep[str_rep_i], tmp);
      s += 5;
    }
    str_rep[str_rep_i][s - 1] = 0; // remove trailing space
  }
  return str_rep[str_rep_i];
}

/**
 * @brief Build a string reprepentation of a list of azap_uint16.
 * @param array array of azap_uint16 that constitute this list.
 * @param len number of attributes in the list
 * @return the built string reprepentation
 */
char *azap_uint16ArrayToString(azap_uint16 *array, azap_uint16 len)
{
  char szTemp[8];
  azap_uint16 size = 0;
  azap_uint16 i;

  str_rep_i ++;
  if (str_rep_i >= 5)
  {
    str_rep_i = 0;
  }

  memset(str_rep[str_rep_i], 0, STR_MAX_SIZE);
  if (array)
  {
    for (i = 0; (i < len) && (size < STR_MAX_SIZE - 7); i++)
    {
      snprintf(szTemp, 8, "0x%.4x ", array[i]);
      strcat(str_rep[str_rep_i], szTemp);
      size += 7;
    }
    str_rep[str_rep_i][size - 1] = 0; // remove trailing space
  }

  return str_rep[str_rep_i];
}

/**
 * @brief Determine if an extended address is null.
 * A extended address is said "null" if the pointer to this address equals NULL or
 * the 8 bytes of the extended address are set to 0.
 * @param extAddr the extended address we are looking at.
 * @return 1 if the provided IEEE address is nil, 0 otherwise.
 */
bool isExtAddrNull(azap_uint8 *extAddr)
{
  static azap_uint8 nullAddr[IEEE_ADDR_LEN] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (NULL == extAddr)
  {
    return true;
  }
  if (0 == memcmp(extAddr, nullAddr, IEEE_ADDR_LEN))
  {
    return true;
  }

  return false;
}


