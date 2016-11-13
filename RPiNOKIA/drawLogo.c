#include"RPiNOKIA.h"
#include"Fonts.h"
#define RST 7
#define CE 8
#define DC 25
#define DIN 24
#define CLK 23

int main(void){
	int Display = NOKIAInit(CE,RST,DC,DIN,CLK);	

	NOKIAClear(Display);
	NOKIABitmap(Display,pi_logo);
	getchar();
	NOKIAClear(Display);
	NOKIAMove(Display,0,0);
	NOKIAString(Display,"TESTE!");
	getchar();
}
