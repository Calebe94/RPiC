#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0

static const char *devName = "/dev/i2c-1";

int i2cInit(int address){
	int file;
	if((file=open(devName,O_RDWR)) < 0){
		printf("I2C: Failed to access %d\n",devName);
		return file < 0;
	}	
	/*address is the address of the device that you are using to comunicate with your RaspberryPi*/	
	if(ioctl(file,I2C_SLAVE,address) < 0){
		printf("I2C: Failed to acquire bus access/talk to slace 0x%x\n",address);
		return file < 0;
	}
	return file;
}
int i2cSend(int file,int data){
	char cmd[1];
	cmd[0] = data;
	if (write(file, cmd, 1) == 1)  usleep(10000);
	/*As we are not talking to direct hardware but a microcontroller we
	  need to wait a short while so that it can respond.
	  1ms seems to be enough but it depends on what workload it has
	*/
	else return -1;
	
	return 0;
}
int i2cReceive(int file){
	char buf[1];
	if (read(file, buf, 1) == 1)	usleep(10000);
	return (int)buf[0];
}
