/*
DHT Library 0x03

copyright (c) Davide Gironi, 2012  // modified by Aldis Adomavicius 2015

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <stdio.h>
#include <string.h>
#include <avr/io.h>

#include "dht.h"

/*
 * get data from sensor
 */
int8_t dht_getdata(int16_t *temperature, int16_t *humidity) {

	uint8_t bits[5];
	uint8_t i,j = 0;

	memset(bits, 0, sizeof(bits));

	//reset port
	DHT_DDR |= (1<<DHT_INPUTPIN); //output
	DHT_PORT |= (1<<DHT_INPUTPIN); //high
	_delay_ms(100);

	//send request
	DHT_PORT &= ~(1<<DHT_INPUTPIN); //low
	_delay_us(500);
	DHT_PORT |= (1<<DHT_INPUTPIN); //high
	DHT_DDR &= ~(1<<DHT_INPUTPIN); //input
	_delay_us(40);

	//check start condition 1
	if((DHT_PIN & (1<<DHT_INPUTPIN))) {
		return -1;
	}
	_delay_us(80);
	//check start condition 2
	if(!(DHT_PIN & (1<<DHT_INPUTPIN))) {
		return -2;
	}
	_delay_us(80);

	//read the data
	uint16_t timeoutcounter = 0;
	for (j=0; j<5; j++) { //read 5 byte
		uint8_t result=0;
		for(i=0; i<8; i++) {//read every bit
			timeoutcounter = 0;
			while(!(DHT_PIN & (1<<DHT_INPUTPIN))) { //wait for an high input (non blocking)
				timeoutcounter++;
				if(timeoutcounter > DHT_TIMEOUT) {
					return -3; //timeout
				}
			}
			_delay_us(30);
			if(DHT_PIN & (1<<DHT_INPUTPIN)) //if input is high after 30 us, get result
				result |= (1<<(7-i));
			timeoutcounter = 0;
			while(DHT_PIN & (1<<DHT_INPUTPIN)) { //wait until input get low (non blocking)
				timeoutcounter++;
				if(timeoutcounter > DHT_TIMEOUT) {
					return -4; //timeout
				}
			}
		}
		bits[j] = result;
	}

	//reset port
	DHT_DDR |= (1<<DHT_INPUTPIN); //output
	DHT_PORT |= (1<<DHT_INPUTPIN); //low
	_delay_ms(100);

	//check checksum
	if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4]) {
		//return temperature and humidity		
		int16_t rawhumidity = bits[0]<<8 | bits[1];
		int16_t rawtemperature = 0;
		if (bits[2] & 0x80)
		{
			bits[2]=(bits[2] & 0x7F);
			rawtemperature =(bits[2]<<8 | bits[3])*-1;
		}
		else
		{
			rawtemperature = bits[2]<<8 | bits[3];
		}		
		*temperature=rawtemperature;
		*humidity=rawhumidity;
		return 0;
	}

	return -5;


}

