#ifndef VCU_DEVICE_H
#define VCU_DEVICE_H

/*  This the base class for all VCU Devices, such as vehicles, chargers, inverters etc... 

*/

#include <stdint.h>
#include "my_fp.h"
#include <libopencm3/stm32/can.h>

#include "stm32scheduler.h"
#include "stm32_can.h"

#include "digio.h"
#include "hwinit.h"
#include "anain.h"

#include "params.h"

class vcu_device
{

public:
     private:
        
		int m_can_rx_list[20];
		int m_can_rx_list_count=0;
	
		
		

    
    public:
		//Stm32Scheduler *m_sch_ptr=0;
		Can			*m_can_ptr=0;
		int m_time_base=100;
		
        virtual void INIT()=0;
		virtual void START()=0;
		virtual void STOP()=0;		
		
		
        virtual bool STANDBY ()=0;
        virtual bool STARTING()=0;
        virtual bool RUNNING ()=0;
        virtual bool STOPPING ()=0;
        virtual bool STOPPED ()=0;
		
		virtual void Update()=0;
		
		virtual bool RequiresStart()=0;
		virtual bool RequiresCAN()=0;
		
		virtual bool ProcessCANMessage(uint32_t can_id,  uint32_t data[2])=0;
		
		void Register(Can *can_ptr){
			//m_sch_ptr=sch_ptr;
			m_can_ptr=can_ptr;
		}
		
		
		
		void AddCANID(int can_id){
			if(m_can_rx_list_count<20)
				m_can_rx_list[m_can_rx_list_count++]=can_id;
			if(m_can_ptr!=0)
				m_can_ptr->RegisterUserMessage(can_id);
			
		}
		
    //vcu_object()
    //{
        
    //}


};

#endif /* VCU_DEVICE_H */

