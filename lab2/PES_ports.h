#ifndef __PES_PORTS_H
#define __PES_PORTS_H

//Define output mode low/high
#define HIGH 1
#define LOW 0

#define PB0 0
#define PB1	1
#define PB2 2
#define PB3 3
#define PB4 4
#define PB5 5
#define PB6 6 //Red LED on extension board

#define PC1 7 //yellow LED on extension board

#define PG1 8 //green LES on extension board


int readPin(unsigned char port_pin);
void writePin(unsigned char port_pin, unsigned char mode);

#endif
