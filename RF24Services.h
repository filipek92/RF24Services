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
    RF24Services(RF24 &s, int8_t irq_pin);
    void addService(void (*callback)(uint8_t *data, uint8_t len), uint8_t sid);
    void setDefaultService(void (*callback)(uint8_t *data, uint8_t len));
    void setActCallbacks(void (*active)(), void (*inactive)());
    void doWork();
    bool send(uint64_t pipe, void *data, uint8_t len);
    inline bool irqStatus();
    unsigned long rxTime();
  private:  
    void handlePacket(uint8_t buffer[], uint8_t len);
    RF24& _rf; 
    int8_t _irq_pin;
    void (*service_ptrs[SERVICES_COUNT])(uint8_t *data, uint8_t len);
    void (*default_service)(uint8_t *data, uint8_t len);
    void (*active_callback)();
    void (*inactive_callback)();
    uint8_t service_ids[SERVICES_COUNT];
    uint8_t service_cnt=0;
    uint8_t buffer[BUFFER_LENGTH];
    unsigned long rx_time;
};

#endif
