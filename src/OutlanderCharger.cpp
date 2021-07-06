#include "OutlanderCharger.h"

 
void OutlanderCharger::INIT(){
   // printf("init");
   //AddCANID(0x1DA); 
   
   
   
}

void OutlanderCharger::START(){
	//m_sch_ptr->AddTask(OutlanderCharger::Update, 1000);
}
void OutlanderCharger::STOP(){

}
		
		


bool OutlanderCharger::ProcessCANMessage(uint32_t can_id,  uint32_t data[2]){
	switch(can_id){
		case 0x389:
			m_bat_voltage=data[0]&0xFF; //1st byte
			m_bat_voltage*=2;

			m_supply_voltage=(data[0]>>8)&0xFF; //second byte
			m_charger_can_alive++;
    
       
			
			return true;
		break;
		
		case 0x38A:
			m_evse_pilot= (data[0]>>24)&0xFF; //4th byte
			m_charger_can_alive++;
			return true;
		break;
		
		default:
			
			break;
		
	}
	return false;
	  
}  

void OutlanderCharger::Update(void){
	static uint16_t x, timer, m_prox_state, prox, m_count, slow_flag_counter, slow_flag;

	if(x==0){
		x = 1;
		DigIo::prec_out.Set();//commence precharge
	}
	else{
		x = 0;
		DigIo::prec_out.Clear();//commence precharg		
	}
	
	
#if 1

  if(m_count==5){
    sendChargerSPData(m_charger_voltage_sp,m_charger_curret_sp );
  }
  else{
    sendEVSEData(m_charger_evse_req,m_charger_hb);
  }

  if(m_count > 10){
    
   
    /*
    if(flag){
        flag = 0;
        
      }
      else{
        flag = 1;       
     }*/


    if(slow_flag_counter<10)slow_flag_counter++;
    else{
      slow_flag_counter=0;
      m_charger_hb++;
      m_charger_hb&=0x1F;
      if(slow_flag)slow_flag=0;
      else slow_flag=1;
    }
     
    
  
    prox = AnaIn::throttle1.Get();
  
    //requires a pull up resistor on the ADC input to allow us to read the prox circuit
	// a pull up value of 1K???
    if(prox < 200){
       //connected, button not pressed 
       m_prox_state = 2;
      }
    else if(prox < 400){
      //connected, button pressed
      m_prox_state=1;
    }
    else{
      // not connected
      m_prox_state=0;
    }
  
  
    
  
    switch(m_state){
  
      case CHARGER_STATES::IDLE:
        if( m_prox_state == 1){
          m_state=CHARGER_STATES::INIT;
          //digitalWrite(CHARGER_POWER, HIGH);
          m_charger_evse_req=3;
          
        }
      break;
  
      
      case CHARGER_STATES::INIT:
        // start sequence for pulling in EVSE
        if(timer>10){
          m_charger_evse_req=0x16;
          m_state=CHARGER_STATES::EVSE_STAGE1;
          //digitalWrite(CHARGER_STATUS_LED, HIGH);

          //SerialUSB.print("bat_voltage ");
          //SerialUSB.println(bat_voltage);
        }
        
      break;
  
      
      case CHARGER_STATES::EVSE_STAGE1:
        //EV
        if(timer>10){
          m_charger_evse_req=0xb6;
          m_state=CHARGER_STATES::EVSE_STAGE2;

          //SerialUSB.print("evse_pilot ");
          //SerialUSB.println(evse_pilot);
        }
      break;
  
      
      case CHARGER_STATES::EVSE_STAGE2:
        
        if(timer>10){

          //Check EVSE has pulled in, by checking pwoer to charger! 
          //SerialUSB.print("supply_voltage ");
          //SerialUSB.println(supply_voltage);
          
          //if EVSE hasn't pulled in, quite and flash fault LED
      
          
          m_charger_curret_sp=2;
          m_charger_voltage_sp=240;
          // Ramp charge current to 3A
          // when we hit top of ramp, jump to the next state
          m_state=CHARGER_STATES::CC_CHARGE;
          }
      break;
  
  
  
      case CHARGER_STATES::CC_CHARGE:
        //Charging
        //Montor battery voltage, only bring it to 230V, before dropping out

        //digitalWrite(CHARGER_STATUS_LED, flag);
        //Press button to quit charging
        //or we are about to be disconnected! 
        //could also check for over voltage here!!! 
        if(m_prox_state != 1){
          m_state=CHARGER_STATES::CV_CHARGE;
          m_charger_curret_sp=0;
          m_charger_voltage_sp=0;
        }

        if(m_bat_voltage > 230){
          m_state=CHARGER_STATES::COMPLETE;
          m_charger_curret_sp=0;
          m_charger_voltage_sp=0;
          // SerialUSB.println("charge complete");
          // SerialUSB.println(bat_voltage);
        }
  
      break;
  
      
      case CHARGER_STATES::CV_CHARGE:
        //Set charging target to zero
        //digitalWrite(CHARGER_STATUS_LED, HIGH);
        m_state=CHARGER_STATES::COMPLETE;
        //press button to re-start charging
        // if(digitalRead(21)==LOW && last_button == HIGH){
        //  digitalWrite(CHARGER_STATUS_LED, HIGH);
          
        //  m_state=3;
        //}
      break;

    case CHARGER_STATES::COMPLETE:
      //Charge Complete state
      //digitalWrite(CHARGER_STATUS_LED, slow_flag);
	  m_state=CHARGER_STATES::WAIT_FOR_CHARGER_DISCONNECT;
      
    break;
	
	case CHARGER_STATES::WAIT_FOR_CHARGER_DISCONNECT:	
		if(m_prox_state == 0) m_state = CHARGER_STATES::FINISHED;
	break;
	case CHARGER_STATES::FINISHED:

	break;
    case CHARGER_STATES::FAULT:
      //fault state! 

    break;
      
    }
  
    if(timer<65535)timer++;
    if(m_state != m_last_state) timer=0;
    
    m_last_state=m_state;
    
    //SerialUSB.println(analogRead(A0));
  
    //last_button = digitalRead(21);

    m_count =0;
  }
  else{
    m_count++;
    }
	
	
#endif

}

#if 1

void OutlanderCharger::sendEVSEData(uint16_t val, uint16_t hb)
{
  uint8_t databyte[8]={0};

  
  databyte[0] = 0;
  databyte[1] = 0;
  databyte[2] = val;
  databyte[3] = 9;

  databyte[4] = hb;
  databyte[5] = 0;

  if(val==0xb6)
    databyte[6] = 0;
  else
    databyte[6] = 8;
  
  databyte[7] = 0xa;

  
  
  m_can_ptr->Send(0x285, (uint32_t*)databyte,8); 
  
}

void OutlanderCharger::sendChargerSPData(uint16_t voltage, uint16_t current)
{
  uint8_t databyte[8]={0};
  
  //CAN_FRAME outgoing;
  //outgoing.id = 0x286;
  //outgoing.extended = false;
  //outgoing.priority = 4; //0-15 lower is higher priority
  //outgoing.length=8;
  voltage *= 10;

  databyte[0] = voltage>>8;
  databyte[1] = voltage &= 0xFF;
  databyte[2] = current;
  databyte[3] = 51;

  databyte[4] = 0x0;
  databyte[5] = 0x0;
  databyte[6] = 0xa;
  databyte[7] = 0x0;

  
  //Can0.sendFrame(outgoing);
  
  m_can_ptr->Send(0x286, (uint32_t*)databyte,8); 
  
}

#endif
