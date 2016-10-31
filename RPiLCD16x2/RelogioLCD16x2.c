#include <stdio.h>
#include <time.h>
#include <string.h>
#include "RaspberryLCD.h"
#define LCD_RS 5
#define LCD_E 6
#define LCD_D4 13
#define LCD_D5 19
#define LCD_D6 26
#define LCD_D7 21
char *format(int number){		
	char 	*retorno,
		ret[100];
	int 	i;

	if (number < 10){
		sprintf(ret,"0%d",number);
		retorno = ret;
		return retorno;
	}
	else{
		sprintf(ret,"%d",number);
		retorno = ret;
		return retorno;
	}
}		
char *data(void){

	int dia,mes,ano;
	char	var1[100],
		var2[100],
		var3[100],
		var4[100],
		*dataPtr;
	struct tm *local;
	time_t t;

	t = time(NULL);
	local = localtime(&t);

	dia = local -> tm_mday;
	mes = local -> tm_mon + 1;
	ano = local -> tm_year + 1900;

	sprintf(var1,"%s",format(dia));
	sprintf(var2,"%s",format(mes));
	sprintf(var3,"%s",format(ano));

	sprintf(var4,"%s/%s/%s",var1,var2,var3);

	dataPtr = var4;
	return dataPtr;
}

char *hora(void){
		
	int	hora,minuto,segundo;
	char	var1[100],
		var2[100],
		var3[100],
		var5[100],
		*retorno;
	struct tm *local;
	time_t t;

	t = time(NULL);
	local = localtime(&t);
		
	// obtem hora, minuto e segundo e os aloca em uma variavel do tipo inteiro
	hora	=	local -> tm_hour;
	minuto	= 	local -> tm_min;
	segundo =	local -> tm_sec;
	
	// por algum motivo precisa converter os valores retornados pelos ponteiros
	// da funcao em variaveis do tipo char
	sprintf(var1,"%s",format(hora));
	sprintf(var2,"%s",format(minuto));
	sprintf(var3,"%s",format(segundo));
	
	// cria a variavel de retorno dos dados e cria um ponteiro para essa variavel
	sprintf(var5,"%s:%s:%s",var1,var2,var3);
	
	// retorna hora no formato hh:mm:ss com um ponteiro
	retorno = var5;
	return retorno;	
}

int main()
{

	int lcd;
	if(lcd = LCDInit(2,16,4,LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7,0,0,0,0)){
		printf("Não Deu Boa !\n");
		return -1;
	}

	char	data_sistema[100],
		hora_sistema[100];
/* Solução Técnica Alternativa para evitar de aparecer lixos na tela do LCD*/
	sprintf(data_sistema,"%s",data());
	sprintf(hora_sistema,"%s",hora());
	LCDPosition(lcd,3,0);
	LCDPrintf(lcd,data_sistema);
	LCDPosition(lcd,4,1);
	LCDPrintf(lcd,hora_sistema);
	delay_ms(500);
	LCDClear(lcd);
	
	while(1){
		sprintf(data_sistema,"%s",data());
		sprintf(hora_sistema,"%s",hora());
		LCDPosition(lcd,3,0);
		LCDPrintf(lcd,data_sistema);
		LCDPosition(lcd,4,1);
		LCDPrintf(lcd,hora_sistema);
	}

}
