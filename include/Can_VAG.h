#ifndef Can_VAG_h
#define Can_VAG_h

/*  This library supports the Powertrain CAN messages for VAG cars for driving dash gauges, putting out malf lights etc

*/

#include <stdint.h>
#include "my_fp.h"
#include "stm32_can.h"

class Can_VAG
{

public:
    static void SendVAG100msMessage();

private:

};

#endif /* Can_VAG_h */
