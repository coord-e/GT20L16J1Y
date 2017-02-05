/*************************************
* Written in SHIFT-JIS for Testing
* Please view and edit on an editor which can view and edit Shift-JIS encoding file.
* If you can't read the text below, You're not in Shift-JIS editable environment.
*                                   こんにちは。
*
* Testing Program for GT20L16J1Y Japanese Font ROM Driver
*
**************************************/

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
  jfont.println("Hello LCD World!");
  jfont.println("GT20L16J1Y");
  jfont.println("日本語フォントROM");
  jfont.setTextSize(1);

  delay(2000);
}

void loop() {
  waysTest();
  delay(3000);
  sizeTest();
  delay(3000);
  colorTest();
  delay(3000);
}

void waysTest(){
  tft.fillScreen(0xFFFF);
  tft.setCursor(0, 220);
  jfont.println("文の指定方法のテスト - print()");

  tft.setCursor(0, 20);

  jfont.setTextSize(1);
  jfont.println("with uint16_t array");
  jfont.setTextSize(2);
  uint16_t hello_world[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x90a2, 0x8a45, 0x8142, 0x0000};
  jfont.println(hello_world);

  jfont.setTextSize(1);
  jfont.enter();
  jfont.println("with SHIFT-JIS text");
  jfont.setTextSize(2);
  jfont.println("Hello, 世界。");

  jfont.setTextSize(1);
  jfont.enter();
  jfont.println("ASCII with default font");
  jfont.setTextSize(2);
  jfont.setUseGFXASCII(true);
  jfont.println("Hello, 世界。");

  jfont.setUseGFXASCII(false);
  jfont.setTextSize(1);
}

void sizeTest(){
  tft.fillScreen(0xFFFF);
  tft.setCursor(0, 220);
  jfont.println("文字サイズのテスト - setTextSize()");

  tft.setCursor(0, 10);

  for(int i=1; i<5; i++){
    jfont.setTextSize(i);
    char s[50];
    sprintf(s, "これは%dです", i);
    jfont.println(s);
  }

  jfont.setTextSize(1);
}

void colorTest(){
  tft.fillScreen(0xFFFF);
  tft.setCursor(0, 220);
  jfont.println("色のテスト - setTextColor()");

  jfont.setTextSize(2);

  for(uint32_t i=0; i<65535; i+=2500){
    tft.setCursor(0, 10);
    jfont.setTextColor(i);
    jfont.println("文字色");
  }

  for(uint32_t i=0; i<65535; i+=10000){
    tft.setCursor(0, 10);
    jfont.setTextColor(0xFFFF, i);
    jfont.println("背景色");
  }

  jfont.setTextSize(1);
  jfont.setTextColor(0x0000);
}
