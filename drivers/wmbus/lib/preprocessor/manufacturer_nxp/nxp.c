#include <string.h>
#include "wmbus.h"

/*
 *  CRC API for NXP OM13006
 */
unsigned short
crc16_0x3D65_normal (unsigned char *message, unsigned int msgsize, unsigned short init, unsigned short xor)
{
	unsigned short c;
	unsigned char d, s, t;
	unsigned int i;
	
	// init
	c = init;

	// compute CRC
	for (i = 0; i < msgsize; i++) {
		d = message[i];
		
		s = (c >> 8) ^ d;
		t = (s >> 5) ^ (s >> 4) ^ (s >> 3) ^ s;
		c = (c << 8) ^
		     t       ^
		    (t << 2) ^
		    (t << 5) ^
		    (t << 6) ^
		    (t << 8) ^
		    (t << 10) ^
		    (t << 11) ^
		    (t << 12) ^
		    (t << 13);
	}

	// final xor
	c = c ^ xor;

	return c;
}

short
calc_crc (unsigned char *message, int msgsize)
{
	return (short)crc16_0x3D65_normal(message, (unsigned int)msgsize, 0x0000, 0xFFFF);
}

/*
 *  Preprocessor for NXP OM13006
 *  xA0 (NXP data) -> x72 (MBus data)
 */
int
OM13006 (char *input, int inputLen, char *output, int outputMaxLen)
{
    int i;
    
    //DUMP_MEMORY(input, inputLen);

    // Copy header
    memcpy(output, input, sizeof(wmBusHeader_t));
    i = sizeof(wmBusHeader_t);
    
    // Set CI Field
    output[i++] = WMBUS_REPORT_APP_CODE;

    // Uptime
    output[i++] = 0x03;         // DIB 24b integer
    output[i++] = 0x20;         // Up Time (seconds)
    output[i++] = input[10];
    output[i++] = input[11];
    output[i++] = input[12];
        
    // Volts
    output[i++] = 0x05;         // DIB 32b real
    output[i++] = 0xFD;         // VIF code in the first extension table
    output[i++] = 0x49;         // VIFE Volts
    output[i++] = input[13];    // Little endian float
    output[i++] = input[14];
    output[i++] = input[15];
    output[i++] = input[16];

    // Amperes
    output[i++] = 0x05;         // DIB 32b real
    output[i++] = 0xFD;         // VIF code in the first extension table
    output[i++] = 0x5C;         // VIFE Amperes
    output[i++] = input[17];    // Little endian float
    output[i++] = input[18];
    output[i++] = input[19];
    output[i++] = input[20];

    // Power
    output[i++] = 0x05;         // DIB 32b real
    output[i++] = 0x2B;         // VIB Power (W)
    output[i++] = input[21];    // Little endian float
    output[i++] = input[22];
    output[i++] = input[23];
    output[i++] = input[24];
    
    // Float Q1
    // Float S
    // Float S1
    // Float PF
    // Float PF1
    // Float SN
    // Float N
    // Float THDI
    
    // Consumed energy
    output[i++] = 0x05;         // DIB 32b real
    output[i++] = 0x03;         // VIB Power (Wh)
    output[i++] = input[57];    // Little endian float
    output[i++] = input[58];
    output[i++] = input[59];
    output[i++] = input[60];    
    
    // Float Produced energie ??
    // Float Fe
    // Bool relais state 


    // Return the new size
    return i;
}

/*
 *  Commands callback of NXP OM13006
 */
int
OM13006_cmd (char *output, int outputMaxLen, int application, int cluster, int command, unsigned long long int cp_addr)
{
    int len = 0;
    short crc;
    unsigned int sn = IEEE_TO_SN(cp_addr);
    
    if (command == 1) {
        output[len++] = 0xA0;
        output[len++] = (sn & 0x000000FF) >> 0;
        output[len++] = (sn & 0x0000FF00) >> 8;
        output[len++] = (sn & 0x00FF0000) >> 16;
        output[len++] = (sn & 0xFF000000) >> 24;
        output[len++] = 'R';
        output[len++] = 'C';        
    } else
    if (command == 0) {
        output[len++] = 0xA0;
        output[len++] = (sn & 0x000000FF) >> 0;
        output[len++] = (sn & 0x0000FF00) >> 8;
        output[len++] = (sn & 0x00FF0000) >> 16;
        output[len++] = (sn & 0xFF000000) >> 24;
        output[len++] = 'R';
        output[len++] = 'O';        
    }
    
    crc = calc_crc(output, len);
    output[len++] = (char)((crc & 0xFF00) >> 8);
    output[len]   = (char)((crc & 0x00FF) >> 0);
    
    return len;
}
