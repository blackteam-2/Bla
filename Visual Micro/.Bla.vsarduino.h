/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Arduino Mega 2560 or Mega ADK, Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
void running();
void program();
void setLED(int q);
void boo();

#include "E:\Dropbox\Projects\arduino-1.0.5\hardware\arduino\variants\mega\pins_arduino.h" 
#include "E:\Dropbox\Projects\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "E:\Dropbox\Projects\Airsoft prop thing\Bla\Bla.ino"
