/*
DHT Library 0x03

copyright (c) Davide Gironi, 2012 // modified by Aldis Adomavicius 2015

Released under GPLv3.
Please refer to LICENSE file for licensing information.

*/

#ifndef DHT_H_
#define DHT_H_


#include <stdio.h>
#include <avr/io.h>
#include "../Setup.h"
#include <util/delay.h>

//timeout retries
#define DHT_TIMEOUT 200

//functions
int8_t dht_getdata(int16_t *temperature, int16_t *humidity);

#endif
