/******************************************************************************
* File Name          : contactor_idx_v_struct.h
* Date First Issued  : 06/26/2019
* Board              :
* Description        : Load parameter struct
*******************************************************************************/

#include <stdint.h>
#include "common_can.h"
#include "iir_filter_lx.h"

#ifndef __CONTACTOR_IDX_V_STRUCT
#define __CONTACTOR_IDX_V_STRUCT

/* Hardware configuration option bit assignments. */
#define AUX1PRESENT   (1 << 0)  // 1 = Contactor #1 has auxilary contacts
#define AUX1SENSE     (1 << 1)  // 1 = Aux I/O bit with Contactor #1 closed
#define AUX2PRESENT   (1 << 2)  // 1 = Contactor #2 has auxilary contacts
#define AUX2SENSE     (1 << 3)  // 1 = Aux I/O bit with Contactor #2 closed
#define ONECONTACTOR  (1 << 4)  // 1 = One contactor; one small pre-charge relay
#define PWMCONTACTOR1 (1 << 5)  // 1 = PWM'ing is used on coil #1
#define PWMCONTACTOR2 (1 << 6)  // 1 = PWM'ing is used on coil #2

/* Calibration parameter, float */
struct CNTCTCALF
{
	float offset;
	float scale;
};
/* Calibration parameters, scaled integer */
struct CNTCTCALSI
{
	int32_t offset;
	int32_t scale;
};

/* Parameters contactor instance */
struct CONTACTORLC
 {
	uint32_t size;			// Number of items in struct
 	uint32_t crc;			// crc-32 placed by loader
	uint32_t version;		// struct version number
	float fdiffb4;       // hv1-hv2 voltage difference before closing (volts)

/* NOTE: 
   - all suffix _t parameters are times in milliseconds
   - all voltages are in volts; prefix 'f' designates float
	
/* Hardware configuration option bits. */
   uint32_t hwconfig;

/* Command/Keep-alive CAN msg timeout duration. */
	uint32_t ka_t;       // (e.g. 1000 ms)

/* uart RX keep-alive timeout duration. */
   uint32_t ksRX_t;     // (e.g. 10 ms)

/* In the disconnect state the battery string voltage must be above the following. */
	float fbattlow;      // Minimum battery volts required to connect

/* Contactor #1 closure time delay */
	uint32_t close1_t;   // Delay: contactor #1 coil energize->closed

/* Contactor #2, or pre-charge relay, closure time delay */
	uint32_t close2_t;   // Delay: #2 coil energize->closed
   
/* Contactor #1 open time delay */
	uint32_t open1_t;   // Delay: contactor #1 coil de-energize->open

/* Contactor #2, or pre-charge relay, open time delay */
	uint32_t open2_t;   // Delay: #2 coil de-energize->open

/* Mininum pre-charge delay (befor monitoring voltage) */
   uint32_t prechgmin_t; // Minimum pre-charge duration

/* Maximum pre-charge wait, after prechgmin_t, for voltage threshold
   to be reached. */
   uint32_t prechgvwait_t; // Maximum allowed for voltage to reach threshold

/* With two contactor config, (hv1-hv2) max when contactor #1 closes */
	float fhv1mhv2max;

/* PWM durations (0.0- 100.0) */
   float  fpwmpct1;     // Percent PWM after closure delay at 100% coil #1
   float  fpwmpct2;     // Percent PWM after closure delay at 100% coil #2

/* Message timings. */
	uint32_t keepalive_t;// keep-alive timeout (timeout delay ms)
	uint32_t hbct1_t;		// Heartbeat ct: ticks between sending msgs hv1:cur1
	uint32_t hbct2_t;		// Heartbeat ct: ticks between sending msgs hv2:cur2
	uint32_t hbct3_t;		// Heartbeat ct: ticks between sending msgs hv3 (if two contactors)

	float   fkr;         // Constant: heat loss of pre-charge R
	int32_t toohot;      // Threshold of summation for pre-charge R too hot

/* Calibrations (offset, scale) */
	struct CNTCTCALF fcalcur1; // Motor current
	struct CNTCTCALF fcalcur2; // spare
	struct CNTCTCALF fcalhv1;  // Battery_minus-to-contactor #1 Battery_plus
	struct CNTCTCALF fcalhv2;  // Battery_minus-to-contactor #1 DMOC_plus
	struct CNTCTCALF fcalhv3;  // Battery_minus-to-contactor #2 DMOC_minus

/* Send CAN ids  */
	uint32_t cid_hb1;    // CANID-Heartbeat msg volt1:cur1 (volts:amps)
	uint32_t cid_hb2;    // CANID-Heartbeat msg volt2:cur2 (volts:amps)
   uint32_t cid_msg1;   // CANID-contactor poll response msg: volt1:cur1 (volts:amps)
   uint32_t cid_msg1;   // CANID-contactor poll response msg: volt2:cur2 (volts:amps)
	uint32_t cid_cmd_r;  // CANID_CMD_CNTCTR1R
	uint32_t cid_keepalive_r; // CANID-keepalive response (status)

/* Receive CAN ids List of CAN ID's for setting up hw filter */
	uint32_t cid_cmd_i;       // CANID_CMD: incoming command
	uint32_t cid_keepalive_i;// CANID-keepalive connect command
	uint32_t cid_gps_sync;    // CANID-GPS time sync msg (poll msg)
	uint32_t code_CAN_filt[CANFILTMAX-3];// Spare
 };

/* *************************************************************************/
static void contactor_idx_v_struct_hardcode_params(struct struct CONTACTORLC* p);
/* @brief	: Init struct from hard-coded parameters (rather than database params in highflash)
 * @return	: 0
 * *************************************************************************/
 
#endif
