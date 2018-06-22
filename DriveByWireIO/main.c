#define PART_TM4C123GH6PM 1;

#include <stdint.h>
#include <stdbool.h>

#include "Globals_and_Defines.h"
#include "initialization.h"

#include "Switches_PORTF.h"
#include "Linear_Actuator.h"
#include "Steering_Actuator.h"
#include "Throttle.h"

#include "DriveByWireIO.h"

#include "CAN_comm.h"



int main()
{
	uint32_t state =  0;
	initialization();
	
	while(driveENABLE)
	{
		if (g_tick_flag == true) //check tick happened
		{
			g_tick_flag = false;   //clear tick_flag
			state = 4;             //just for testing steering always state 4
				switch(state)
				{
					//Idle State
					case 0:
						g_stop_flag = 0;            //clear stop_flag
						PF3 = 0x08;
						if (g_start_flag == true)
						{
							g_start_flag = false;     //clear start_flag
							disable_start();
							state = 1;
							PF3 = 0x00;
						}
						break;
					case 1:
						//en_clutch_lin_act();   //enable clutch on the linear actuator
						state = 2;
						break;
					case 2:
						//en_motor_lin_act();    //enable motor on the linear actuator
						state = 3;
						break;
					case 3:
						//zero_lin_act();        //move linear actuator to zero
						//zero_steering_act();   //move steering actuator to zero
						//zero_throttle();     //set throttle to zero
						state = 4;
						break;
					case 4:
						//if (g_stop_flag == true)
						//{
						//	g_stop_flag = false;   //clear stop flag
						//	state = 5;             //start stop sequence
						//}
						DriveByWireIO();       //read inputs and output to actuators
						PF3 ^= 0x08;
						break;
					case 5:
						disable_start();
						disable_stop();
						PF2 = 0x00;           //clear PF2
						//zero_lin_act();        //move linear actuator to zero
						zero_steering_act();   //move steering actuator to zero
						//zero_throttle();     //set throttle to zero
						state = 6;
						break;
					case 6:
						//disable_motor_lin_act();   //move steering actuator to zero
						state = 7;
						break;
					case 7:
						//disable_clutch_lin_act();
						enable_start(); 
						enable_stop();
						g_start_flag = false;      //clear start flag
						state = 0;                 //return to idle state
						break;
					default:
					//do nothing yet
					break;
			}
		} 
	}
}


	
	
	
	


