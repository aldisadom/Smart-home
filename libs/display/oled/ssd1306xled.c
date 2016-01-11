/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Copyright (c) 2015 Neven Boyanov, Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Please, as a favour, retain the link http://tinusaur.org to The Tinusaur Project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <avr/io.h>

#include <avr/pgmspace.h>

#include "ssd1306xled.h"
#include "../fonts.h"
//#include "font8x16.h"


// ----------------------------------------------------------------------------

// Convenience definitions for PORTB

#define DIGITAL_WRITE_HIGH(PORT) PORTC |= (1 << PORT)
#define DIGITAL_WRITE_LOW(PORT) PORTC &= ~(1 << PORT)

// ----------------------------------------------------------------------------

// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358

const uint8_t ssd1306_init_sequence [] PROGMEM = {	// Initialization Sequence
	0xAE,			// Display OFF (sleep mode)
	0x20, 0b00,		// Set Memory Addressing Mode
					// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
					// 10=Page Addressing Mode (RESET); 11=Invalid
	0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7
	0xC8,			// Set COM Output Scan Direction
	0x00,			// ---set low column address
	0x10,			// ---set high column address
	0x40,			// --set start line address
	0x81, 0x3F,		// Set contrast control register
	0xA1,			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped. 
	0xA6,			// Set display mode. A6=Normal; A7=Inverse
	0xA8, 0x3F,		// Set multiplex ratio(1 to 64)
	0xA4,			// Output RAM to Display
					// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0xD5,			// --set display clock divide ratio/oscillator frequency
	0xF0,			// --set divide ratio
	0xD9, 0x22,		// Set pre-charge period
	0xDA, 0x12,		// Set com pins hardware configuration		
	0xDB,			// --set vcomh
	0x20,			// 0x20,0.77xVcc
	0x8D, 0x14,		// Set DC-DC enable
	0xAF			// Display ON in normal mode
	
};

// ----------------------------------------------------------------------------

// These function should become separate library for handling I2C simplified output.

void ssd1306_xfer_start(void)
{
	DIGITAL_WRITE_HIGH(SSD1306_SCL);	// Set to HIGH
	DIGITAL_WRITE_HIGH(SSD1306_SDA);	// Set to HIGH
	DIGITAL_WRITE_LOW(SSD1306_SDA);		// Set to LOW
	DIGITAL_WRITE_LOW(SSD1306_SCL);		// Set to LOW
}

void ssd1306_xfer_stop(void)
{
	DIGITAL_WRITE_LOW(SSD1306_SCL);		// Set to LOW
	DIGITAL_WRITE_LOW(SSD1306_SDA);		// Set to LOW
	DIGITAL_WRITE_HIGH(SSD1306_SCL);	// Set to HIGH
	DIGITAL_WRITE_HIGH(SSD1306_SDA);	// Set to HIGH
}

void ssd1306_send_byte(uint8_t byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		if ((byte << i) & 0x80)
			DIGITAL_WRITE_HIGH(SSD1306_SDA);
		else
			DIGITAL_WRITE_LOW(SSD1306_SDA);
		
		DIGITAL_WRITE_HIGH(SSD1306_SCL);
		DIGITAL_WRITE_LOW(SSD1306_SCL);
	}
	DIGITAL_WRITE_HIGH(SSD1306_SDA);
	DIGITAL_WRITE_HIGH(SSD1306_SCL);
	DIGITAL_WRITE_LOW(SSD1306_SCL);
}

void ssd1306_send_command_start(void) {
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);  // Slave address, SA0=0
	ssd1306_send_byte(0x00);	// write command
}

void ssd1306_send_command_stop(void) {
	ssd1306_xfer_stop();
}

void ssd1306_send_command(uint8_t command)
{
	ssd1306_send_command_start();
	ssd1306_send_byte(command);
	ssd1306_send_command_stop();
}

void ssd1306_send_data_start(void)
{
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);
	ssd1306_send_byte(0x40);	//write data
}

void ssd1306_send_data_stop(void)
{
	ssd1306_xfer_stop();
}

// ----------------------------------------------------------------------------

void ssd1306_init(void)
{
	DDRC |= (1 << SSD1306_SDA);	// Set port as output
	DDRC |= (1 << SSD1306_SCL);	// Set port as output
	
	for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
		ssd1306_send_command(pgm_read_byte(&ssd1306_init_sequence[i]));
	}
}

