/*
 * Main.h
 *
 * Created: 12/03/2017 11:54:14 AM
 *  Author: Juan Bernal
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include "Com/nrf24l01.h"
#include "Com/UART/UART.h"
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/queue.h"
#include "Soil/SoilSensor.h"
#include "Humidity/dht11.h"
#include "Pressure/bmp085.h"
#include "Clock/ds1307.h"

#include <avr/io.h>
#include <avr/interrupt.h>

 struct I2CMessage
 {
	 uint8_t data1;
	 uint8_t data2;
 } xMessage;

 struct DigSensorMessage
 {
	 uint32_t data1;
	 uint8_t data2;
 } xDigMessage;
 struct HumidityMessage
 {
	 uint8_t data1;
	 uint8_t data2;
 } xHumMessage;


#endif /* MAIN_H_ */