#ifndef _GT20L16J1Y_H
#define _GT20L16J1Y_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <SPI.h>
#include <Adafruit_GFX.h>

#define HANKAKU_WIDTH 8
#define ZENKAKU_WIDTH 16
#define WORD_HEIGHT 16

class GT20L16J1Y {

public:

  template <typename T>
  GT20L16J1Y(T gfx, uint8_t cs, long speed = 8000000) : _size(1), _color(0xfffff), _bgcolor(0x0000), _enbg(false), _gfxascii(false)
  {
    _gfx = dynamic_cast<Adafruit_GFX*>(gfx);
    _cs = cs;
    _setting = SPISettings(speed, MSBFIRST, SPI_MODE0);
  }

  void     begin();
  void     setTextSize(uint8_t s);
  void     setTextColor(uint16_t c);
  void     setTextColor(uint16_t c, uint16_t bg);
  void     print(uint16_t *str);
  void     print(char *str);
  void     println(uint16_t *str);
  void     println(char *str);
  void     setUseBackground(bool tf);
  void     setUseGFXASCII(bool tf);
  void     enter();

private:

  void     drawChar(uint8_t code);
  void     drawChar(uint16_t code);
  void     show1byte(uint8_t code);
  uint8_t  charType(char *str, int pos);
  void     show2byte(unsigned short code);
  void     readFontJIS(uint8_t c1, uint8_t c2);
  void     readFontASCII(uint8_t ASCIICODE);
  void     sendDotsToTFT32();
  void     sendDotsToTFT16();

  Adafruit_GFX* _gfx;
  SPISettings _setting;
  uint8_t _cs, _size;
  uint16_t _color, _bgcolor;
  uint8_t _matrixdata32[32]; //16×16用表示データ
  uint8_t _matrixdata16[16]; //16×8用表示データ
  bool _enbg, _gfxascii;
};
#endif // _GT20L16J1Y_H
