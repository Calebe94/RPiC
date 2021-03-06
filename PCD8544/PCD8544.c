/*
=================================================================================
 Name        : PCD8544.c
 Version     : 0.1

 Copyright (C) 2010 Limor Fried, Adafruit Industries
 CORTEX-M3 version by Le Dang Dung, 2011 LeeDangDung@gmail.com (tested on LPC1769)
 Raspberry Pi version by Andre Wussow, 2012, desk@binerry.de

 Description :
     A simple PCD8544 LCD (Nokia3310/5110) driver. Target board is Raspberry Pi.
     This driver uses 5 GPIOs on target board with a bit-bang SPI implementation
     (hence, may not be as fast).
	 Makes use of WiringPI-library of Gordon Henderson (https://projects.drogon.net/raspberry-pi/wiringpi/)

	 Recommended connection (http://www.raspberrypi.org/archives/384):
	 LCD pins      Raspberry Pi
	 LCD1 - GND    P06  - GND
	 LCD2 - VCC    P01 - 3.3V
	 LCD3 - CLK    P11 - GPIO0
	 LCD4 - Din    P12 - GPIO1
	 LCD5 - D/C    P13 - GPIO2
	 LCD6 - CS     P15 - GPIO3
	 LCD7 - RST    P16 - GPIO4
	 LCD8 - LED    P01 - 3.3V 

 References  :
 http://www.arduino.cc/playground/Code/PCD8544
 http://ladyada.net/products/nokia5110/
 http://code.google.com/p/meshphone/

================================================================================
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
================================================================================
 */
#include "RaspberryGPIO.h"
#include "PCD8544.h"
#include "Fonts.h"
// An abs() :)
#define abs(a) (((a) < 0) ? -(a) : (a))

// bit set
#define _BV(bit) (0x1 << (bit))


static uint8_t cursor_x, cursor_y, textsize, textcolor;
static int8_t _din, _sclk, _dc, _rst, _cs;

// the memory buffer for the LCD
uint8_t pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8] = {0,};

#define enablePartialUpdate

static void my_setpixel(uint8_t x, uint8_t y, uint8_t color){
	if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
		return;
	// x is which column
	if (color)
		pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= _BV(y%8);
	else
		pcd8544_buffer[x+ (y/8)*LCDWIDTH] &= ~_BV(y%8);
}

void LCDshowLogo(){
	uint32_t i;
	for (i = 0; i < LCDWIDTH * LCDHEIGHT / 8; i++  )
	{
		pcd8544_buffer[i] = pi_logo[i];
	}
	LCDdisplay();
}

#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
	if (xmin < xUpdateMin) xUpdateMin = xmin;
	if (xmax > xUpdateMax) xUpdateMax = xmax;
	if (ymin < yUpdateMin) yUpdateMin = ymin;
	if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}

