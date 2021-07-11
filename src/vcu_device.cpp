#ifndef VCU_DEVICE_H
#define VCU_DEVICE_H

/*  This the base class for all VCU Devices, such as vehicles, chargers, inverters etc... 

*/

#include <stdint.h>
#include "my_fp.h"

class vcu_object
{

public:
     private:
        int timebase;
		Stm32Scheduler *sch;
		Can *can_interface;
    
    public:
        virtual void INIT()=0;
        virtual void STANDBY ()=0;
        virtual void STARTING()=0;
        virtual void RUNNING ()=0;
        virtual void STOPPING ()=0;
        virtual void STOPPED ()=0;
		
    vcu_object()
    {
        
    }


};

#endif /* BMW_E65_h */

