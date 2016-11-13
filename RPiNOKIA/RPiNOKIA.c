/*Esta biblioteca para Raspberry Pi utiliza-se dos códigos do artigo: http://playground.arduino.cc/Code/PCD8544
E a mesma ainda está em desenvolvimento por Calebe94(Parcialmente terminada)

  +-----------------------------------------------+
  |()___________________________________________()|
  |	|										    | |
  |	|										    | |
  |	| +---------------------------------------+ | |
  |	| |								  	      | | |
  |	| |			  LCD NOKIA 5510	  		  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |				  						  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| |										  | | |
  |	| +---------------------------------------+ | |
  | +-------------------------------------------+ |
  |()      [RST][CE][DC][Din][Vcc][BL][GND]     ()|
  +-----------------------------------------------+
*/
#include<stdint.h>
#include<stdlib.h>
#include"RPiNOKIA.h"
//#include"Fonts.h"
void shiftOut(const int lcd,uint8_t dataPin,uint8_t clockPin,uint8_t bitOrder,uint8_t value){
  uint8_t valor;
  GPIOWrite(clockPin,LOW);
  for(int cont=0;cont<8;cont++){
    if(bitOrder == MSBFIRST){
      valor = value&(1 <<(7-cont));
      valor = valor>0?1:0;
      GPIOWrite(dataPin,valor);
	  }else{
      valor = value&(1 <<(cont));
      valor = valor>0?1:0;
      GPIOWrite(dataPin,valor);
	  }
	  GPIOWrite(clockPin,HIGH);
    //delay(1);
    GPIOWrite(clockPin,LOW);
  }
}
/*	Tentativa de desenhar Bitmaps  */
void NOKIABitmap(const int lcd,uint8_t *logo){
	uint8_t buffer[LCD_X * LCD_Y / 8] = {0,},lines = 6;
	for(unsigned int count = 0 ; count < LCD_X * LCD_Y / 8; count++){
		buffer[count] = logo[count];
	}
	//NOKIADisplay();
	
	for(unsigned int line = 0 ; line < 6; line++){
		NOKIAWrite(lcd,LOW,PCD8544_SETYADDR | line)	;
		
		NOKIAWrite(lcd,LOW,PCD8544_SETXADDR | 0);
		for(unsigned int col=0; col < LCD_X ; col ++){
			NOKIAWrite(lcd,HIGH,buffer[(LCD_X*line)+col]);
		}
	}
	NOKIAWrite(lcd,LOW,PCD8544_SETYADDR);
}
/* 	Tentativa de desenhar Bitmaps	*/


