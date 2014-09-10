
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
 * id $Id$                                                            
 * author $Author$                                                    
 * version $Revision$                                                 
 * lastrevision $Date$                                                
 * modifiedby $LastChangedBy$                                         
 * lastmodified $LastChangedDate$                                     
 */                                                                   


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h> /* POSIX terminal control definitions */
#include <poll.h>
#include <stddef.h>

#define TTY_SCAN_ROOT_DIR "/sys/class/tty/"
#define SMALL_BUF_SIZE 32
#define BIG_BUF_SIZE 255
/**
 * @fn static bool getTTYMappedName(azap_uint8 * targetedIeee, char *bindingDevice)
 * @brief Scan the USB devices plugged on the system and detect which one is CC2531 dongle.
 * WARNING: the function works on linux running kernel 2.6.18, but there is no warranty on
 * other systems.
 * @param bindingDevice if CC2531 dongle found, provides with the mapped TTY name.
 * @return 1 if CC2531 dongle found, 0 otherwise. 
 */
int getTTYMappedName(char * ttyPrefix, char * productId, char *bindingDevice)
{
  int rc;
  char deviceLink[BIG_BUF_SIZE];
  char devicePath[BIG_BUF_SIZE];
  char readBuf[SMALL_BUF_SIZE];
  char productPath[BIG_BUF_SIZE];
  FILE * product;
  
  DIR * ttyDir;
  int res = 0;
  struct dirent * currentEntry;
  char * searchString;

  if (!bindingDevice)
  {
    return 0;
  }

  if (strcmp(ttyPrefix,"ttyUSB")==0)
  {
      searchString="%s%s/%s/../../product";
  }
  else //ttyACM
  { 
      searchString="%s%s/%s/../product";
  }

  
  sprintf(bindingDevice, "<unknown>");
  
  ttyDir = opendir(TTY_SCAN_ROOT_DIR);
  if (ttyDir)
  {
    while ((currentEntry = readdir(ttyDir)))
    {
      //printf("scanning %s (type:%d) (expected:%d)\n", currentEntry->d_name, currentEntry->d_type, DT_DIR);
      //printf("searching %s\n",ttyPrefix);
      // scan /sys/class/tty/ content, looking for ttyACM* sub-dirs
      if (
          (!strncmp(currentEntry->d_name, ttyPrefix, strlen(ttyPrefix))))
      {
        snprintf(devicePath, sizeof(devicePath), "%s%s/device", TTY_SCAN_ROOT_DIR,
          currentEntry->d_name);
        rc = readlink(devicePath, deviceLink, sizeof(deviceLink));
        if (rc > 0)
        {
          deviceLink[rc] = 0;
          // if "device" symbolic link is valid, look for the product description
          snprintf(productPath, sizeof(productPath), searchString, TTY_SCAN_ROOT_DIR,
            currentEntry->d_name, deviceLink);
          //printf("product path is : %s\n",productPath);
          product = fopen(productPath, "r");
          if (product)
          {
            rc = fread(readBuf, sizeof(char), sizeof(readBuf), product);
            //printf("read in product file: <%s>\n", readBuf);
            if ((rc > 0) &&
                (!strncmp(readBuf, productId, strlen(productId))))
            {
              // The product description matches and the serial number is ignored
              sprintf(bindingDevice, "/dev/%s", currentEntry->d_name);
              //printf("-------------found-->%s",bindingDevice);
              res = 1;
            }
            else if (-1 == rc)
            {
              fprintf(stderr, "getTTYMappedName - Failed to read %s (errno:%d) "
                "(err:%s)\n", productPath, errno, strerror(errno));
            }
            
            if (0 != fclose(product))
            {
              fprintf(stderr, "getTTYMappedName - Failed to close %s (errno:%d) "
                "(err:%s)\n", productPath, errno, strerror(errno));
            }
          }
          else
          {
            fprintf(stderr, "getTTYMappedName - Failed to open %s (errno:%d) (err:%s)\n", 
              productPath, errno, strerror(errno));
          }
        }
        else
        {
          fprintf(stderr, "getTTYMappedName - Failed to read link %s (errno:%d) "
            "(err:%s)\n", devicePath, errno, strerror(errno));
        }
      }
    }
    
    if (0 != closedir(ttyDir))
    {
      fprintf(stderr, "getTTYMappedName - Failed to close %s (errno:%d) (err:%s)\n", 
        TTY_SCAN_ROOT_DIR, errno, strerror(errno));
    }
  }
  else
  {
    fprintf(stderr, "getTTYMappedName - Failed to open %s (errno:%d) (err:%s)\n", 
      TTY_SCAN_ROOT_DIR, errno, strerror(errno));
  }

  return res;
}

int main(int argc, char * argv[])
{
  if (argc != 3)
  {
    printf("bad arguments \n");
    printf("--------------------------------------------------\n");
    printf("Usage : tty_mapper Product\n");
    printf("--------------------------------------------------\n");
    printf("Return the first matching tty associated to a specific productId\n");
    printf("Product can be found in dmesg or /var/log/messages\n");
    printf("examples: \n");
    printf(" - localhost kernel: usb 2-1.3: \n");
    printf("   New USB device strings: Mfr=1, Product=2, SerialNumber=3\n");
    printf("          localhost kernel: usb 2-1.3: Product: SAMSUNG_Android\n");
    printf("    command is : tty_mapper \"ttyUSB\" \"SAMSUNG_Android\"\n");

    exit(1);
  }
  char res[BIG_BUF_SIZE];
  getTTYMappedName(argv[1],argv[2], res);
  printf("%s\n", res);

  return 0;
}

