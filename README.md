# GT20L16J1Y

Arduino Library for GT20L16J1Y Japanese Font ROM.
It can be used with any graphics library using Adafruit GFX Library.

## Supported devices
- [Only IC](https://www.switch-science.com/catalog/2272/)
- [DIP module](https://www.switch-science.com/catalog/2273/)

## Known issue
- it doesn't inherit to Arduino's `Print` class.

## Getting started
### Circuit
Please refer to [Switch Science's Wiki](http://trac.switch-science.com/wiki/KanjiROM)

### Sketch
**Encoding of sketch has to be Shift-JIS to write Japanese directly in the sketch and display it on LCD**

Please use external editor to change encoding of sketch in Arduino environment.

The example code below is to use with my [ili9328SPI](https://github.com/coord-e/ili9328SPI/) library.

(Any graphics library inherits [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) can be used)

```cpp
#include <ili9328.h>
#include <GT20L16J1Y.h>

ili9328SPI tft(2, 3); //CS 2 RST 3 (You can use any Graphics Driver which supports Adafruit GFX Library.)
GT20L16J1Y jfont(&tft, 8); //CS 8

void setup() {
  Serial.begin(9600);

  tft.begin();
  jfont.begin();

  tft.fillScreen(0xFFFF);
  jfont.setTextColor(0x0000);

  tft.setCursor(0, 10);
  jfont.setTextSize(2);
  jfont.println("こんにちは、LCD World!");
  jfont.println("GT20L16J1Y");
  jfont.println("日本語フォントROM");
  jfont.setTextSize(1);

  delay(2000);
}

void loop() {
}
```

## LICENSE
This library is published under MIT LICENSE.

See `LICENSE`.