void NOKIAWrite(const int lcd, uint8_t dc, uint8_t data){
  GPIOWrite(NOKIA_LCD[lcd].PIN_DC, dc);
  GPIOWrite(NOKIA_LCD[lcd].PIN_SCE, LOW);
  shiftOut(lcd,NOKIA_LCD[lcd].PIN_SDIN,NOKIA_LCD[lcd].PIN_SCLK, MSBFIRST, data);
  GPIOWrite(NOKIA_LCD[lcd].PIN_SCE, HIGH);
}
void NOKIACharacter(const int lcd,char character){
  NOKIAWrite(lcd,LCD_D, 0x00);
  for (int index = 0; index < 5; index++) NOKIAWrite(lcd,LCD_D, ASCII[character - 0x20][index]);
  NOKIAWrite(lcd,LCD_D, 0x00);
}
void NOKIAClear(const int lcd){
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++){
    NOKIAWrite(lcd,LCD_D, 0x00);
  }
}
int NOKIAInit(unsigned const int PIN_SCE,unsigned const int PIN_RESET,unsigned const int PIN_DC,unsigned const int PIN_SDIN,unsigned const int PIN_SCLK){
  static int NOKIA_LCDINIT = -1;
  static int initialised = 0 ;
  if(initialised == 0){
    initialised = 1;
    for(int cont = 0 ; cont < MAX_LCDS ; cont ++){
      NOKIA_LCD[cont].PIN_SCE = 0;
      NOKIA_LCD[cont].PIN_RESET = 0;
      NOKIA_LCD[cont].PIN_DC = 0;
      NOKIA_LCD[cont].PIN_SDIN = 0;
      NOKIA_LCD[cont].PIN_SCLK = 0;
    }
  }
  for(int cont = 0;cont<MAX_LCDS;cont++){
    if(NOKIA_LCD[cont].PIN_SCE == 0 && NOKIA_LCD[cont].PIN_RESET == 0 && NOKIA_LCD[cont].PIN_DC == 0 && NOKIA_LCD[cont].PIN_SDIN == 0 &&   NOKIA_LCD[cont].PIN_SCLK == 0){
      NOKIA_LCDINIT = cont;
      break;
    }
  }
  if(NOKIA_LCDINIT == -1) return -1; // Se reprovar nestes testes não inicia o display
  else{

  NOKIA_LCD[NOKIA_LCDINIT].PIN_SCE = PIN_SCE;
  NOKIA_LCD[NOKIA_LCDINIT].PIN_RESET = PIN_RESET;
  NOKIA_LCD[NOKIA_LCDINIT].PIN_DC = PIN_DC;
  NOKIA_LCD[NOKIA_LCDINIT].PIN_SDIN = PIN_SDIN;
  NOKIA_LCD[NOKIA_LCDINIT].PIN_SCLK = PIN_SCLK;

  GPIOExport(NOKIA_LCD[NOKIA_LCDINIT].PIN_SCE);		GPIODirection(NOKIA_LCD[NOKIA_LCDINIT].PIN_SCE,0);
  GPIOExport(NOKIA_LCD[NOKIA_LCDINIT].PIN_RESET);	GPIODirection(NOKIA_LCD[NOKIA_LCDINIT].PIN_RESET,0);
  GPIOExport(NOKIA_LCD[NOKIA_LCDINIT].PIN_DC);		GPIODirection(NOKIA_LCD[NOKIA_LCDINIT].PIN_DC,0);
  GPIOExport(NOKIA_LCD[NOKIA_LCDINIT].PIN_SDIN);	GPIODirection(NOKIA_LCD[NOKIA_LCDINIT].PIN_SDIN,0);
  GPIOExport(NOKIA_LCD[NOKIA_LCDINIT].PIN_SCLK);	GPIODirection(NOKIA_LCD[NOKIA_LCDINIT].PIN_SCLK,0);
  // Inicialização do Display

  GPIOWrite(NOKIA_LCD[NOKIA_LCDINIT].PIN_RESET, LOW);
  GPIOWrite(NOKIA_LCD[NOKIA_LCDINIT].PIN_RESET, HIGH);

  NOKIAWrite(NOKIA_LCDINIT, LCD_CMD, 0x21 );  // LCD Extended Commands.
  NOKIAWrite(NOKIA_LCDINIT, LCD_CMD, 0xBf );  // Set LCD Vop (Contrast). //B1
  NOKIAWrite(NOKIA_LCDINIT, LCD_CMD, 0x04 );  // Set Temp coefficent. //0x04
  NOKIAWrite(NOKIA_LCDINIT, LCD_CMD, 0x14 );  // LCD bias mode 1:48. //0x13
  NOKIAWrite(NOKIA_LCDINIT, LCD_CMD, 0x0C );  // LCD in normal mode. 0x0d for inverse
  NOKIAWrite(NOKIA_LCDINIT, LCD_C,   0x20);
  NOKIAWrite(NOKIA_LCDINIT, LCD_C,   0x0C);

  /*NEW PARAMETERS: Draw Bitmap */

  NOKIAWrite(NOKIA_LCDINIT,LOW,PCD8544_FUNCTIONSET    | PCD8544_EXTENDEDINSTRUCTION);
  NOKIAWrite(NOKIA_LCDINIT,LOW,PCD8544_SETBIAS     	  | 0x4);
  NOKIAWrite(NOKIA_LCDINIT,LOW,PCD8544_FUNCTIONSET);
  NOKIAWrite(NOKIA_LCDINIT,LOW,PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
}

  return NOKIA_LCDINIT;
}
void NOKIAString(const int lcd,char *characters){
  while (*characters){
    NOKIACharacter(lcd,*characters++);
  }
}
void NOKIAMove(const int lcd ,uint8_t x, uint8_t y){
  NOKIAWrite(lcd, 0, 0x80 | x);  // Column. x - range: 0 to 84
  NOKIAWrite(lcd, 0, 0x40 | y);  // Row.  y - range: 0 to 5
}
void NOKIADrawHL(const int lcd,const int begin,const int end){
  for(int cont=0; cont<end; cont++){ //RANGE 0 - 84
    NOKIAMove(lcd,cont,begin);
	NOKIAWrite(lcd,1,0x01);
  }
}
void NOKIADrawVL(const int lcd,const int begin,const int end){
  for(int cont=0; cont<end; cont++){ //RANGE 0 - 6
    NOKIAMove(lcd,begin,cont);
	NOKIAWrite(lcd,1,0xff);
  }
}
void NOKIADrawbox(const int lcd,const int x,const int y,const int beginX,const int endX,const int beginY,const int endY){
  for(int cont=0; cont<x; cont++){ // top
    NOKIAMove(lcd,cont,beginX);
	NOKIAWrite(lcd,1,0x01);
  }
  for(int cont=0; cont<x; cont++){ //Bottom
    NOKIAMove(lcd,cont,endX);
	NOKIAWrite(lcd,1,0x80);
  }
  for(int cont=0; cont<y; cont++){ // Right
    NOKIAMove(lcd,endY,cont);
	NOKIAWrite(lcd,1,0xff);
  }
  for(int cont=0; cont<y; cont++){ // Left
    NOKIAMove(lcd,beginY,cont);
	NOKIAWrite(lcd,1,0xff);
  }
}
