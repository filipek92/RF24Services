/*
  RF24Services.cpp - Library for handling callbacs with RF24 library.
  Created by Filip Richter, 30.07.2016.
*/

#include "Arduino.h"
#include "RF24Services.h"

RF24Services::RF24Services(RF24 &rf): _rf(rf)
{
  _irq_pin = NO_IRQ;
  default_service = 0;
}

RF24Services::RF24Services(RF24 &rf, int8_t irq_pin): _rf(rf)
{
  _irq_pin = irq_pin;
  pinMode(_irq_pin, INPUT);
  default_service = 0;
}

void RF24Services::addService(void (*callback)(uint8_t *data, uint8_t len), uint8_t sid)
{
  service_ids[service_cnt] = sid;
  service_ptrs[service_cnt] = callback;
  service_cnt++;
}

void RF24Services::setDefaultService(void (*callback)(uint8_t *data, uint8_t len))
{
  default_service = callback;
}

void RF24Services::setActCallbacks(void (*active)(), void (*inactive)()){
  active_callback = active;
  inactive_callback = inactive;
}

void RF24Services::setBaseAddress(uint64_t base){
  base_adr = base;
}

void RF24Services::doWork()
{
  if(!irqStatus()) return;
  rx_time=millis();
  bool tx,fail,rx;
  uint8_t pipe;
  _rf.whatHappened(tx,fail,rx);
  if(rx){
    if(active_callback) active_callback();
    while ( _rf.available(&pipe) )
    {
      uint8_t len = _rf.getDynamicPayloadSize();
      if(!len) continue; 
      _rf.read(buffer, len);
      handlePacket(buffer, len);
    }
    if(inactive_callback) inactive_callback();
  }
}

void RF24Services::handlePacket(uint8_t buffer[], uint8_t len){
  for(uint8_t i=0; i<service_cnt; i++){
    if(service_ids[i]==buffer[0]){
      service_ptrs[i](buffer, len);
      return;
    }
  }
  if(default_service!=0) default_service(buffer, len);
}

unsigned long RF24Services::rxTime(){
  return rx_time;
}

inline bool RF24Services::irqStatus(){
  if(_irq_pin==NO_IRQ) return true;
  return !digitalRead(_irq_pin);
}

bool RF24Services::send(uint8_t pipe, void *data, uint8_t len){
  if(active_callback) active_callback();
  boolean status = false;
  _rf.stopListening();
  _rf.openWritingPipe(pipe+base_adr);
  status = _rf.write(data, len);
  _rf.startListening();
  if(inactive_callback) inactive_callback();
  return status;
}
