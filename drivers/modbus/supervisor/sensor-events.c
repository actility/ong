#include "modbus.h"

/**
 * @brief Do synchronize each layer with actual time.
 * One second has elapsed since last call. Each layer that needs
 * to process periodical tasks (i.e. recurrent timers) can do this
 * from this call.
 *
 * @param now current time in second (time(2))
 * @return void
 */
void
ModbusSensorClockSc(time_t now)
{
  static time_t next15 = 0;

  if (now > next15) {
    next15 += 15;
    ProductsSyncNull();   // Ensure all product descriptor are downloaded
    NetworkStartAll();    // Ensure all network and devices are created
  }
}

