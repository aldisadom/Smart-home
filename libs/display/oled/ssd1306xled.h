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
#include "../../Setup.h"


#ifndef SSD1306XLED_H
#define SSD1306XLED_H

// ----------------------------------------------------------------------------

// These functions are used only internally by the library
void ssd1306_xfer_start(void);
void ssd1306_xfer_stop(void);
void ssd1306_send_byte(uint8_t byte);
void ssd1306_send_command(uint8_t command);
void ssd1306_send_data_start(void);
void ssd1306_send_data_stop(void);

// ----------------------------------------------------------------------------

void ssd1306_init(void);
void ssd1306_contrast(uint8_t contrastas);
void ssd1306_setpos(uint8_t x, uint8_t y);
void ssd1306_fill(uint8_t p);

void ssd1306_char_font6x8(char ch);
void ssd1306_string_font6x8(uint8_t x, uint8_t y, char *s);

void ssd1306_char_f8x16(uint8_t x, uint8_t y, const char ch[]);
void ssd1306_numdec_font8x16(uint8_t x, uint8_t y, uint16_t num, uint8_t no_zero);
void ssd1306_numdec_div10_font8x16(uint8_t x, uint8_t y,int16_t num);

// ----------------------------------------------------------------------------

#endif
