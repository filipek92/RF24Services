/*
  RF24Services.cpp - Library for handling callbacs with RF24 library.
  Created by Filip Richter, 30.07.2016.
*/

#include "Arduino.h"
#include "RF24Services.h"

RF24Services::RF24Services(RF24 &rf): _rf(rf)
{
  _irq_pin = NO_IRQ;
}

RF24Services::RF24Services(RF24 &rf, int8_t irq_pin): _rf(rf)
{
  _irq_pin = irq_pin;
  pinMode(_irq_pin, INPUT_PULLUP);
}

void RF24Services::addService(void (*callback)(uint8_t *data, uint8_t len), uint8_t sid)
{
  service_ids[service_cnt] = sid;
  service_ptrs[service_cnt] = callback;
  service_cnt++;
}

void RF24Services::doWork()
{
  unsigned long t = millis();
  if(((t - rxTime) > lostTime) && (connectionState == CONNECTED)){
    connectionState = LOST;
    if(lostCallback) lostCallback();
  }
  if(!irqStatus()) return;
  bool tx,fail,rx;
  uint8_t pipe;
  _rf.whatHappened(tx,fail,rx);
  if(rx){
    rxTime=t;
    if(activeCallback) activeCallback();
    while ( _rf.available(&pipe) )
    {
      uint8_t len = _rf.getDynamicPayloadSize();
      if(!len) continue; 
      _rf.read(buffer, len);
      handlePacket(buffer, len);
    }
    if(inactiveCallback) inactiveCallback();
    if(connectionState==LOST){
      connectionState = CONNECTED;
      if(connectCallback) connectCallback();
    }
  }
}

void RF24Services::handlePacket(uint8_t buffer[], uint8_t len){
  for(uint8_t i=0; i<service_cnt; i++){
    if(service_ids[i]==buffer[0]){
      service_ptrs[i](buffer, len);
      return;
    }
  }
  if(defaultService!=0) defaultService(buffer, len);
}

inline bool RF24Services::irqStatus(){
  if(_irq_pin==NO_IRQ) return true;
  return !digitalRead(_irq_pin);
}

bool RF24Services::send(uint8_t pipe, void *data, uint8_t len){
  if(activeCallback) activeCallback();
  boolean status = false;
  _rf.stopListening();
  _rf.openWritingPipe(pipe+baseAddress);
  status = _rf.write(data, len);
  _rf.startListening();
  if(inactiveCallback) inactiveCallback();
  return status;
}
