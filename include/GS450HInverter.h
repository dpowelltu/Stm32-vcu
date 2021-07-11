#ifndef GS450H_INVERTER_H
#define GS450H_INVERTER_H



#include "ToyotaInverter.h"


class GS450HInverter: public ToyotaInverter{
    
    private:
        
     
    public:
		GS450HInverter():ToyotaInverter(80,100){ //call base constructor and give frame sizes
			//mth_length=100;
			//htm_length=120;
		}
	
		 
		void ProcessHTMFrame()override;
		void ProcessMTHFrame()override;
		uint16_t getSpeed()override;
		void setTorque(uint16_t)override;
		
	

    
    
};

#endif
