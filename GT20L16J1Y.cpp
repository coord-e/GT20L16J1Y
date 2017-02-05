#include "GT20L16J1Y.h"

void GT20L16J1Y::begin(){
  pinMode(_cs, OUTPUT);
  SPI.begin();
}

void GT20L16J1Y::setTextSize(uint8_t s){
  _size = s;
  _gfx->setTextSize(s + 1);
}

void GT20L16J1Y::setTextColor(uint16_t c){
  _color = c;
  _gfx->setTextColor(c);
  _enbg = false;
}

void GT20L16J1Y::setTextColor(uint16_t c, uint16_t bg){
  _color = c;
  _bgcolor = bg;
  _enbg = true;
  _gfx->setTextColor(c, bg);
}

void GT20L16J1Y::setUseBackground(bool tf){
  _enbg = tf;
}

void GT20L16J1Y::setUseGFXASCII(bool tf){
  _gfxascii = tf;
}

void GT20L16J1Y::print(uint16_t *str){ //文字コード配列から描画
  uint16_t nextc = str[0];
  uint16_t c = 0;
  while(nextc != 0x0000){
    if(nextc > 0xFF)
      drawChar(nextc);
    else
      drawChar((uint8_t)nextc);
    nextc = str[++c];
  }
}

void GT20L16J1Y::print(char *str){  //全半角混合char配列から描画
  uint8_t c = 0x00; //全角文字の前半or半角文字
  uint16_t i = 0; //現在位置
  uint16_t cn = 0; //全角開始からのカウント(スキップ用兼)

  do{
    if(cn % 2){ //奇数(全角描画)
      drawChar((uint16_t)(c << 8 | str[i] & 0xFF)); //0b(0000)c(0000)str[i]
    } else { //偶数(半角描画, 全角1Byte取得)
      c = str[i] & 0xFF;
      if(charType(str, i) == 0){ //半角なら
        if(_gfxascii)
          _gfx->print((char)c);
        else
          drawChar((uint8_t)c);
        cn = 1; //全角描画はスキップ
        /*Serial.print(F("Onebyte. char=0x"));
        Serial.println((char)c, HEX);*/
      }else{
        /*Serial.print(F("Not Onebyte. charb=0x"));
        Serial.println((char)c, HEX);*/
        cn = 0; //全角描画スタート
      }
    }
    cn++;
    i++;  //文字は1つずつ進む
  }while(c != 0x00);  //NULLで終了
}

void GT20L16J1Y::println(uint16_t *str){
  print(str);
  enter();
}

void GT20L16J1Y::println(char *str){
  print(str);
  enter();
}

void GT20L16J1Y::drawChar(uint8_t code){
  readFontASCII(code);
  sendDotsToTFT16();
}

/*#define jms1(c) \
(((0x81 <= c) && (c <= 0x9F)) || ((0xE0 <= c) && (c <= 0xFC) ))
#define jms2(c) \
((0x7F != c) && (0x40 <= c) && (c <= 0xFC))*/

/*********************************************************
* 文字列 str の str[pos] について、
*   0 …… 1バイト文字
*   1 …… 2バイト文字の一部(第1バイト)
*   2 …… 2バイト文字の一部(第2バイト)
* のいずれかを返す。
*
* Thanks http://katsura-kotonoha.sakura.ne.jp/prog/c/tip00010.shtml
*********************************************************/
uint8_t GT20L16J1Y::charType(char *str, int pos)
{
	uint8_t state = 0; // { 0, 1, 2 }

	for(uint16_t i = 0; str[i] != '\0'; i++ )
	{
    uint8_t c = str[i];
		if (state == 0 && ((0x81 <= c && c <= 0x9F) || (0xE0 <= c && c <= 0xFC)))
      state = 1; // 0 -> 1
		else if(state == 1 && ((0x7F != c && 0x40 <= c && c <= 0xFC)))
      state = 2; // 1 -> 2
		else if(state == 2 && ((0x81 <= c && c <= 0x9F) || (0xE0 <= c && c <= 0xFC)))
      state = 1; // 2 -> 1
		else
      state = 0; // 2 -> 0, その他

		// str[pos] での状態を返す。
		if ( i == pos ) return state;
	}

	return 0;
}

void GT20L16J1Y::drawChar(uint16_t code){
  //Serial.print("SJIS, 0x"); Serial.print(code, HEX); Serial.print("\t");
  uint8_t c1 = ((code & 0xff00) >> 8);
  uint8_t c2 = (code & 0xFF);
  if (c1 >= 0xe0)
  {
    c1 = c1 - 0x40;
  }
  if (c2 >= 0x80)
  {
    c2 = c2 - 1;
  }
  if (c2 >= 0x9e)
  {
    c1 = (c1 - 0x70) * 2;
    c2 = c2 - 0x7d;
  } else
  {
    c1 = ((c1 - 0x70) * 2) - 1;
    c2 = c2 - 0x1f;
  }
  /*読み出し*/
  readFontJIS(c1, c2);
  /*表示*/
  sendDotsToTFT32();
}
/*1byteのSJISを表示する*/
void GT20L16J1Y::show1byte(uint8_t code)
{
  readFontASCII(code);
  sendDotsToTFT16();
}

