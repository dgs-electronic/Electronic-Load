#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>
#include "RS232Kommunikation.h"
#include "Messungen.h"
#include "main.h"

#define LED_2 11

void RS232Task(void *pv)
{
    uint16_t uiLEDCnt = 0;
    struct ADCMessage sADCin;

    pinMode(LED_2, OUTPUT);
    for (;;)
    {
        if (uiLEDCnt < 50)
        {
            digitalWrite(LED_2, LOW);
        }
        else
        {
            digitalWrite(LED_2, HIGH);
        }
        uiLEDCnt = (uiLEDCnt + 1) % 100;

        if (xQueueReceive(RS232Send_Queue_Handle, &sADCin, 20))
        {
            Serial.print("RS232:AIn0 Spannung = ");
            Serial.print(sADCin.dStrom1_1A0, 4);
            Serial.println(" V");
        }
        else
        {
            Serial.println("no messages from ADC!");
        }

        //Serial.println("RS232-Task.....");
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}