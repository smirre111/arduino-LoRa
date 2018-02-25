// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include <SPI.h>

#define LORA_DEFAULT_SS_PIN    10
#define LORA_DEFAULT_RESET_PIN 9
#define LORA_DEFAULT_DIO0_PIN  2

#define PA_OUTPUT_RFO_PIN      0
#define PA_OUTPUT_PA_BOOST_PIN 1

// Interrupt related
#define LORA_IRQ_DIO0_RXDONE      0x0
#define LORA_IRQ_DIO0_TXDONE      0x1
#define LORA_IRQ_DIO0_CADDONE     0x2

#define LORA_IRQ_DIO1_RXTIMEOUT   0x0
#define LORA_IRQ_DIO1_FHSSCHCH    0x1
#define LORA_IRQ_DIO1_CADDETECTED 0x2

#define LORA_IRQ_DIO2_FHSSCHCH    0x0

#define LORA_IRQ_DIO3_CADDONE     0x0
#define LORA_IRQ_DIO3_VALIDHEADER 0x1
#define LORA_IRQ_DIO3_CRCERROR    0x2

#define LORA_IRQ_DIO4_CADDETECTED 0x0
#define LORA_IRQ_DIO4_PLLLOCK     0x1

#define LORA_IRQ_DIO5_MODEREADY   0x0
#define LORA_IRQ_DIO5_CLKOUT      0x1

#define LORA_IRQ_FLAG_RX_TIMEOUT        B10000000
#define LORA_IRQ_FLAG_RX_DONE           B01000000
#define LORA_IRQ_FLAG_PAYLOAD_CRC_ERROR B00100000
#define LORA_IRQ_FLAG_VALID_HEADER      B00010000
#define LORA_IRQ_FLAG_TX_DONE           B00001000
#define LORA_IRQ_FLAG_CAD_DONE          B00000100
#define LORA_IRQ_FLAG_FHSS_CHANGE_CH    B00000010
#define LORA_IRQ_FLAG_CAD_DETECTED      B00000001

class LoRaClass : public Stream {
public:
  LoRaClass();

  int begin(long frequency);
  void end();

  int beginPacket(int implicitHeader = false);
  int endPacket();

  int parsePacket(int size = 0);
  int packetRssi();
  float packetSnr();

  // from Print
  virtual size_t write(uint8_t byte);
  virtual size_t write(const uint8_t *buffer, size_t size);

  // from Stream
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();

  void onReceive(void(*callback)(int));

  void receive(int size = 0);
  void idle();
  void sleep();
  void cad(); // Channel activity detection

  void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
  void setFrequency(long frequency);
  void setSpreadingFactor(int sf);
  void setSignalBandwidth(long sbw);
  void setCodingRate4(int denominator);
  void setPreambleLength(long length);
  void setSyncWord(uint8_t sw);
  void enableLowDataRateOptimize(bool enabled);
  void enableCrc();
  void disableCrc();

  void setInterruptMode(byte pin, byte mode); // pin: [DIO]0..5; mode: see LORA_IRQ_DIO*
  uint8_t readInterrupts(); // See LORA_IRQ_FLAG_* for testing against a specific one
  
  float compensateFrequencyOffset();

  void clearInterrupts(uint8_t irqFlags);

  // deprecated
  void crc() { enableCrc(); }
  void noCrc() { disableCrc(); }

  byte random();

  void setPins(int ss = LORA_DEFAULT_SS_PIN, int reset = LORA_DEFAULT_RESET_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);
  void setSPIFrequency(uint32_t frequency);

  void dumpRegisters(Stream& out);

private:
  void explicitHeaderMode();
  void implicitHeaderMode();

  void handleDio0RiseRx();

  uint8_t readRegister(uint8_t address);
  void writeRegister(uint8_t address, uint8_t value);
  uint8_t singleTransfer(uint8_t address, uint8_t value);

  static void onDio0RiseRx();

private:
  SPISettings _spiSettings;
  int _ss;
  int _reset;
  int _dio0;
  long _frequency;
  long _bandWidth;
  int _packetIndex;
  int _implicitHeaderMode;
  void (*_onReceive)(int);
};

extern LoRaClass LoRa;

#endif
