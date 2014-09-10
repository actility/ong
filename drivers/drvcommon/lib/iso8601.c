
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




/**
 * @brief Parse the provided string as an ISO8601 duration.
 * Note that this function only support duration in days, hours, minutes and seconds.
 * Years and month are out of the scope for now.
 * @param string the string to parse.
 * @return the parsed duration in seconds, or 0 if a parse error happens.
 */
int parseISO8601Duration(char * string)
{
  short sign = 1;
  int days = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  int tmp = 0;
  int err = 0;

  if (!string) return 0;

  if ('+' == *string)
  {
    string++;
  }
  else if ('-' == *string)
  { 
    sign = -1;
    string++;
  }
  // else positive duration

  if ('P' != *string) return 0;
  string++;

  while ((*string != '\0') && (!err))
  {
    tmp = 0;
    while ((*string != '\0') && ('0' <= *string) && ('9' >= *string))
    {
      tmp = (10 * tmp) + (*string - '0');
      string++;
    }
    switch (*string)
    {
      case 'D':
        if (days) err = 1;
        else days = tmp;
        break;
      case 'T':
        if (hours || minutes || seconds) err = 1;
        break;
      case 'H':
        if (hours) err = 1;
        else hours = tmp;
        break;
      case 'M':
        if (minutes) err = 1;
        else minutes = tmp;
        break;
      case 'S':
        if (seconds) err = 1;
        else seconds = tmp;
        break;
      default:
        err = 1;
        break;
    }
    string++; 
  } 
  
  if (err) return 0;
  return (sign * ( (days * 86400) + ( hours * 3600) + (minutes * 60) + seconds ));
}