int LCDInit(struct LCD *init,uint8_t _din,uint8_t _sclk,uint8_t _dc,uint8_t _rst,uint8_t _cs, uint8_t contrast){
		
//	static int LCDINIT = -1;	uint8_t cursor_x,cursor_y,textsize,textcolor,DIN,SCLK,DC,RST,CS,initialised=0;
	if(init->initialised == 0){
		init->initialised = 1;
		init->DIN = _din;
		init->SCLK = _sclk;
		init->DC = _dc;
		init->RST = _rst;
		init->CS = _cs;
		cursor_x = 0;
		cursor_y = 0;
		textsize = 1;
		textcolor = BLACK;
	}
	GPIOExport(_din);	GPIODirection(_din,OUTPUT);
	GPIOExport(_sclk);	GPIODirection(_sclk,OUTPUT);
	GPIOExport(_dc);	GPIODirection(_dc,OUTPUT);
	GPIOExport(_rst);	GPIODirection(_rst,OUTPUT);
	GPIOExport(_cs);	GPIODirection(_cs,OUTPUT);

	GPIOWrite(_cs,LOW);	
	GPIOWrite(_rst,LOW);
	delay_ms(500);
	GPIOWrite(_rst,HIGH);

	// get into the EXTENDED mode!
	LCDcommand(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

	// LCD bias select (4 is optimal?)
	LCDcommand(PCD8544_SETBIAS | 0x4);

	// set VOP
	if (contrast > 0x7f)
		contrast = 0x7f;

	LCDcommand( PCD8544_SETVOP | contrast); // Experimentally determined

	// normal mode
	LCDcommand(PCD8544_FUNCTIONSET);

	// Set display to Normal
	LCDcommand(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

	// set up a bounding box for screen updates
	updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);

}

void LCDdrawbitmap(uint8_t x, uint8_t y,const uint8_t *bitmap, uint8_t w, uint8_t h,uint8_t color)
{
	uint8_t j,i;
	for ( j=0; j<h; j++){
		for ( i=0; i<w; i++ ){
			if (*(bitmap + i + (j/8)*w) & _BV(j%8)){
				my_setpixel(x+i, y+j, color);
			}
		}
	}
	updateBoundingBox(x, y, x+w, y+h);
}

void LCDdrawstring(struct LCD *nokia,uint8_t x, uint8_t y, char *c){
	nokia->cursor_x = x;
	nokia->cursor_y = y;
	while (*c)
	{
		LCDwrite(nokia,*c++);
	}
}

void LCDdrawstring_P(struct LCD *nokia,uint8_t x, uint8_t y, const char *str)
{
	nokia->cursor_x = x;
	nokia->cursor_y = y;
	while (1){
		char c = (*str++);
		if (! c)
			return;
		LCDwrite(nokia,c);
	}
}

void LCDdrawchar(uint8_t x, uint8_t y, char c)
{
	if (y >= LCDHEIGHT) return;
	if ((x+5) >= LCDWIDTH) return;
	uint8_t i,j;
	for ( i =0; i<5; i++ )
	{
		uint8_t d = *(font+(c*5)+i);
		uint8_t j;
		for (j = 0; j<8; j++)
		{
			if (d & _BV(j))
			{
				my_setpixel(x+i, y+j, textcolor);
			}
			else
			{
				my_setpixel(x+i, y+j, !textcolor);
			}
		}
	}

	for ( j = 0; j<8; j++)
	{
		my_setpixel(x+5, y+j, !textcolor);
	}
	updateBoundingBox(x, y, x+5, y + 8);
}

void LCDwrite(struct LCD *nokia,uint8_t c)
{
	if (c == '\n')
	{
		nokia->cursor_y += textsize*8;
		nokia->cursor_x = 0;
	}
	else if (c == '\r')
	{
		// skip em
	}
	else
	{
		LCDdrawchar(nokia->cursor_x, nokia->cursor_y, c);
		nokia->cursor_x += textsize*6;
		if (nokia->cursor_x >= (LCDWIDTH-5))
		{
			nokia->cursor_x = 0;
			nokia->cursor_y+=8;
		}
		if (nokia->cursor_y >= LCDHEIGHT)
			nokia->cursor_y = 0;
	}
}

void LCDsetCursor(struct LCD *nokia,uint8_t x, uint8_t y)
{
	nokia->cursor_x = x;
	nokia->cursor_y = y;
}

// bresenham's algorithm - thx wikpedia
void LCDdrawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	// much faster to put the test here, since we've already sorted the points
	updateBoundingBox(x0, y0, x1, y1);

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1)
	{
		ystep = 1;
	} else
	{
		ystep = -1;
	}

	for (; x0<=x1; x0++)
	{
		if (steep)
		{
			my_setpixel(y0, x0, color);
		}
		else
		{
			my_setpixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

// filled rectangle
void LCDfillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,  uint8_t color)
{
	// stupidest version - just pixels - but fast with internal buffer!
	uint8_t i,j;
	for ( i=x; i<x+w; i++)
	{
		for ( j=y; j<y+h; j++)
		{
			my_setpixel(i, j, color);
		}
	}
	updateBoundingBox(x, y, x+w, y+h);
}

// draw a rectangle
void LCDdrawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	// stupidest version - just pixels - but fast with internal buffer!
	uint8_t i;
	for ( i=x; i<x+w; i++) {
		my_setpixel(i, y, color);
		my_setpixel(i, y+h-1, color);
	}
	for ( i=y; i<y+h; i++) {
		my_setpixel(x, i, color);
		my_setpixel(x+w-1, i, color);
	}

	updateBoundingBox(x, y, x+w, y+h);
}

// draw a circle outline
void LCDdrawcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
	updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

	my_setpixel(x0, y0+r, color);
	my_setpixel(x0, y0-r, color);
	my_setpixel(x0+r, y0, color);
	my_setpixel(x0-r, y0, color);

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		my_setpixel(x0 + x, y0 + y, color);
		my_setpixel(x0 - x, y0 + y, color);
		my_setpixel(x0 + x, y0 - y, color);
		my_setpixel(x0 - x, y0 - y, color);

		my_setpixel(x0 + y, y0 + x, color);
		my_setpixel(x0 - y, y0 + x, color);
		my_setpixel(x0 + y, y0 - x, color);
		my_setpixel(x0 - y, y0 - x, color);

	}
}

void LCDfillcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
	updateBoundingBox(x0-r, y0-r, x0+r, y0+r);
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;
	uint8_t i;

	for (i=y0-r; i<=y0+r; i++)
	{
		my_setpixel(x0, i, color);
	}

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for ( i=y0-y; i<=y0+y; i++)
		{
			my_setpixel(x0+x, i, color);
			my_setpixel(x0-x, i, color);
		}
		for ( i=y0-x; i<=y0+x; i++)
		{
			my_setpixel(x0+y, i, color);
			my_setpixel(x0-y, i, color);
		}
	}
}

