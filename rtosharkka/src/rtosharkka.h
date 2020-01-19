/*
 * rtosharkka.h
 *
 *  Created on: 7 Dec 2019
 *      Author: Jussi M Savola
 */

#ifndef SRC_RTOSHARKKA_H_
#define SRC_RTOSHARKKA_H_

#include "zynq_registers.h"
#include <xuartps_hw.h>

#define u1_max 100.0

extern  void ui();
extern  void kontrolli();
extern  void converter();
extern  void ledita_4();
xSemaphoreHandle LEDsem;

volatile float Kp;
volatile float Ki;
volatile float Uref;
volatile float Uact;


extern float set_Kp(float);
extern float set_Ki(float);
extern float set_Uref(float);
extern float set_Uact(float);

extern float get_Kp();
extern float get_Ki();
extern float get_Uref();
extern float get_Uact();

#define BUFFER_SIZE 20


#endif /* SRC_RTOSHARKKA_H_ */
