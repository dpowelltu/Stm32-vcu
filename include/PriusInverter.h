#ifndef PRIUS_INVERTER_H
#define PRIUS_INVERTER_H



#include "ToyotaInverter.h"


class PriusInverter: public ToyotaInverter{
    
    private:
        
     
    public:
		PriusInverter():ToyotaInverter(100,120){ //call base constructor and give frame sizes
			//mth_length=100;
			//htm_length=120;
		}
	
		 
		void ProcessHTMFrame()override;
		void ProcessMTHFrame()override;
		uint16_t getSpeed()override;
		void setTorque(uint16_t)override;
		
	

    
    
};

#endif
