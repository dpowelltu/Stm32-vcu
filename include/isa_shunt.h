#ifndef ISA_SHUNT_H
#define ISA_SHUNT_H

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>

#include "vcu_device.h"


enum class ISA_STATES{
			IDLE,
			INIT,
			START_CONFIGIRATION,
			LOAD_CONFIG,
			STORE_CONFIG,
			SEND_START,
			CONFIGURED,
			FINISHED,
			FAULT
		};
		
		
class ISA_Shunt: public vcu_device{
    
    private:
        ISA_STATES m_state;
		uint8_t m_config_index;
		
		void STOP_MSG();
		void STORE_MSG();
		void START_MSG();
		void RESTART_MSG();
		void DEFAULT_MSG();
		void CONFIG_MSG(uint8_t );
	
	protected:
		
		bool firstframe=true;

		int32_t Amperes;
		int32_t Ah;
		int32_t KW;
		int32_t KWh;
		int32_t Voltage=0;
		int32_t Voltage2=0;
		int32_t Voltage3=0;
		int16_t Temperature;
		uint32_t framecount;

		
    
    public:
		
		ISA_Shunt(){
			m_time_base=200;
		}
		
	
		
        void INIT()override{
			//init DMA etc? 
			
			//turn on power to inverter?
			m_can_ptr->RegisterUserMessage(0x521);
			m_can_ptr->RegisterUserMessage(0x522);
			m_can_ptr->RegisterUserMessage(0x523);
			m_can_ptr->RegisterUserMessage(0x524);
			m_can_ptr->RegisterUserMessage(0x525);
			m_can_ptr->RegisterUserMessage(0x526);
			m_can_ptr->RegisterUserMessage(0x527);
			m_can_ptr->RegisterUserMessage(0x528);
			
			m_state = ISA_STATES::START_CONFIGIRATION;
			
		}
        void START()override{
			
		}
        void STOP()override{
	
		}		
		
        bool STANDBY ()override{return false;}
        bool STARTING()override{return false;}
        bool RUNNING ()override{return false;}
        bool STOPPING ()override{return false;}
        bool STOPPED ()override{return false;}
		
		
		
		
		bool RequiresStart()override{ return false; }
		bool RequiresCAN()override{ return true; }
		
		
			
		bool ProcessCANMessage(uint32_t can_id, uint32_t data[2]) override;
		
		void deFAULT();
		void RESTART();
		
		
	
			
		void Update()override;

		
	};

#endif
