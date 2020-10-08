#include "StromAusgabe.h"
#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>
#include <Wire.h>

#define LED_3 10

//Defines für die DACs
//LDAC PIN
#define DAC0 16
#define DAC1 17

#define DAC0_ADR (0x61)
#define DAC1_ADR (0x62)
#define MCP4728_ID (0x60)
#define MCPCMD_WRITE_I2C_ADR (0b011)

//Commands
#define CMD_SINGLEWRITE (0b01011)
#define REFBIT   1
#define PD_BITS  0b00
#define GAIN_BIT 0

void setupStromAusgabe(void)
{
  Wire.begin();
  pinMode(DAC0, OUTPUT);
  pinMode(DAC1, OUTPUT);
  digitalWrite(DAC0, HIGH);
  digitalWrite(DAC1, HIGH);
}


void writeDAC(uint8_t ucDAC, uint8_t ucCh, uint16_t uiOut)
{
  uint16_t uiData = uiOut & 0x0FFF;
  uint8_t ucByte2 = (uint8_t)CMD_SINGLEWRITE << 3;
  uint8_t ucByte3=0;
  uint8_t ucByte4=0;

  ucByte2 |= (ucCh << 1);

  ucByte3 = (uint8_t)REFBIT << 7;
  ucByte3 |= (uint8_t)PD_BITS << 5;
  ucByte3 |= (uint8_t)GAIN_BIT << 4;
  ucByte3 |= (uint8_t)(uiData >> 8);

  ucByte4 = (uint8_t)(uiData & 0x00FF);
  
   
  
  Wire.beginTransmission(0x60 + ucDAC);
  Wire.write(ucByte2);
  Wire.write(ucByte3);
  Wire.write(ucByte4);  
  Wire.endTransmission();
}

void fastwriteDAC(uint8_t ucDAC, uint16_t uiCh1, uint16_t uiCh2, uint16_t uiCh3, uint16_t uiCh4)
{
    uint16_t uiData1 = uiCh1 & 0x0FFF;
    uint16_t uiData2 = uiCh2 & 0x0FFF;
    uint16_t uiData3 = uiCh3 & 0x0FFF;
    uint16_t uiData4 = uiCh4 & 0x0FFF;

    uint8_t ucC1Byte2 = (uint8_t)PD_BITS << 4;
    uint8_t ucC1Byte3 = (uint8_t)(uiData1 & 0x00FF);
    ucC1Byte2 |= (uint8_t)(uiData1 >> 8);

    uint8_t ucC2Byte2 = (uint8_t)PD_BITS << 4;
    uint8_t ucC2Byte3 = (uint8_t)(uiData2 & 0x00FF);
    ucC2Byte2 |= (uint8_t)(uiData2 >> 8);

    uint8_t ucC3Byte2 = (uint8_t)PD_BITS << 4;
    uint8_t ucC3Byte3 = (uint8_t)(uiData3 & 0x00FF);
    ucC3Byte2 |= (uint8_t)(uiData3 >> 8);

    uint8_t ucC4Byte2 = (uint8_t)PD_BITS << 4;
    uint8_t ucC4Byte3 = (uint8_t)(uiData4 & 0x00FF);
    ucC4Byte2 |= (uint8_t)(uiData4 >> 8);


    Wire.beginTransmission(0x60 + ucDAC);
    Wire.write(ucC1Byte2);
    Wire.write(ucC1Byte3);
    Wire.write(ucC2Byte2);
    Wire.write(ucC2Byte3);
    Wire.write(ucC3Byte2);
    Wire.write(ucC3Byte3);
    Wire.write(ucC4Byte2);
    Wire.write(ucC4Byte3);    
    Wire.endTransmission();
}

void StromAusgabeTask(void* pv)
{
    uint16_t uiTestValue = 0;
    pinMode(LED_3, OUTPUT);
    for (;;)
    {
        digitalWrite(DAC0, HIGH);
        digitalWrite(DAC1, HIGH);
        
        //for (uint8_t ucIdx = 0; ucIdx < 4; ucIdx++)
        //{
        //    digitalWrite(LED_3,HIGH);
        //    writeDAC(1,ucIdx,uiTestValue);
        //    writeDAC(2,ucIdx,uiTestValue);
        //    digitalWrite(LED_3,LOW);
        //    vTaskDelay(60 / portTICK_PERIOD_MS);
        //}
        digitalWrite(LED_3,HIGH);
        fastwriteDAC(1,uiTestValue,uiTestValue,uiTestValue,uiTestValue);
        vTaskDelay(20 / portTICK_PERIOD_MS);
        digitalWrite(LED_3,LOW);
        fastwriteDAC(2,uiTestValue,uiTestValue,uiTestValue,uiTestValue);
        vTaskDelay(20 / portTICK_PERIOD_MS);
        digitalWrite(DAC0, LOW);
        digitalWrite(DAC1, LOW);
        uiTestValue = (uiTestValue + 1) % 4096;

        //vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}