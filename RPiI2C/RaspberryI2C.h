#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
/*Definitions to be used in programm*/
#define ARDUINO 0x04

int i2cInit(int address);
int i2cSend(int file,int data);
int i2cReceive(int file);
