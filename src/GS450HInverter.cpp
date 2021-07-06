#include "GS450HInverter.h"

void GS450HInverter::ProcessHTMFrame(){
	 
}
	
void GS450HInverter::ProcessMTHFrame(){
	   
	   mth_data[0]=htm_data[1];   
	   
}

uint16_t GS450HInverter::getSpeed(){
 	 return 0;
 	 		   
} 

void GS450HInverter::setTorque(uint16_t){
	
} 
