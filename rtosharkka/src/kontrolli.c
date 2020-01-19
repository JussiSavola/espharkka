/*
 * task2.c
 *
 *  Created on: 7 Dec 2019
 *      Author: Jussi M Savola
 */

#include "stdlib.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_types.h"


/* LUT includes. */
#include "rtosharkka.h"


/* Controller. This is simply a PI-controller.*/
/* PWM output constructed from controller output driving a led (led brightness as a
visualization of the controller output voltage / input voltage of the converter model). */


// extern void controller()
//u1(k) =u1(k-1)+Ki*h* e(k) // Integral part
// Ki ja Kp tulee UI:lta saakka
//u2(k)=Kp*e(k) // proportional par
// u_ref= read_reference(); // Uref UI:lta (y_ref)
// u_act= read_conv(); // U3 harjoituskuvassa reas_AD_con() (y_act

// call every 100 ms // (x)

// float PI(float iUref, float iUact ,float iKi, float iKp) // defined in Main External
float PI() // defined in Main External
{
float local_Kp, local_Ki;
float local_Uref, local_Uact;

static float u1_old = 0;
float error_new , u1_new;

 local_Kp = get_Kp();
 local_Ki = get_Ki();
 local_Uref = get_Uref();
 local_Uact = get_Uact();


  error_new = local_Uref - local_Uact;
  u1_new = u1_old + local_Ki * error_new;

  if (abs(u1_new)  > u1_max) { // no Saturation
	  u1_new = u1_old;// non saturated value
  }

  u1_old = u1_new;

  return (u1_new + local_Kp * error_new);

}

extern  void kontrolli() {
	const TickType_t freq = pdMS_TO_TICKS( 100 );
	TickType_t wakeTime;


	/* If juu need floating point context, and in conf it
	 *  is not set for all #define configUSE_TASK_FPU_SUPPORT 2
	 *  use the following */

	portTASK_USES_FLOATING_POINT();

	// https://www.freertos.org/a00021.html#xTaskGetTickCount
	wakeTime = xTaskGetTickCount();  // only once initialized

	for( ;; ) {


		PI( );



		// https://www.freertos.org/vtaskdelayuntil.html
		vTaskDelayUntil( &wakeTime, freq );
	}

	}


#if 0
extern  void kontrolli() {
	const TickType_t freq = pdMS_TO_TICKS( 750 );
	TickType_t wakeTime;
	uint16_t looppi;

	/* If juu need floating point context, and in conf it
	 *  is not set for all #define configUSE_TASK_FPU_SUPPORT 2
	 *  use the following */

	// portTASK_USES_FLOATING_POINT();

	// https://www.freertos.org/a00021.html#xTaskGetTickCount
	wakeTime = xTaskGetTickCount();  // only once initialized

	for( ;; ) {
		AXI_LED_DATA =AXI_LED_DATA ^ 0x02;
		while (AXI_BTN_DATA & 0x01) {
						  //AXI_LED_DATA =AXI_LED_DATA & 0x01;
						looppi++;
						}


		// https://www.freertos.org/vtaskdelayuntil.html
		vTaskDelayUntil( &wakeTime, freq );

	}
}
#endif
