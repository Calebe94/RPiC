#include<stdio.h>
#include"RaspberryGPIO.h"
#include<pthread.h>
#define tempo 10
#define frequencia 1000
//#define periodo 1/frequencia

struct PWMStruct{
	unsigned int pin;
	unsigned int potencia;
};
void *pwmWrite(void *nomequalquer){
	struct PWMStruct *sPWMarg=nomequalquer;
	while(sPWMarg->pin){
		GPIOWrite(sPWMarg->pin, HIGH);	
		usleep(100*sPWMarg->potencia);
		GPIOWrite(sPWMarg->pin, LOW);
		usleep(10000-(100*sPWMarg->potencia));
	}
}
int main(){
	struct PWMStruct arg;
	struct PWMStruct arg1;
	GPIOExport(18);
	GPIOExport(21);
	GPIODirection(18, OUTPUT);
	GPIODirection(21, OUTPUT);
	pthread_t pwm,pwm1;
	arg.pin = 18;
	arg1.pin = 21;

	pthread_create(&pwm, NULL, pwmWrite, (void*)&arg);
	pthread_create(&pwm1, NULL, pwmWrite, (void*)&arg1);
	while(1){
			printf("Informe a Porcentagem do LED 1:\n");
			scanf("%d",&arg.potencia);
			printf("Informe a Porcentagem do LED 2:\n");
			scanf("%d",&arg1.potencia);	
			//printf("Ton:%d\n",(10*(arg.potencia)));
			//printf("Toff:%d\n",1000-(10*arg.potencia));
	}
	
	
}
