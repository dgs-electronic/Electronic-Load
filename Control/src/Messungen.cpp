#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>
#include "Messungen.h"
#include "Filter.h"
#include "main.h"

//AD-Wandler
#define REF_VOLTAGE (3.333)
#define ADRESOLUTION (12)
#define ADSTEPS (((uint16_t)1 << ADRESOLUTION) - 1)
#define ADCDEFAULTGAIN ((double)(REF_VOLTAGE / ADSTEPS))

//AD-Wandler Channelmapping
const u_char ucADCh[6] = {0, 1, 6, 7, 8, 9};

//AD-Wandler Umrechnung in Spannung oder Strom oder....
const double dADScaling[6] = {ADCDEFAULTGAIN, ADCDEFAULTGAIN,
                              ADCDEFAULTGAIN, ADCDEFAULTGAIN,
                              ADCDEFAULTGAIN, ADCDEFAULTGAIN};

//AD-Wandler Kalibrierungsfaktor
const double dADCal[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

//LEDs
#define LED_MESSEN 12

//FIR-Filter
#define FIRLEN 16

double c1[FIRLEN] = {62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03,
                           62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03,
                           62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03,
                           62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03};

/*
Task der alle Messungen vornimmt und auch filtert
*/
void MessenTask(void *pv)
{
    //FIR-Filter
    double f[6][FIRLEN] = {0};
    uint16_t uiIdx[6] = {0};

    double AIN_1V0[6] = {0, 0, 0, 0, 0, 0};
    double AIN_FILTERT_1V0[6] = {0, 0, 0, 0, 0, 0};
    uint16_t uiTimeStamp = 0;
    uint16_t uiLEDCnt = 0;
    struct ADCMessage sQMsg;

    analogReadResolution(ADRESOLUTION);
    pinMode(LED_MESSEN, OUTPUT);

    for (;;)
    {
        if (uiLEDCnt < 5)
        {
            digitalWrite(LED_MESSEN, HIGH);
        }
        else
        {
            digitalWrite(LED_MESSEN, LOW);
        }
        uiLEDCnt = (uiLEDCnt + 1) % 10;

        //Alle Kanäle holen
        for (u_char ucCh = 0; ucCh < 6; ucCh++)
        {
            AIN_1V0[ucCh] = (double)analogRead(ucADCh[ucCh]);
            AIN_1V0[ucCh] *= dADScaling[ucCh];
            AIN_1V0[ucCh] *= dADCal[ucCh];
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        //Filtern
        for (u_char ucCh = 0; ucCh < 6; ucCh++)
        {
            AIN_FILTERT_1V0[ucCh] = filterFIR(AIN_1V0[ucCh], f[ucCh], FIRLEN, &(uiIdx[ucCh]), c1);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        sQMsg.dStrom1_1A0 = AIN_FILTERT_1V0[0];
        sQMsg.dStrom2_1A0 = AIN_FILTERT_1V0[1];
        sQMsg.dStrom3_1A0 = AIN_FILTERT_1V0[2];
        sQMsg.dStrom4_1A0 = AIN_FILTERT_1V0[3];
        sQMsg.dStrom5_1A0 = AIN_FILTERT_1V0[4];
        sQMsg.dSpannung_1V0 = AIN_FILTERT_1V0[5];
        sQMsg.uiTimestamp = uiTimeStamp;
        uiTimeStamp++;

        if (!xQueueSend(ADC_Queue_Handle, &sQMsg, 100))
        {
            Serial.println("Queue läuft über");
        }

        if (!xQueueSend(RS232Send_Queue_Handle, &sQMsg, 100))
        {
            Serial.println("Queue läuft über");
        }
    }
}