// the most basic function, set a single pixel
void LCDsetPixel(uint8_t x, uint8_t y, uint8_t color)
{
	if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
		return;

	// x is which column
	if (color)
		pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= _BV(y%8);
	else
		pcd8544_buffer[x+ (y/8)*LCDWIDTH] &= ~_BV(y%8);
	updateBoundingBox(x,y,x,y);
}

// the most basic function, get a single pixel
uint8_t LCDgetPixel(uint8_t x, uint8_t y)
{
	if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
		return 0;

	return (pcd8544_buffer[x+ (y/8)*LCDWIDTH] >> (7-(y%8))) & 0x1;
}

void LCDspiwrite(uint8_t c)
{
	shiftOut(_din, _sclk, MSBFIRST, c);
}

void LCDcommand(uint8_t c)
{
	GPIOWrite(_dc,LOW);
	//digitalWrite( _dc, LOW);
	LCDspiwrite(c);
}

void LCDdata(uint8_t c)
{
	GPIOWrite(_dc,HIGH);
	//digitalWrite(_dc, HIGH);
	LCDspiwrite(c);
}

void LCDsetContrast(uint8_t val)
{
	if (val > 0x7f) {
		val = 0x7f;
	}
	LCDcommand(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
	LCDcommand( PCD8544_SETVOP | val);
	LCDcommand(PCD8544_FUNCTIONSET);
}

void LCDdisplay(void)
{
	uint8_t col, maxcol, p;

	for(p = 0; p < 6; p++){
#ifdef enablePartialUpdate
		// check if this page is part of update
		if ( yUpdateMin >= ((p+1)*8) )
			continue;   // nope, skip it!
	
		if (yUpdateMax < p*8)
			break;
#endif

		LCDcommand(PCD8544_SETYADDR | p);


#ifdef enablePartialUpdate
		col = xUpdateMin;
		maxcol = xUpdateMax;
#else
		// start at the beginning of the row
		col = 0;
		maxcol = LCDWIDTH-1;
#endif

		LCDcommand(PCD8544_SETXADDR | col);

		for(; col <= maxcol; col++) {
			//uart_putw_dec(col);
			//uart_putchar(' ');
			LCDdata(pcd8544_buffer[(LCDWIDTH*p)+col]);
		}
	}

	LCDcommand(PCD8544_SETYADDR );  // no idea why this is necessary but it is to finish the last byte?
#ifdef enablePartialUpdate
	xUpdateMin = LCDWIDTH - 1;
	xUpdateMax = 0;
	yUpdateMin = LCDHEIGHT-1;
	yUpdateMax = 0;
#endif

}

// clear everything
void LCDclear(struct LCD *nokia) {
	//memset(pcd8544_buffer, 0, LCDWIDTH*LCDHEIGHT/8);
	uint32_t i;
	for ( i = 0; i < LCDWIDTH*LCDHEIGHT/8 ; i++)
		pcd8544_buffer[i] = 0;
	updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
	nokia->cursor_y = nokia->cursor_x = 0;
}

// bitbang serial shift out on select GPIO pin. Data rate is defined by CPU clk speed and CLKCONST_2. 
// Calibrate these value for your need on target platform.
/*void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	uint8_t i;
	uint32_t j;

	for (i = 0; i < 8; i++){
		if (bitOrder == LSBFIRST)
			GPIOWrite(dataPin, !!(val & (1 << i)));
		else
			GPIOWrite(dataPin, !!(val & (1 << (7 - i))));

		GPIOWrite(clockPin, HIGH);
		for (j = CLKCONST_2; j > 0; j--); // clock speed, anyone? (LCD Max CLK input: 4MHz)
		GPIOWrite(clockPin, LOW);
	}
}*/
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val){
	if(bitOrder == LSBFIRST){
		for(register uint8_t count = 0 ; count < 8 ; count ++){
			GPIOWrite(dataPin,!!(val&(1<<count)));
			GPIOWrite(clockPin,HIGH);
			for(register unsigned int count1 = CLKCONST_2 ; count1 > 0 ; count1--);
			GPIOWrite(clockPin,LOW);
		}
	}
	else{
		for(register uint8_t count = 0 ; count < 8 ; count ++){
			GPIOWrite(dataPin,!!(val & (1 << (7 - count))));
			GPIOWrite(clockPin,HIGH);
			for(register unsigned int count1 = CLKCONST_2 ; count1 > 0 ; count1--);
			GPIOWrite(clockPin,LOW);
		}
	}			
}

// roughly calibrated spin delay
void _delay_ms(uint32_t t){
	uint32_t nCount = 0;
	while (t != 0)
	{
		nCount = CLKCONST_1;
		while(nCount != 0)
			nCount--;
		t--;
	}
}
void LCDDrawBitmap(const unsigned char *logo){
	for (register unsigned int cont = 0; cont < LCDWIDTH * LCDHEIGHT / 8; cont++  ){
		pcd8544_buffer[cont] = logo[cont];
	}
	LCDdisplay();
}
void LCDSet(struct LCD *nokia,uint8_t color,uint8_t size){
	nokia->textcolor = color;
	nokia->textsize = size;
}
