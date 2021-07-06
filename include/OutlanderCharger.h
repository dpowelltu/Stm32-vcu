#ifndef OUTLANDER_CHARGER_H
#define OUTLANDER_CHARGER_H

#include "vcu_device.h"

enum class CHARGER_STATES{
			IDLE,
			INIT,
			EVSE_STAGE1,
			EVSE_STAGE2,
			EVSE_STAGE3,

			CC_CHARGE,
			CV_CHARGE,
			COMPLETE,
			WAIT_FOR_CHARGER_DISCONNECT,	
			FINISHED,
			FAULT
		};

class OutlanderCharger: public vcu_device{
    
    private:
        CHARGER_STATES m_state;
		CHARGER_STATES m_last_state;
		
		uint16_t m_bat_voltage;
		uint16_t m_supply_voltage;
		uint16_t m_charger_can_alive;
		uint16_t m_evse_pilot;
		
		uint16_t m_max_battery_voltage;
		uint16_t m_CC_to_CV_changeover_voltage;
		uint16_t m_max_charge_current;
		
		uint16_t m_count;
		
		uint16_t m_charger_voltage_sp, m_charger_curret_sp;
		uint16_t m_charger_evse_req, m_charger_hb;
	
	
    
    public:
        void INIT() override;
		void START()override;
		void STOP()override;
		
        bool STANDBY ()override{return m_state == CHARGER_STATES::IDLE;}
        bool STARTING()override{return m_state == CHARGER_STATES::INIT;}
        bool RUNNING ()override{return (m_state == CHARGER_STATES::CC_CHARGE || m_state==CHARGER_STATES::CV_CHARGE);}
        bool STOPPING ()override{return (m_state == CHARGER_STATES::COMPLETE ||m_state==CHARGER_STATES::WAIT_FOR_CHARGER_DISCONNECT);}
        bool STOPPED ()override{return  m_state == CHARGER_STATES::FINISHED;}
		
		bool RequiresStart()override{ 
		
			
			return (GetProxState()==2); 
			
		
		}

		
		bool RequiresCAN() override{ return true; }
		bool ProcessCANMessage(uint32_t, uint32_t *)override;
		void Update(void)override;
		
		
		
		void sendEVSEData(uint16_t , uint16_t );
		void sendChargerSPData(uint16_t , uint16_t );
		
		uint8_t GetProxState(){
			uint16_t prox = AnaIn::GP_analog1.Get();
  
			//requires a pull up resistor on the ADC input to allow us to read the prox circuit
			// a pull up value of 1K???
			if(prox < 200){
			   //connected, button not pressed 
			   return 2;
			  }
			else if(prox < 400){
			  //connected, button pressed
			  return 1;
			}
			else{
			  // not connected
			  return 0;
			}
		}
    
};

#endif
