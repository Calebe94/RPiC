#include<stdio.h>
#include"Fonts.h"
#include<stdint.h>
int main(void){
	unsigned int size = sizeof(pi_logo)/sizeof(uint8_t);
	for (unsigned int count = 0 ; count < size ; count ++){
		printf("%c",pi_logo[count]);
	}
}

