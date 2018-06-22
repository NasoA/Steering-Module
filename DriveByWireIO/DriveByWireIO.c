#define PART_TM4C123GH6PM 1;

#include <stdint.h>
#include <stdbool.h>

#include "Globals_and_Defines.h"
#include "initialization.h"

#include "Joystick.h"

#include "Switches_PORTF.h"
#include "Linear_Actuator.h"
#include "Steering_Actuator.h"
#include "Throttle.h"



/************************************************************************
 *
 * calc_steering_pos called in DriveByWireIO()
 * Used to calculate steering potion by scaling the input ADC value.
 * add Position by 2^11 (2048). 
 * Clear all bits but the last 12 via anding by 0x000000FFF
 * Logic shift left by 20 bits, Divide by 0x00020000 and multiply by 100
 * Add 360k
 *
 ************************************************************************/
int32_t calc_steering_pos(uint32_t position)
{
	int32_t	scaled_position = (((position + 2048)&0x00000FFF)<<20); 
    scaled_position = (scaled_position/131072)*100;               
    scaled_position += 360000;                                 
  return scaled_position;
}

//Throttle scaling
uint32_t calc_throttle_pos(uint32_t position)
{
	uint32_t digital_pot_position;
	digital_pot_position = position;
	return digital_pot_position;
}

//Linear actuator scaling
uint32_t calc_linear_act_pos(uint32_t position)
{
	return position;
}


/********************************************************************
 *
 * Drive by wire: Calculating position/moving actuators
 * ADC_Values[]: 0- Steering | 1 - Throttle | 2 - Brake act
 * 
 ********************************************************************/
void DriveByWireIO(void)                                         
{
	uint32_t ADC_values[3];
	int32_t steering_pos;
	uint32_t throttle_pos, linear_act_pos;
	
	get_contrlr_inputs(ADC_values);
	
	steering_pos = calc_steering_pos(ADC_values[0]);
	throttle_pos = calc_throttle_pos(ADC_values[1]);
	linear_act_pos = calc_linear_act_pos(ADC_values[2]);

	moveto_steering_act(steering_pos);
	//moveto_throttle(throttle_pos);
	//moveto_lin_act(linear_act_pos);
}

