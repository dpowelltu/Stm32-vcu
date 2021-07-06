/*
 * This file is part of the tumanako_vc project.
 *
 * Copyright (C) 2010 Johannes Huebner <contact@johanneshuebner.com>
 * Copyright (C) 2010 Edward Cheeseman <cheesemanedward@gmail.com>
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2020 Damien Maguire <info@evbmw.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "stm32_vcu.h"

#define RMS_SAMPLES 256
#define SQRT2OV1 0.707106781187
#define  UserCAN  2
#define  Zombie  3
#define  BMW_E46  0
#define  User  2
#define  None  4
#define  BMW_E39  5
#define  VAG  6

HWREV hwRev; // Hardware variant of board we are running on
static Stm32Scheduler* scheduler;
static bool chargeMode = false;
static Can* can1;
static Can* can2;
static _invmodes targetInverter;
static _vehmodes targetVehicle;
static _chgmodes targetCharger;
static uint8_t Lexus_Gear;
static uint16_t Lexus_Oil;
static uint16_t maxRevs;
static uint32_t oldTime;


// Instantiate Classes
BMW_E65Class E65Vehicle;
GS450HClass gs450Inverter;
chargerClass chgtype;

void MainSystemTask(void);

vcu_device *devices[4]={0};

enum DEVICE_TYPES{
	VEHICLE,
	CHARGER,
	INVERTER,
	SHUNT	
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Ms200Task(void)
{
   for(int i =0; i<4; i++){
		if(devices[i]->m_time_base == 200)
			devices[i]->Update();
	}
}






static void Ms100Task(void)
{
	MainSystemTask();
	
    for(int i =0; i<4; i++){
		if(devices[i]->m_time_base == 100)
			devices[i]->Update();
	}
}


static void Ms10Task(void)
{
    for(int i =0; i<4; i++){
		if(devices[i]->m_time_base == 10)
			devices[i]->Update();
	}
}


static void Ms1Task(void)
{
    for(int i =0; i<4; i++){
		if(devices[i]->m_time_base == 1)
			devices[i]->Update();
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void parm_Change(Param::PARAM_NUM paramNum)
{
    // This function is called when the user changes a parameter
    if (Param::canspeed == paramNum)
        can1->SetBaudrate((Can::baudrates)Param::GetInt(Param::canspeed));
        can2->SetBaudrate((Can::baudrates)Param::GetInt(Param::canspeed));

    Throttle::potmin[0] = Param::GetInt(Param::potmin);
    Throttle::potmax[0] = Param::GetInt(Param::potmax);
    Throttle::potmin[1] = Param::GetInt(Param::pot2min);
    Throttle::potmax[1] = Param::GetInt(Param::pot2max);
    Throttle::throtmax = Param::Get(Param::throtmax);
    Throttle::throtmin = Param::Get(Param::throtmin);
    Throttle::idcmin = Param::Get(Param::idcmin);
    Throttle::idcmax = Param::Get(Param::idcmax);
    Throttle::udcmin = FP_MUL(Param::Get(Param::udcmin), FP_FROMFLT(0.95)); //Leave some room for the notification light
    targetInverter=static_cast<_invmodes>(Param::GetInt(Param::Inverter));//get inverter setting from menu
    Param::SetInt(Param::inv, targetInverter);//Confirm mode
    targetVehicle=static_cast<_vehmodes>(Param::GetInt(Param::Vehicle));//get vehicle setting from menu
    Param::SetInt(Param::veh, targetVehicle);//Confirm mode
    targetCharger=static_cast<_chgmodes>(Param::GetInt(Param::chargemode));//get charger setting from menu
    Param::SetInt(Param::Charger, targetCharger);//Confirm mode
    Lexus_Gear=Param::GetInt(Param::GEAR);//get gear selection from Menu
    Lexus_Oil=Param::GetInt(Param::OilPump);//get oil pump duty % selection from Menu
    maxRevs=Param::GetInt(Param::revlim);//get revlimiter value

}


static void CanCallback(uint32_t id, uint32_t data[2]) //This is where we go when a defined CAN message is received.
{
for(int i =0; i<4; i++){
		if(devices[i]!=0){
			if(devices[i]->RequiresCAN()){
				if(devices[i]->ProcessCANMessage(id,data))
					break;
			}
		}
	}
  
}


static void ConfigureVariantIO()
{
    hwRev = HW_REV1;
    Param::SetInt(Param::hwver, hwRev);

    ANA_IN_CONFIGURE(ANA_IN_LIST);
    DIG_IO_CONFIGURE(DIG_IO_LIST);

    AnaIn::Start();
}


extern "C" void tim3_isr(void)
{
    scheduler->Run();
}


extern "C" int main(void)
{
    clock_setup();
    rtc_setup();
    ConfigureVariantIO();
    gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON,AFIO_MAPR_CAN2_REMAP);
    usart_setup();
    usart2_setup();//TOYOTA HYBRID INVERTER INTERFACE
    nvic_setup();
    term_Init();
    parm_load();
    parm_Change(Param::PARAM_LAST);
    DigIo::inv_out.Clear();//inverter power off during bootup

    Can c1(CAN1, (Can::baudrates)Param::GetInt(Param::canspeed));//can1 Inverter / isa shunt.
    Can c2(CAN2, (Can::baudrates)Param::GetInt(Param::canspeed));//can2 vehicle side.
    can1 = &c1;
    can2 = &c2;

    // Set up CAN 1 callback and messages to listen for
    c1.SetReceiveCallback(CanCallback);
     // Set up CAN 2 (Vehicle CAN) callback and messages to listen for.
    c2.SetReceiveCallback(CanCallback);
 

    //can = &c; // FIXME: What about CAN2?

    Stm32Scheduler s(TIM3); //We never exit main so it's ok to put it on stack
    scheduler = &s;

			//create the objects for the charger, inverter and vehicle
	BasicVehicle myCar;
	OutlanderCharger myCharger;
    GS450HInverter myInverter;
	
	
	
	devices[VEHICLE]=&myCar;
	devices[CHARGER]=&myCharger;
	devices[INVERTER]=&myInverter;
	devices[SHUNT]=0;
	
	//register the CAN interface!
	myCar.Register(&c2);
	myCharger.Register(&c1);
	myInverter.Register(&c1);
    
	
	//Initialise the VCU main Task
	//The VCU Task will need objects for the following: Vehicle, Inverter, Charger
	
	//myCharger.INIT();
	//for(int i =0; i<4; i++)
	//	if(devices[i]!=0)
	//		devices[i]->INIT();
	
	myInverter.setTorque(0);
	
	//Start Vehicle, Charger and Inverter Tasks if required! 

	for(int i =0; i<4; i++){
		if(devices[i]->m_time_base==1){
		s.AddTask(Ms1Task, 1); //only start 1mS task if needed!
		break;
		}
	}
	
	
    s.AddTask(Ms1Task, 1);
    s.AddTask(Ms10Task, 10);
    s.AddTask(Ms100Task, 100);
    s.AddTask(Ms200Task, 200);


    // ISA::initialize();//only call this once if a new sensor is fitted. Might put an option on web interface to call this....
    //  DigIo::prec_out.Set();//commence precharge
    Param::SetInt(Param::version, 4); //backward compatibility

    term_Run();

    return 0;
}


enum VCU_STATES{
SYSTEM_INIT_STATE,
SYSTEM_LOW_POWER_IDLE_STATE,


SYSTEM_PRE_CHARGE_B_MINUS_CHECK, // should see zero current as we close pre-charge relay, if a B- contactor used
SYSTEM_PRE_CHARGE_TEST, //should see zero current as we close B- contactor
SYSTEM_PRE_CHARGE_ACTIVE, //close Pre-charge contactor
SYSTEM_PRE_CHARGE_COMPLETE,//close B+ Contactor
SYSTEM_PRE_CHARGE_FAULT,



SYSTEM_HV_READY, //waiting for throttle == 0

SYSTEM_DISCONNECT_CONTACTORS,
SYSTEM_DISCONNECT_WAIT, //return to low power state

SYSTEM_DISABLED,

SYSTEM_GENERAL_FAULT

} ;

enum{
	SYSTEM_IDLE,
	SYSTEM_CHARGING,
	SYSTEM_DRIVING
};

//called every 100mS
void MainSystemTask(void){
	static uint16_t state = 0, timer = 0, system_mode = 0;
	
		switch(state){
			
			case SYSTEM_INIT_STATE:
				devices[CHARGER]->INIT();
				devices[VEHICLE]->INIT();
				devices[INVERTER]->INIT();
				//devices[SHUNT]->INIT();
				state++;
				
			break;
			case SYSTEM_LOW_POWER_IDLE_STATE:
				
				if(devices[CHARGER]->RequiresStart()){
					devices[CHARGER]->START(); //signal to the charger we are starting
					state++;
					system_mode = SYSTEM_CHARGING;
				}
				
				else if(devices[VEHICLE]->RequiresStart()){
					devices[VEHICLE]->START(); //signal to the vehicle we are starting
					devices[INVERTER]->START(); //signal to the inverter we are starting
					state++;
					system_mode = SYSTEM_DRIVING;
				}
				
			break;
			
			case SYSTEM_PRE_CHARGE_B_MINUS_CHECK:	  // should see zero current as we close pre-charge relay, if a B- contactor used
			
			
			break;
						
			case SYSTEM_PRE_CHARGE_TEST: //should see zero current as we close B- contactor
			
			
			break;
						
			case SYSTEM_PRE_CHARGE_ACTIVE: //close Pre-charge contactor
			
			break;			
			
			case SYSTEM_PRE_CHARGE_COMPLETE://close B+ Contactor
			
			break;			
			
			case SYSTEM_PRE_CHARGE_FAULT:

			break;
			
			case SYSTEM_HV_READY:
				//?
				switch(system_mode){
					case SYSTEM_CHARGING:
						if(devices[CHARGER]->STOPPED()) //charge cycle ended
							state++;
					break;
					
					case SYSTEM_DRIVING:
						if(devices[VEHICLE]->STOPPED()) //drive cycle ended
							state++;					
					break;
					
					default:
						state++;
					break;
					
				}
				
			break;			

			case SYSTEM_DISCONNECT_CONTACTORS:
				timer = 10;
			break;			

			case SYSTEM_DISCONNECT_WAIT: //return to low power state
				if(timer==0)state=SYSTEM_LOW_POWER_IDLE_STATE;
			break;		
			
		}
	
	
	if(timer>0)timer--;
	
	
}

