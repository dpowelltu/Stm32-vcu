#include "PriusInverter.h"

void PriusInverter::ProcessHTMFrame(){
	 
}
	
void PriusInverter::ProcessMTHFrame(){
	   
	   mth_data[0]=htm_data[1];   
	   
}

uint16_t PriusInverter::getSpeed(){
 	 return 0;
 	 		   
} 

void PriusInverter::setTorque(uint16_t){
	
} 
