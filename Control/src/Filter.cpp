#include <Arduino.h>
#include "Filter.h"

double filterFIR(double dDataIn, double *pdBuffer, uint16_t uiBufLen, uint16_t *puiWriteIndex, double *pdCoeff)
{
    double dDataOut = 0;
    uint16_t uiMovingIndex = 0;

    //Schreiben der neuen Daten an aktuellen Index
    pdBuffer[*puiWriteIndex] = dDataIn;

    //Filter Loop
    for (uint16_t uiM = 0; uiM < uiBufLen; uiM++)
    {
        //Rotierenden Index berechnen Modulo erzeugt einen Ringbuffer
        uiMovingIndex = (uint16_t)(*puiWriteIndex - uiM);
        uiMovingIndex %= uiBufLen;
        dDataOut += pdBuffer[uiMovingIndex] * pdCoeff[uiM];
    }
    *puiWriteIndex = *puiWriteIndex + 1;
    *puiWriteIndex = *puiWriteIndex % uiBufLen;

    return dDataOut;
}
