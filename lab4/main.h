#ifndef __MAIN_H
#define __MAIN_H

#define CLK_FREQ 40000000
#define OSC_PER_INST 4
#define FCYC (CLK_FREQ/OSC_PER_INST)

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed short long int int24_t;
typedef signed long int int32_t;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned short long int uint24_t;
typedef unsigned long int uint32_t;

#endif