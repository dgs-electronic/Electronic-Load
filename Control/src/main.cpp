#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>
#include "Messungen.h"
#include "StromAusgabe.h"
#include "RS232Kommunikation.h"

// The LED is attached to pin 13 on the Teensy 4.0
#define LED_PIN 13

#define LED_4 09
#define LED_5 06
#define LED_6 05

xQueueHandle ADC_Queue_Handle = 0;
xQueueHandle RS232Send_Queue_Handle = 0;

void mainTask(void *pv)
{
  struct ADCMessage sQMsgRec;
  uint16_t uiLEDCnt = 0;
  for (;;)
  {
    if (uiLEDCnt < 10)
    {
      digitalWrite(LED_PIN, LOW);
    }
    else
    {
      digitalWrite(LED_PIN, HIGH);
    }

    uiLEDCnt = (uiLEDCnt + 1) % 20;

    //Serial.println("Main-Task.....");

    if (xQueueReceive(ADC_Queue_Handle, &sQMsgRec, 20))
    {
      Serial.print("Analog In 0 Spannung = ");
      Serial.print(sQMsgRec.dStrom1_1A0, 4);
      Serial.println(" V");
    }
    else
    {
      Serial.println("no messages from ADC!");
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

//------------------------------------------------------------------------------
void setup()
{

  pinMode(LED_PIN, OUTPUT);
  Serial.begin(28800);

  //Stromausgabe und DACs initialisieren
  setupStromAusgabe();

  // Queue_Handle generieren um Nchrichten mit dem Messen-Task auszutauschen
  ADC_Queue_Handle = xQueueCreate(10, sizeof(struct ADCMessage));

  //Queue Handle generieren um Nachrichten via RS232 zu verschicken
  RS232Send_Queue_Handle = xQueueCreate(10, sizeof(struct ADCMessage));

  // Task generieren um alles Messungen auszuführen
  xTaskCreate(MessenTask, "ADC-Task", 1024, NULL, 1, NULL);

  // Task generieren um Strom auszugeben
  xTaskCreate(StromAusgabeTask, "DAC-Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  //Task generieren um RS232 Kommunikation abzuwickeln
  xTaskCreate(RS232Task, "RS232-Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  //Haupt-Task generieren
  xTaskCreate(mainTask, "Haupttask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  Serial.println("Starting the scheduler !");

  // start scheduler
  vTaskStartScheduler();
  Serial.println("Insufficient RAM");
  while (1)
    ;
}
//------------------------------------------------------------------------------
// WARNING idle loop has a very small stack (configMINIMAL_STACK_SIZE)
// loop must never block
void loop()
{
  // Not used.
}