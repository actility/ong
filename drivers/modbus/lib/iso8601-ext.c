#include "modbus-acy.h"

/**
 * @brief Parse the provided string as an ISO8601 durationw with milli-seconds support, based on drvcommon parser
 * @param string the string to parse.
 * @param ms milli-seconds read from the input string
 * @return the parsed duration in seconds, or 0 if a parse error happens.
 */
int
parseISO8601Duration_ms(char *string, int *ms)
{
  if (ms != NULL) {
    char *ms_string = index(string, ',');
    if (ms_string != NULL) {
      if (*(ms_string + 1) != '\0') {
        if (strlen(ms_string + 1) > 3) {
          *(ms_string + 4) = '\0'; // Ignore after ms
        }
      
        *ms = atoi(ms_string + 1);
      }
      
      *ms_string = '\0'; 
    }
  }
  
  int ts = parseISO8601Duration(string);
  
  // Return the bit sign on ms when ts is zero
  if (ts == 0 && *string == '-') {
    *ms = -1 * *ms;
  }

  return ts;
}

