/*
  RF24Services.h - Library for handling callbacs with RF24 library.
  Created by Filip Richter, 30.07.2016.
*/

#ifndef RF24Services_h
#define RF24Services_h

#define SERVICES_COUNT  20
#define BUFFER_LENGTH   33
#define NO_IRQ          -1

#include "Arduino.h"
#include "stdint.h"
#include "RF24.h"

class RF24Services
{
  public:
    RF24Services(RF24 &s);
    RF24Services(RF24 &s, byte irq_pin);
    void addService(void (*callback)(uint8_t *data, uint8_t len), uint8_t sid);
    void doWork();
    inline void setIrqPin(byte irq_pin);
    inline bool IrqStatus();
  private:  
    RF24& _rf; 
    byte _irq_pin;
    void (*service_ptrs[SERVICES_COUNT])(uint8_t *data, uint8_t len);
    byte service_ids[SERVICES_COUNT];
    uint8_t service_cnt=0;
    uint8_t buffer[BUFFER_LENGTH];
};

#endif
