#ifndef BASIC_VEHICLE_H
#define BASIC_VEHICLE_H

#include "vcu_device.h"



class BasicVehicle: public vcu_device{
    
	
    private:
        int state;
		
		enum VEHICLE_STATES{
			STARTING_STATE,
			NEUTRAL_STATE,
			MOVING_STATE,
			
			
			FAULT_STATE
			
			
		};
    
    public:
        void INIT()override{}
        void START()override{}
        void STOP()override{}		
		
        bool STANDBY ()override{return false;}
        bool STARTING()override{return false;}
        bool RUNNING ()override{return false;}
        bool STOPPING()override{return false;}
        bool STOPPED ()override{return false;}
		
		void Update()override{ }
		
		bool RequiresStart()override{ return false; }
		bool RequiresCAN()override{ return false; }

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
