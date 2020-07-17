#include <Arduino.h>

double filterFIR(double dDataIn, double* pdBuffer, uint16_t uiBufLen, uint16_t* puiWriteIndex, double* pdCoeff);