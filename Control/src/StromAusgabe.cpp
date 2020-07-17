#include "StromAusgabe.h"
#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>

void StromAusgabeTask(void* pv)
{
    for (;;)
    {
        //Serial.println("Stromausgabe-Task......");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}