void ssd1306_contrast(uint8_t contrastas)
{
	ssd1306_send_command(0x81);
	ssd1306_send_command(contrastas);
}

void ssd1306_setpos(uint8_t x, uint8_t y)
{
	ssd1306_send_command_start();
	ssd1306_send_byte(0xb0 + y);
	ssd1306_send_byte(((x & 0xf0) >> 4) | 0x10); // | 0x10
	ssd1306_send_byte((x & 0x0f)); // | 0x01
	ssd1306_send_command_stop();
}


void ssd1306_fill(uint8_t p)
{
	ssd1306_setpos(0, 0);
	ssd1306_send_data_start();
	for (uint16_t i = 0; i < 128 * 8; i++)
	{
		ssd1306_send_byte(p);
	}
	ssd1306_send_data_stop();
}

// ----------------------------------------------------------------------------

void ssd1306_char_font6x8(char ch) {
	uint8_t c = ch - 32;
	ssd1306_send_data_start();
	for (uint8_t i = 0; i < 6; i++)
	{
		ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font6x8[c * 6 + i]));
	}
	ssd1306_send_data_stop();
}

void ssd1306_string_font6x8(uint8_t x, uint8_t y,char *s) {
	ssd1306_setpos(x, y);
	while (*s) {
		ssd1306_char_font6x8(*s++);
	}
}


void ssd1306_numdec_font8x16(uint8_t x, uint8_t y, uint16_t num, uint8_t no_zero) {
	uint8_t value=num;
	char ssd1306_numdec_buffer[5 + 1];
	ssd1306_numdec_buffer[5] = '\0';   // Terminate the string.
	ssd1306_numdec_buffer[4]=num%10+48;
	num=num/10;
	ssd1306_numdec_buffer[3]=num%10+48;
	num=num/10;
	ssd1306_numdec_buffer[2]=num%10+48;
	num=num/10;
	ssd1306_numdec_buffer[1]=num%10+48;
	num=num/10;
	ssd1306_numdec_buffer[0]=num%10+48;
	if (no_zero)
	{
		if (ssd1306_numdec_buffer[0]==48)				{ssd1306_numdec_buffer[0]=32;}
		if (ssd1306_numdec_buffer[1]==48 && value<1000)	{ssd1306_numdec_buffer[1]=32;}
		if (ssd1306_numdec_buffer[2]==48 && value<100)	{ssd1306_numdec_buffer[2]=32;}
		if (ssd1306_numdec_buffer[3]==48 && value<10)		{ssd1306_numdec_buffer[3]=32;}
	}
	ssd1306_char_f8x16( x,  y,ssd1306_numdec_buffer);
}

void ssd1306_char_f8x16(uint8_t x, uint8_t y, const char ch[])
{
	uint8_t c, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if (x > 120)
		{
			x = 0;
			y++;
		}
		ssd1306_setpos(x, y);
		ssd1306_send_data_start();
		for (uint8_t i = 0; i < 8; i++)
		{
			ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font8x16[c * 16 + i]));
		}
		ssd1306_send_data_stop();
		ssd1306_setpos(x, y + 1);
		ssd1306_send_data_start();
		for (uint8_t i = 0; i < 8; i++)
		{
			ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font8x16[c * 16 + i + 8]));
		}
		ssd1306_send_data_stop();
		x += 8;
		j++;
	}
}

void ssd1306_numdec_div10_font8x16(uint8_t x, uint8_t y,int16_t num)
{
	char ssd1306_numdec_buffer[5 + 1];
	ssd1306_numdec_buffer[5] = '\0';   // Terminate the string.
	if (num<0)
	{
		ssd1306_numdec_buffer[0]=45;
		num=num*-1;
	}
	else
	{
		ssd1306_numdec_buffer[0]=32;
	}

	ssd1306_numdec_buffer[4]=num%10+48;
	num=num/10;
	ssd1306_numdec_buffer[3]=44;
	ssd1306_numdec_buffer[2]=num%10+48;
	num=num/10;
	ssd1306_numdec_buffer[1]=num%10+48;
	
	if (ssd1306_numdec_buffer[1]==48)	{ssd1306_numdec_buffer[1]=32;}
		
	ssd1306_char_f8x16( x,  y,ssd1306_numdec_buffer);
}
