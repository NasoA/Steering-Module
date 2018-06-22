#define PART_TM4C123GH6PM 1;

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/can.h"

#include "Steering_Actuator.h"
#include "Globals_and_Defines.h"
	

int average = 0;

void CAN_Setup(void)
{
	tCANMsgObject sMsgObjectRx;
	//Enable PORTE
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
	{}

	// Configure the GPIO pin muxing to select CAN0 functions for these pins.
	// This step selects which alternate function is available for these pins.
	GPIOPinConfigure(GPIO_PE4_CAN0RX);
	GPIOPinConfigure(GPIO_PE5_CAN0TX);

	//Configure the pins for CAN
	GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	
	// Enable the CAN0 module.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0))
	{} 
	CANInit(CAN0_BASE);

	// 250Kbit CAN speed
	CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 250000);
		
	CANIntEnable(CAN0_BASE, CAN_INT_MASTER);
	IntEnable(INT_CAN0);

	CANEnable(CAN0_BASE);
		
	sMsgObjectRx.ui32MsgID = 0x18FF0313;
	sMsgObjectRx.ui32MsgIDMask = 0;
	sMsgObjectRx.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	sMsgObjectRx.ui32MsgLen = 8;
	CANMessageSet(CAN0_BASE, 2, &sMsgObjectRx, MSG_OBJ_TYPE_RX);
}

void CAN0_Handler(void)
{
	uint32_t int_status, CAN_status;
	uint8_t data_array[8];
	tCANMsgObject sMsgObjectRx;
	sMsgObjectRx.pui8MsgData = data_array;
	//get interrupt status
	int_status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);  
	//get CAN status
	CAN_status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
	if (int_status == 0x2)   //interrupt was generated by receive object
	{
		CANMessageGet(CAN0_BASE, 2, &sMsgObjectRx, true);   //get received data
		g_new_CAN_data = true;
			if(sMsgObjectRx.ui32MsgID == 0x18FF0313)
			{                                                                              
				if ((((data_array[2] > 0x80 && data_array[2] < 0xF0))&& data_array[3] == 0x00)||//RIGHT INTERVAL 
					(data_array[2] > 0x30 && data_array[2] < 0x80&& data_array[3] == 0xFF)) 			//LEFT INTERVAL
					{
						average++;
						if (average > 3)
						{
							average = 0;
							PF2 ^= 0x04;
							zero_steering_act();
							driveENABLE = false;
						}
					}else
			average = 0;
		}
	}
}