void GT20L16J1Y::enter() {
  /*do{
    sendDotsToTFT16();
  } while (_gfx->getCursorX() != 0);*/
  _gfx->setCursor(0, _gfx->getCursorY() + WORD_HEIGHT * _size);
}

/*2byteのSJISを表示する*/
//show2byte(SJIS文字コード)
void GT20L16J1Y::show2byte(unsigned short code)
{
  /*Arduinoのシリアルで日本語はSJIS送信なのでSJIS->JISx0208変換をする*/
  //Serial.print("SJIS, 0x"); Serial.print(code, HEX); Serial.print("\t");
  uint8_t c1 = ((code & 0xff00) >> 8);
  uint8_t c2 = (code & 0xFF);
  if (c1 >= 0xe0)
  {
    c1 = c1 - 0x40;
  }
  if (c2 >= 0x80)
  {
    c2 = c2 - 1;
  }
  if (c2 >= 0x9e)
  {
    c1 = (c1 - 0x70) * 2;
    c2 = c2 - 0x7d;
  } else
  {
    c1 = ((c1 - 0x70) * 2) - 1;
    c2 = c2 - 0x1f;
  }
  /*読み出し*/
  readFontJIS(c1, c2);
  /*表示*/
  sendDotsToTFT32();
}

/*漢字ROMとやりとり*/
//readFontJIS(JIS上位8bit,JIS下位8bit);
void GT20L16J1Y::readFontJIS(uint8_t c1, uint8_t c2)
{
  /*jisx変換後の表示*/
  //Serial.print("jisx up8 = 0x"); Serial.print(c1, HEX); Serial.print("\t");
  //Serial.print("jisx down8 = 0x"); Serial.print(c2, HEX); Serial.print("\t");
  /*jisxの区点を求める*/
  uint32_t MSB = c1 - 0x20;//区
  uint32_t LSB = c2 - 0x20;//点
  /*JISの句点番号で分類*/
  uint32_t Address = 0;
  //Serial.print("MSB = d"); Serial.print(MSB, DEC); Serial.print("\t");
  //Serial.print("LSB = d"); Serial.print(LSB, DEC); Serial.print("\t");
  /*各種記号・英数字・かな(一部機種依存につき注意,㍍などWindowsと互換性なし)*/
  if (MSB >= 1 && MSB <= 15 && LSB >= 1 && LSB <= 94)
  {
    Address = ( (MSB - 1) * 94 + (LSB - 1)) * 32;
  }
  /*第一水準*/
  if (MSB >= 16 && MSB <= 47 && LSB >= 1 && LSB <= 94)
  {
    Address = ( (MSB - 16) * 94 + (LSB - 1)) * 32 + 43584;
  }
  /*第二水準*/
  if (MSB >= 48 && MSB <= 84 && LSB >= 1 && LSB <= 94)
  {
    Address = ((MSB - 48) * 94 + (LSB - 1)) * 32 + 138464;
  }
  //
  /*GT20L16J1Y内部では1区と同等の内容が収録されている*/
  if (MSB == 85 && LSB >= 0x01 && LSB <= 94)
  {
    Address = ((MSB - 85) * 94 + (LSB - 1)) * 32 + 246944;
  }
  /*GT20L16J1Y内部では2区、3区と同等の内容が収録されている*/
  if (MSB >= 88 && MSB <= 89 && LSB >= 1 && LSB <= 94)
  {
    Address = ((MSB - 88) * 94 + (LSB - 1)) * 32 + 249952;
  }

  /*漢字ROMにデータを送信*/
  SPI.beginTransaction(_setting);
  digitalWrite(_cs, HIGH);
  //Serial.print("Address = "); Serial.println(Address, HEX);
  digitalWrite(_cs, LOW);  //通信開始
  SPI.transfer(0x03);
  SPI.transfer(Address >> 16  & 0xff);
  SPI.transfer(Address >> 8   & 0xff);
  SPI.transfer(Address        & 0xff);
  /*漢字ROMからデータを受信*/
  for (int i = 0; i < 32; i++)
  {
    _matrixdata32[i] = SPI.transfer(0x00);
  }
  digitalWrite(_cs, HIGH); //通信終了
  SPI.endTransaction();
}//spireadfont

