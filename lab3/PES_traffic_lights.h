#ifndef __PES_TRAFFIC_LIGHTS_H
#define __PES_TRAFFIC_LIGHTS_H

typedef enum {AMBER_BLINKING, RED, RED_AMBER, AMBER, GREEN} tl_state_t;

void traffic_lights_init(const tl_state_t);
void traffic_ligtts_update(void);


#endif