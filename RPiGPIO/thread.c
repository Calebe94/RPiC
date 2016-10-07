#include"RaspberryGPIO.h"
#include<pthread.h>
unsigned const int BOT1=5;
unsigned const int BOT2=6;
unsigned const int LED1=13;
unsigned const int LED2=19;
void *ligaLed1(){
	while(1){	
		if(GPIORead(BOT1)==HIGH)
			while(GPIORead(BOT1)==HIGH)
				GPIOWrite(LED1,HIGH);
		else	GPIOWrite(LED1,LOW);
	}
		pthread_exit(NULL);
}
void ligaLed2(){
	while(1){	
	if(GPIORead(BOT2)==HIGH)
		while(GPIORead(BOT2)==HIGH)
			GPIOWrite(LED2,HIGH);
	else	GPIOWrite(LED2,LOW);
	}
}
int main(void){
	GPIOExport(BOT1);
	GPIOExport(BOT2);
	if(!GPIOExport(LED1))	printf("Deu Ruim na exportação do LED1(13)! \n");
	if(!GPIOExport(LED2)) printf("Deu Ruim na Exportação do LED2(19)!\n");
	GPIODirection(BOT1,ENTRADA);
	GPIODirection(BOT2,ENTRADA);
	if(!GPIODirection(LED1,OUTPUT)) printf("Deu ruim na direção do LED1(13)!\n");
	if(!GPIODirection(LED2,OUTPUT)) printf("Deu Ruim na direção do LED2(19)!\n");
	GPIOSetup(LED1,OUTPUT);
	GPIOSetup(LED2,OUTPUT);
	pthread_t thread;
	int rc=pthread_create(&thread,NULL,ligaLed1,NULL);
	if(rc)	printf("Deu Bosta!\n");
	while(1){	
		ligaLed2();
	}	
}
