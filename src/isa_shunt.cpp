/*  This library supports ISA Scale IVT Modular current/voltage sensor device.  These devices measure current, up to three voltages, and provide temperature compensation.

    This library was written by Jack Rickard of EVtv - http://www.evtv.me
    copyright 2014
    You are licensed to use this library for any purpose, commercial or private,
    without restriction.

*/


#include <isa_shunt.h>
#include "my_fp.h"
#include "my_math.h"
#include "stm32_can.h"
#include "params.h"






bool ISA_Shunt::ProcessCANMessage(uint32_t can_id, uint32_t data[2]){
			uint16_t temp=0;
			uint8_t* bytes;
			
			switch(can_id){
				case 521:
					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					Amperes = ((bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]));
					return true;
				break;
				
				case 522:
					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					Voltage=((bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]));
					return true;
				break;
				
				case 523:
					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					Voltage2 = (uint32_t)((bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]));
					return true;
						break;

				case 524:
					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					Voltage3 = (uint32_t)((bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]));

					return true;
				break;

				case 525:

					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					framecount++;
					
					temp = (uint16_t)((bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]));

					Temperature=temp/10;
					return true;
					
					break;



				case 526:
					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					KW = (int32_t)((bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]));
					return true;
					break;

				case 527:
					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					Ah = (bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]);
					return true;
					break;

				case 528:
					bytes = (uint8_t*)data;// arrgghhh this converts the two 32bit array into bytes. See comments are useful:)
					KWh=((bytes[5] << 24) | (bytes[4] << 16) | (bytes[3] << 8) | (bytes[2]));
					return true;
					break;
				
				
				default:
					
					break;
				
			}
			return false;
	
		}
		
		





void ISA_Shunt::STOP_MSG()
{
    uint8_t bytes[8];
//SEND STOP///////


    bytes[0]=0x34;
    bytes[1]=0x00;
    bytes[2]=0x01;
    bytes[3]=0x00;
    bytes[4]=0x00;
    bytes[5]=0x00;
    bytes[6]=0x00;
    bytes[7]=0x00;

    m_can_ptr->Send(0x411, (uint32_t*)bytes,8);

}
void ISA_Shunt::STORE_MSG()
{
    uint8_t bytes[8];
//SEND STORE///////

    bytes[0]=0x32;
    bytes[1]=0x00;
    bytes[2]=0x00;
    bytes[3]=0x00;
    bytes[4]=0x00;
    bytes[5]=0x00;
    bytes[6]=0x00;
    bytes[7]=0x00;

     m_can_ptr->Send(0x411, (uint32_t*)bytes,8);


}

void ISA_Shunt::START_MSG()
{
    uint8_t bytes[8];
//SEND START///////

    bytes[0]=0x34;
    bytes[1]=0x01;
    bytes[2]=0x01;
    bytes[3]=0x00;
    bytes[4]=0x00;
    bytes[5]=0x00;
    bytes[6]=0x00;
    bytes[7]=0x00;
    m_can_ptr->Send(0x411, (uint32_t*)bytes,8);
}

void ISA_Shunt::RESTART_MSG()
{
    //Has the effect of zeroing AH and KWH
    uint8_t bytes[8];

    bytes[0]=0x3F;
    bytes[1]=0x00;
    bytes[2]=0x00;
    bytes[3]=0x00;
    bytes[4]=0x00;
    bytes[5]=0x00;
    bytes[6]=0x00;
    bytes[7]=0x00;
    m_can_ptr->Send(0x411, (uint32_t*)bytes,8);


}


void ISA_Shunt::DEFAULT_MSG()
{
    //Returns module to original defaults
    uint8_t bytes[8];

    bytes[0]=0x3D;
    bytes[1]=0x00;
    bytes[2]=0x00;
    bytes[3]=0x00;
    bytes[4]=0x00;
    bytes[5]=0x00;
    bytes[6]=0x00;
    bytes[7]=0x00;
    m_can_ptr->Send(0x411, (uint32_t*)bytes,8);


}

void ISA_Shunt::CONFIG_MSG(uint8_t i)
{
    //Returns module to original defaults
    uint8_t bytes[8];
	
	if(i>8) return;
	
	bytes[0]=(0x20+i);
	bytes[1]=0x42;
	bytes[2]=0x00;
	bytes[3]=0x64;
	bytes[4]=0x00;
	bytes[5]=0x00;
	bytes[6]=0x00;
	bytes[7]=0x00;

	m_can_ptr->Send(0x411, (uint32_t*)bytes, 8);
}

#if 0
void ISA::initCurrent()
{
    uint8_t bytes[8];
    STOP();
    delay();
    bytes[0]=0x21;
    bytes[1]=0x42;
    bytes[2]=0x01;
    bytes[3]=0x61;
    bytes[4]=0x00;
    bytes[5]=0x00;
    bytes[6]=0x00;
    bytes[7]=0x00;

    Can::GetInterface(0)->Send(0x411, (uint32_t*)bytes,8);

    delay();
    sendSTORE();
    START();
    delay();

}

void ISA_Shunt::initialize()
{
    uint8_t bytes[8];

    firstframe=false;
    STOP();
    delay();
    for(int i=0; i<9; i++)
    {
        bytes[0]=(0x20+i);
        bytes[1]=0x42;
        bytes[2]=0x00;
        bytes[3]=0x64;
        bytes[4]=0x00;
        bytes[5]=0x00;
        bytes[6]=0x00;
        bytes[7]=0x00;

        m_can_ptr->Send(0x411, (uint32_t*)bytes, 8);
        delay();

        sendSTORE();
        delay();
    }
    START();
    delay();

}
#endif


void ISA_Shunt::Update(){
	static uint8_t flag =0;
	
	if(flag){
		flag = 0;
		DigIo::led_out.Clear();
	}
	else{
		flag = 1;
		DigIo::led_out.Set();
	}
	
	
	switch(m_state){
		case ISA_STATES::IDLE:
		
		break;
		
		case ISA_STATES::INIT:
		
		break;
		
		case ISA_STATES::START_CONFIGIRATION:
			STOP_MSG();
			m_state = ISA_STATES::LOAD_CONFIG;
			m_config_index=0;
		break;
		
		case ISA_STATES::LOAD_CONFIG:
			CONFIG_MSG(m_config_index);
			m_config_index++;
			m_state =ISA_STATES::STORE_CONFIG;
		break;

		case ISA_STATES::STORE_CONFIG:
			STORE_MSG();
			m_state =ISA_STATES::SEND_START;
		break;
		
		case ISA_STATES::SEND_START:
			START_MSG();
			m_state = ISA_STATES::CONFIGURED;
		break;
		
		case ISA_STATES::CONFIGURED:
				
		break;
	
		case ISA_STATES::FINISHED:
		
		break;
		case ISA_STATES::FAULT:
		
		break;
		
		
		
	}

}	