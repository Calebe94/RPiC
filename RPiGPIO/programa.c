#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#define ENTRADA 1
#define SAIDA 	0
#define HIGH 	1
#define LOW 	0

bool GPIOexport(int pino);
bool GPIOdirection(int pino, int direcao);
bool GPIOWrite(int pino,int valor);
bool GPIORead(int pino);
bool GPIOUnexport(int pino);

int main(void)
{
	GPIOexport(7);
	GPIOdirection(7,SAIDA);
	while(1)
	{
		GPIOWrite(7,HIGH);
		usleep(1000000);
		GPIOWrite(7,LOW);
		usleep(1000000);
	}
	GPIOUnexport(7);
}
bool GPIOexport(int pino)
{
	char buffer[3];
	int arquivo;
	arquivo = open("/sys/class/gpio/export",O_WRONLY);
	if( arquivo == -1)
		return false;
	snprintf( buffer, 3 , "%d", pino);
	if( write(arquivo,buffer,3) == -1 )
	{	
		close(arquivo);
		return false;
	}
	close(arquivo);
	return true;	
}
bool GPIOUnexport(int pino)
{
	char buffer[3];
	int arquivo;
	arquivo = open("/sys/class/gpio/unexport",O_WRONLY);
	if( arquivo == -1)
		return false;
	snprintf( buffer, 3 , "%d", pino);
	if( write(arquivo,buffer,3) == -1 )
	{	
		close(arquivo);
		return false;
	}
	close(arquivo);
	return true;	
}
bool GPIOdirection(int pino,int direcao)
{
	char caminho[35];
	int arquivo;
	
	snprintf(caminho,35,"/sys/class/gpio/gpio%d/direction",pino);
	if(arquivo == -1)
		return false;
if( write(arquivo,(direcao == ENTRADA)?"in":"out",(direcao== ENTRADA)?2:3) == -1 )
{
	close(arquivo);
	return false;
}
close(arquivo);
return true;
}
bool GPIOWrite(int pino,int valor)
{
	char caminho[35];
	int arquivo;
	
	snprintf(caminho,35,"/sys/class/gpio/gpio%d/value",pino);
	if(arquivo == -1)
		return false;
	if( write(arquivo,(valor == HIGH)?"1":"0",1) == -1 )
	{
		close(arquivo);
		return false;
	}
	close(arquivo);
	return true;	
}
bool GPIORead(int pino)
{
	char caminho[35];
	int arquivo;
	char retorno[2];
	snprintf(caminho,35,"/sys/class/gpio/gpio%d/value",pino);
	if(arquivo == -1)
		return false;
	if( read(arquivo,retorno,2)==-1)
	{
		close(arquivo);
		return false;
	}
	
	close(arquivo);
	return (int)retorno[0]-48;	
	//return retorno[0]-'0';
}
