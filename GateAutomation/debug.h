/*
 * debug.h
 *
 *  Created on: 29 kwi 2022
 *      Author: jant
 */

#ifndef GATEAUTOMATION_DEBUG_H_
#define GATEAUTOMATION_DEBUG_H_

#define DEBUG_PROG                Serial
#ifdef DEBUG_PROG
    #if defined(ARDUINO_ARCH_ESP32)
        #define DEBUG_MSG_PROG(fmt, ...) { DEBUG_PROG.printf_P((PGM_P) PSTR(fmt), ## __VA_ARGS__); }
    #else
        #define DEBUG_MSG_PROG(fmt, ...) { DEBUG_PROG.printf(fmt, ## __VA_ARGS__); }
    #endif
#else
    #define DEBUG_MSG_PROG(...)
#endif



#endif /* GATEAUTOMATION_DEBUG_H_ */
