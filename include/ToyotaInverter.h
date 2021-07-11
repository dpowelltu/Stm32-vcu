#ifndef TOYOTA_INVERTER_H
#define TOYOTA_INVERTER_H

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>

#include "vcu_device.h"
//#include "PriusInverter.h"


class ToyotaInverter: public vcu_device{
    
    private:
        uint8_t htm_state=0;
	
	protected:
		uint8_t mth_data[150];
		uint8_t mth_length=80;
		uint8_t htm_data[150];
		uint8_t htm_length=100;
		
    
    public:
		
		ToyotaInverter(){
			m_time_base=1;
		}
		
		ToyotaInverter(uint8_t mth, uint8_t htm){
			m_time_base=1;
			mth_length=mth;
			htm_length=htm;
		}
		
        void INIT()override{
			//init DMA etc? 
			
			//turn on power to inverter?
			
		}
        void START()override{
			htm_state=1;
		}
        void STOP()override{
			htm_state=0;
		}		
		
        bool STANDBY ()override{return false;}
        bool STARTING()override{return false;}
        bool RUNNING ()override{return htm_state!=0;}
        bool STOPPING ()override{return false;}
        bool STOPPED ()override{return htm_state==0;}
		
		
		
		
		bool RequiresStart()override{ return false; }
		bool RequiresCAN()override{ return false; }
		
		//These abstract functions need to be provided by a childclass, ie PriusInverter, GS450HInverter etc... 
		virtual uint16_t getSpeed()=0;
		virtual void setTorque(uint16_t)=0;
		virtual void ProcessHTMFrame()=0;
		virtual void ProcessMTHFrame()=0;
		
		
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
		
		
		bool VerifyMTHChecksum()
			{

				uint16_t mth_checksum=0;

				for(int i=0; i<(mth_length-2); i++)
					mth_checksum+=mth_data[i];


				if(mth_checksum==(mth_data[mth_length-2]|(mth_data[mth_length-1]<<8))) return true;
				else return false;

			}

		void CalcHTMChecksum()
			{

				uint16_t htm_checksum=0;

				for(int i=0; i<(htm_length-2); i++)htm_checksum+=htm_data[i];
				htm_data[htm_length-2]=htm_checksum&0xFF;
				htm_data[htm_length-1]=htm_checksum>>8;

			}

			
		void Update()override{ 
			static uint16_t inv_status = 0;
			static uint16_t counter = 0;
			
			switch(htm_state)
			{
				case 0:	//idle state!
			
				break;
			
				case 1:
				{
					//dma_read(mth_data,100);//read in mth data via dma. Probably need some kind of check dma complete flag here
					DigIo::req_out.Clear(); //HAL_GPIO_WritePin(HTM_SYNC_GPIO_Port, HTM_SYNC_Pin, 0);
					htm_state++;
				}
				break;

				case 2:
				{
					DigIo::req_out.Set();  //HAL_GPIO_WritePin(HTM_SYNC_GPIO_Port, HTM_SYNC_Pin, 1);

					if(inv_status==0)
					{
						if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL7, DMA_TCIF))// if the transfer complete flag is set then send another packet
						{
							dma_clear_interrupt_flags(DMA1, DMA_CHANNEL7, DMA_TCIF);//clear the flag.
							//dma_write(htm_data,80); //HAL_UART_Transmit_IT(&huart2, htm_data, 80);
						}

					}
					else
					{
						//dma_write(htm_data_setup,80);   //HAL_UART_Transmit_IT(&huart2, htm_data_setup, 80);
						if(mth_data[1]!=0)
							inv_status--;
					}
					htm_state++;
					break;

				case 3:
					htm_state++;
				}
				break;

				case 4:
				{
					ProcessMTHFrame();

					htm_state++;
				}
				break;

				case 5:
				{

					ProcessHTMFrame();

					if(counter>100)
					{
						counter = 0;
					}
					else
					{
						counter++;
					}

					htm_state=1;
				}
				break;
				
			
			}
		}

		
	};

#endif
