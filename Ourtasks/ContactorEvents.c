/******************************************************************************
* File Name          : ContactorEvents.c
* Date First Issued  : 07/01/2019
* Description        : Events in Contactor function w STM32CubeMX w FreeRTOS
*******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "malloc.h"
#include "ADCTask.h"
#include "adctask.h"

#include "contactor_idx_v_struct.h"
#include "ContactorTask.h"
#include "morse.h"

#include "SerialTaskReceive.h"
#include "ContactorTask.h"
#include "can_iface.h"
#include "contactor_hv.h"
#include "contactor_cmd_msg.h"
#include "contactor_msgs.h"
#include "contactor_hv.h"
#include "MailboxTask.h"

/* *************************************************************************
 * void ContactorEvents_00(struct CONTACTORFUNCTION* pcf);
 * @brief	: ADC readings available
 * *************************************************************************/
void ContactorEvents_00(struct CONTACTORFUNCTION* pcf)
{
}

/* *************************************************************************
 * void ContactorEvents_01(struct CONTACTORFUNCTION* pcf);
 * @brief	: HV sensors usart RX line ready
 * *************************************************************************/
uint32_t dbgCE1;
void ContactorEvents_01(struct CONTACTORFUNCTION* pcf)
{
dbgCE1 += 1;
	contactor_hv_uartline(pcf);  // Extract readings from received line
	contactor_hv_calibrate(pcf); // Calibrate raw ADC ticks to scale int volts
	
	xTimerReset(pcf->swtimer3,1); // Reset keep-alive timer
	pcf->evstat &= ~CNCTEVTIMER3;	// Clear timeout bit 
	pcf->evstat |= CNCTEVHV;      // Show new HV readings available
	return;
}
/* *************************************************************************
 * void ContactorEvents_02(struct CONTACTORFUNCTION* pcf);
 * @brief	: (spare)
 * *************************************************************************/
void ContactorEvents_02(struct CONTACTORFUNCTION* pcf)
{
}
/* *************************************************************************
 * void ContactorEvents_03(struct CONTACTORFUNCTION* pcf);
 * @brief	: TIMER3: uart RX keep alive failed
 * *************************************************************************/
void ContactorEvents_03(struct CONTACTORFUNCTION* pcf)
{  // Readings failed to come in before timer timed out.
	pcf->evstat |= CNCTEVTIMER3;	// Set timeout bit 

	pcf->evstat &= ~CNCTEVHV;      // Show new HV readings NOT available

	/* Show uart RX timer timed out, i.e. no readings. */
	pcf->outstat |= CNCTOUTUART3;
	return;
}
/* *************************************************************************
 * void ContactorEvents_04(struct CONTACTORFUNCTION* pcf);
 * @brief	: TIMER1: Command Keep Alive failed (loss of command control)
 * *************************************************************************/
void ContactorEvents_04(struct CONTACTORFUNCTION* pcf)
{
	pcf->evstat |= CNCTEVTIMER1;	// SEt: Show that TIMER1 timed out

	/* Update reset status */
	pcf->evstat |= CNCTEVCMDCN;		

	/* Send status msg as a status heartbeat. */
	contactor_msg_ka(pcf);

	return;
}
/* *************************************************************************
 * void ContactorEvents_05(struct CONTACTORFUNCTION* pcf);
 * @brief	: TIMER2: delay ended
 * *************************************************************************/
void ContactorEvents_05(struct CONTACTORFUNCTION* pcf)
{
	pcf->evstat |= CNCTEVTIMER2;	// Set timeout bit 	
	return;
}
/* *************************************************************************
 * void ContactorEvents_06(struct CONTACTORFUNCTION* pcf);
 * @brief	: CAN: cid_cmd_i (function/diagnostic command/poll)
 * *************************************************************************/
void ContactorEvents_06(struct CONTACTORFUNCTION* pcf)
{
	contactor_cmd_msg_i(pcf); // Build and send CAN msg with data requested
	return;
}
/* *************************************************************************
 * void ContactorEvents_07(struct CONTACTORFUNCTION* pcf);
 * @brief	: CAN: cid_keepalive_i 
 * *************************************************************************/
void ContactorEvents_07(struct CONTACTORFUNCTION* pcf)
{
	pcf->outstat |=  CNCTOUT05KA;  // Output status bit: Show keep-alive
	pcf->evstat  &= ~CNCTEVTIMER1; // Reset timer1 keep-alive timed-out bit

	/* Incoming command byte with command bits */
	uint8_t cmd = pcf->pmbx_cid_keepalive_i->ncan.can.cd.uc[0];

	/* Send status msg in response. */
	contactor_msg_ka(pcf);

	/* Update connect request status bits */
	if ( (cmd & CMDCONNECT) != 0) // Command to connect
	{ // Here, request to connect
		pcf->evstat |= CNCTEVCMDCN;		
	}
	else
	{
		pcf->evstat &= ~CNCTEVCMDCN;		
	}
	/* Update reset status */
	if ( (cmd & CMDRESET ) != 0) // Command to reset
	{ // Here, request to reset
		pcf->evstat |= CNCTEVCMDRS;		
	}
	else
	{
		pcf->evstat &= ~CNCTEVCMDRS;		
	}
	return;
}	
/* *************************************************************************
 * void ContactorEvents_08(struct CONTACTORFUNCTION* pcf);
 * @brief	: CAN: cid_gps_sync: send response CAN msgs
 * *************************************************************************/
void ContactorEvents_08(struct CONTACTORFUNCTION* pcf)
{
	contactor_msg1(pcf, 1); // Send battery string voltage and current
	contactor_msg2(pcf, 1); // Send DMOC+ and DMOC- voltages
	return;
}
	
