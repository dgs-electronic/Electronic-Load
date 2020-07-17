#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>
#include "Messungen.h"
#include "Filter.h"

//AD-Wandler
#define REF_VOLTAGE (3.333)
#define ADRESOLUTION (12)
#define ADSTEPS (((uint16_t)1 << ADRESOLUTION) - 1)

//AD-Wandler Channelmapping
const u_char ucADCh[6] = {0, 1, 6, 7, 8, 9};

//FIR-Filter
#define FIRLEN 16

double c1[FIRLEN] = {62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03,
                     62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03,
                     62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03,
                     62.5e-03, 62.5e-03, 62.5e-03, 62.5e-03};

double f[6][FIRLEN] = {0};

uint16_t uiIdx[6] = {0};

/*
Task der alle Messungen vornimmt und auch filtert
*/
void MessenTask(void *pv)
{
    double AIN_1V0[6] = {0, 0, 0, 0, 0, 0};
    double AIN_FILTERT_1V0[6] = {0, 0, 0, 0, 0, 0};

    analogReadResolution(ADRESOLUTION);

    for (;;)
    {
        Serial.println("Messungen-Task..........");

        //Alle Kanäle holen
        for (u_char ucCh = 0; ucCh < 6; ucCh++)
        {
            AIN_1V0[ucCh] = (double)analogRead(ucADCh[ucCh]);
            AIN_1V0[ucCh] *= (double)REF_VOLTAGE;
            AIN_1V0[ucCh] /= (double)ADSTEPS;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);

        //Filtern
        for (u_char ucCh = 0; ucCh < 6; ucCh++)
        {
            AIN_FILTERT_1V0[ucCh] = filterFIR(AIN_1V0[ucCh], f[ucCh], FIRLEN, &(uiIdx[ucCh]), c1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);

        Serial.print("Analog In 0 Spannung = ");
        Serial.print(AIN_1V0[0], 4);
        Serial.println(" V");
        Serial.print("Analog In 0 Spannung gefiltert= ");
        Serial.print(AIN_FILTERT_1V0[0], 4);
        Serial.println(" V");

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