/*漢字ROMとやりとり*/
//readFontASCII(ASCIIコード);
void GT20L16J1Y::readFontASCII(uint8_t ASCIICODE)
{
  //Serial.print("ASCII,0x");Serial.print(ASCIICODE, HEX); //10/07
  uint32_t Address = 0;
  /*ASCII文字*/
  if (ASCIICODE >= 0x20 && ASCIICODE <= 0x7F)
  {
    Address = ( ASCIICODE - 0x20) * 16 + 255968;
  }
  /*漢字ROMにデータを送信*/
  SPI.beginTransaction(_setting);
  digitalWrite(_cs, HIGH);
  //Serial.print("  Address = "); Serial.println(Address, HEX);
  digitalWrite(_cs, LOW);  //通信開始
  SPI.transfer(0x03);
  SPI.transfer(Address >> 16  & 0xff);
  SPI.transfer(Address >> 8   & 0xff);
  SPI.transfer(Address        & 0xff);
  /*漢字ROMからデータを受信*/
  for (int i = 0; i < 16; i++)
  {
    _matrixdata16[i] = SPI.transfer(0x00);
  }
  digitalWrite(_cs, HIGH); //通信終了
  SPI.endTransaction();
}

/*TFTへ16*16のデータを表示する*/
void GT20L16J1Y::sendDotsToTFT32()
{
  /*上半分*/
  for (int i = 0; i < 8; i++)
  {
    for (int b = 0; b < 16; b++)
    {
      char byteDigit = (1 << i);
      if (_matrixdata32[b] & byteDigit)
      {
        _gfx->fillRect(_gfx->getCursorX() + b * _size, _gfx->getCursorY() + i * _size, _size, _size, _color);
        //_gfx->drawPixel(_gfx->getCursorX()+b,_gfx->getCursorY()+i,BLACK);
      } else
      {
        if(_enbg)
          _gfx->fillRect(_gfx->getCursorX() + b * _size, _gfx->getCursorY() + i * _size, _size, _size, _bgcolor);
        //_gfx->drawPixel(_gfx->getCursorX()+b,_gfx->getCursorY()+i,WHITE);
      }
    }
  }
  /*下半分*/
  for (int i = 0; i < 8; i++)
  {
    for (int b = 16; b < 32 ; b++)
    {
      char byteDigit = (1 << i);
      if (_matrixdata32[b] & byteDigit)
      {
        _gfx->fillRect(_gfx->getCursorX() + (b - 16)*_size, _gfx->getCursorY() + (i + 8)*_size, _size, _size, _color);
        //_gfx->drawPixel(_gfx->getCursorX()+b-16,_gfx->getCursorY()+i+8,BLACK);
      } else
      {
        if(_enbg)
          _gfx->fillRect(_gfx->getCursorX() + (b - 16)*_size, _gfx->getCursorY() + (i + 8)*_size, _size, _size, _bgcolor);
        //_gfx->drawPixel(_gfx->getCursorX()+b-16,_gfx->getCursorY()+i+8,WHITE);
      }
    }
  }
  /*描画位置をずらす処理*/
  _gfx->setCursor( _gfx->getCursorX() + ZENKAKU_WIDTH * _size, _gfx->getCursorY());
  if (_gfx->getCursorX() >= _gfx->width() - 15)
  {
    _gfx->setCursor(0, _gfx->getCursorY() + WORD_HEIGHT * _size);
  }
  if (_gfx->getCursorY() >= _gfx->height())
  {
    _gfx->setCursor(_gfx->getCursorX(), 0);
  }
}//sendDataToSerial32

/*TFTへ16*8のデータを表示する*/
void GT20L16J1Y::sendDotsToTFT16()
{
  /*上半分*/
  for (int i = 0; i < 8; i++)
  {
    for (int b = 0; b < 8; b++)
    {
      char byteDigit = (1 << i);
      if (_matrixdata16[b] & byteDigit)
      {
        _gfx->fillRect(_gfx->getCursorX() + b * _size, _gfx->getCursorY() + i * _size, _size, _size,_color);
      } else
      {
        if(_enbg)
          _gfx->fillRect(_gfx->getCursorX() + b * _size, _gfx->getCursorY() + i * _size, _size, _size, _bgcolor);
      }
    }
  }
  /*下半分*/
  for (int i = 0; i < 8; i++)
  {
    for (int b = 8; b < 16; b++)
    {
      char byteDigit = (1 << i);
      if (_matrixdata16[b] & byteDigit)
      {
        _gfx->fillRect(_gfx->getCursorX() + (b - 8)*_size, _gfx->getCursorY() + (i + 8)*_size, _size, _size,_color);
      } else
      {
        if(_enbg)
          _gfx->fillRect(_gfx->getCursorX() + (b - 8)*_size, _gfx->getCursorY() + (i + 8)*_size, _size, _size, _bgcolor);
      }
    }
  }
  /*描画位置をずらす処理*/
  _gfx->setCursor(_gfx->getCursorX() + HANKAKU_WIDTH * _size, _gfx->getCursorY());
  if (_gfx->getCursorX() >= _gfx->width())
  {
    _gfx->setCursor(0, _gfx->getCursorY() + WORD_HEIGHT * _size);
  }
  if (_gfx->getCursorY() >= _gfx->height())
  {
    _gfx->setCursor(_gfx->getCursorX(), 0);
  }
}//sendDataToSerial32
