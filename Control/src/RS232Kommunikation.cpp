#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>
#include "RS232Kommunikation.h"

void RS232Task(void* pv)
{
    for (;;)
    {
        //Serial.println("RS232-Task.....");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}