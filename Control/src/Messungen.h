#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>

struct ADCMessage 
{
    uint16_t uiTimestamp;
    double dStrom1_1A0;
    double dStrom2_1A0;
    double dStrom3_1A0;
    double dStrom4_1A0;
    double dStrom5_1A0;
    double dSpannung_1V0;
};

void MessenTask(void* pv);