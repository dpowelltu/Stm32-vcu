#ifndef BASIC_INVERTER_H
#define BASIC_INVERTER_H

#include "vcu_device.h"



class BasicInverter: public vcu_device{
    
    private:
        int state;
    
    public:
         void INIT()override{}
         void START()override{}
         void STOP()override{}		
		
         bool STANDBY ()override{return false;}
         bool STARTING()override{return false;}
         bool RUNNING ()override{return false;}
         bool STOPPING ()override{return false;}
         bool STOPPED ()override{return false;}
		
		 void Update()override{ }
		
		
		 bool RequiresStart()override{ return false; }
		 bool RequiresCAN()override { return false; }
		
		 bool ProcessCANMessage(uint32_t can_id, uint32_t data[2])override{
			switch(can_id){
				case 0:
					
					return true;
				break;
				
				default:
					
					break;
				
			}
			return false;
	
		}
    
    
};

#endif
