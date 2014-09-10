#include <stdlib.h>
#include <stdio.h>

#include "processor_parser.h"

#define RUN_PARSER(x)   wmbusParser(x, sizeof(x) - 1)

void
singleValue(void)
{
    // Test for No Data
    RUN_PARSER("\x00\x03");

    // Test for Interger 8 bits
    //  1
    RUN_PARSER("\x01\x03\x01");

    // Test for Interger 16 bits
    //  -2
    RUN_PARSER("\x02\x03\xFE\xFF");

    // Test for Interger 24 bits
    //  12345678
    RUN_PARSER("\x03\x03\x4E\x61\xBC");

    // Test for Interger 32 bits
    //  1234567890
    RUN_PARSER("\x04\x03\xD2\x02\x96\x49");

    // Test for Float 32 bits
    //  229.1964
    RUN_PARSER("\x05\x03\x4a\x32\x65\x43");
    
    // Test for Interger 48 bits
    //  0123456789055
    RUN_PARSER("\x06\x03\x3F\x1a\x99\xbe\x1c\x00");
    
    // Test for Interger 48 bits (Negative)
    //  -16
    RUN_PARSER("\x06\x03\xF0\xFF\xFF\xFF\xFF\xFF");
    
    // Test for Interger 64 bits
    //  1234567890123456789
    RUN_PARSER("\x07\x03\x15\x81\xE9\x7D\xF4\x10\x22\x11");

    // Test for BCD 2 Digits
    //  E-
    RUN_PARSER("\x09\x03\xEF");

    // Test for BCD 4 Digits
    //  E-12
    RUN_PARSER("\x0A\x03\xEF\x12");

    // Test for BCD 6 Digits
    //  E-1234
    RUN_PARSER("\x0B\x03\xEF\x12\x34");

    // Test for BCD 8 Digits
    //  E-123456
    RUN_PARSER("\x0C\x03\xEF\x12\x34\x56");

    // Test for BCD 12 Digits
    //  E-1234567890
    RUN_PARSER("\x0E\x03\xEF\x12\x34\x56\x78\x90");
}

void
multipleValue(void)
{
    // Test for Interger 8 bits follow by integer 16 bits
    //  1 and -2
    RUN_PARSER("\x01\x03\x01\x02\x04\xFE\xFF");
    
    // Test for Mulitiple integer 16 bits with vife
    //  1 and 5
    RUN_PARSER("\x02\xFB\x30\x01\x00\x02\xFD\x08\x05\x00");
}

void
singleValueWithVariableLength(void)
{
    // Text
    //  AbCdEFgH
    RUN_PARSER("\x0D\x03\x08""AbCdEFgH");
    
    // Positive BCD
    //  1234
    RUN_PARSER("\x0D\x03\xC2\x12\x34");
    
    // Negative BCD
    //  123456
    RUN_PARSER("\x0D\x03\xD3\x12\x34\x56");
    
    // Float
    //  229.1964
    RUN_PARSER("\x0D\x03\xF8\x4a\x32\x65\x43");
}

void
vifCode(void)
{
    // Test for Primary VIF Code
    //  vif = 3
    RUN_PARSER("\x01\x03\x01");
    
    // Test for Primary VIF Code
    //  vif = x78
    RUN_PARSER("\x01\x78\x01");
    
    // Test for Main VIFE code
    //  vif = xFD08
    RUN_PARSER("\x01\xFD\x08\x01");

    // Test for Alternate VIFE code
    //  vif = xFB30
    RUN_PARSER("\x01\xFB\x30\x01");
}

void
difCode(void)
{
    // Test for 1 dif ext byte
    //  dib = xC411
    //      dib.storage	 = x0
    //      dib.function = x2
    //      dib.coding	 = xE   (BCD 12 digits)
    //      dib.tarif	 = x0
    //      dib.subunit	 = x0
    RUN_PARSER("\x2E\x03\x01\x01\x01\x01\x01\x01");

    // Test for 1 dif ext byte
    //  dib = xC411
    //      dib.storage	 = x3
    //      dib.function = x0
    //      dib.coding	 = x4   (int 64bits)
    //      dib.tarif	 = x1
    //      dib.subunit	 = x0
    RUN_PARSER("\xC4\x11\x03\x01\x00\x00\x00");

    // Test for 3 dif ext byte
    //  dib = xC491A375
    //      dib.storage	 = xA63
    //      dib.function = x0
    //      dib.coding	 = x4
    //      dib.tarif	 = x39
    //      dib.subunit	 = x4
    RUN_PARSER("\xC4\x91\xA3\x75\x03\x01\x00\x00\x00");
}

void
scaleForObix(void)
{
    // 1 * 10^1 Wh => 10
    RUN_PARSER("\x01\x04\x01");
    
    // 1 * 10^-1 Wh => 0.1
    RUN_PARSER("\x01\x02\x01");
    
    // 229 * 10^-2 Volts
    RUN_PARSER("\x05\x0FD\x47\x4a\x32\x65\x43");
}

int
main (int argc, char **argv)
{
  int nbLoops = 1;
  int i = 0;

  if (argc == 2)
  {
    nbLoops = atoi(argv[0]);
  } 

  for (i = 0; i < nbLoops; i++)
  {
    fprintf(stderr, "###### %d ######\n", i);
    singleValue();
    multipleValue();
    singleValueWithVariableLength();
    vifCode();
    difCode();
    scaleForObix();
  }

  return 0;
}

