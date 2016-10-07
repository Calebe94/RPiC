#include"RaspberryNOKIA.h"
int main(void){
	int lcd = 0;
// NOKIAInit(PIN_SCE,PIN_RESET,PIN_DC, PIN_SDIN, PIN_SCLK)
	lcd=NOKIAInit(6,5,13,19,26);
 	NOKIAClear(lcd);	
	while(1){
		//drawLine(lcd);
		NOKIADrawbox(lcd,84,6,0,5,0,83);
		NOKIAMove(lcd,30,2);
		NOKIAString (lcd,"LCD");
  		NOKIAMove(lcd,22,3);
		NOKIAString(lcd,"Teste");
		delay_s(5);	
		NOKIAClear(lcd);
		NOKIADrawHL(lcd,0,84);
		delay_s(5);	
		NOKIAClear(lcd);
		NOKIADrawVL(lcd,0,6);		
		delay_s(5);	
		NOKIAClear(lcd);
  }
}

