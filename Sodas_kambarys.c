
#include "libs/Setup.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#include "libs/sensors/dht.h"
#include "libs/display/oled/ssd1306xled.h"

uint16_t sec_05=0;  
uint8_t operation=0; // for better cpu utilization and task division in time

ISR(TIMER1_OVF_vect)	// 1 interupt ~0,5M cycles, cpu 2MHz
{
	sec_05++;
	if (sec_05>=10)
	{
		sec_05=0;
	}
	uint8_t tmp=sec_05 % 2;
	if (tmp==0)
	{
		tmp=sec_05/2;
		Set_Bit(operation,op_lum_b);
		if (tmp % 2== 0)
		{
			Set_Bit(operation,op_temp_b);
		}
		if (tmp % 5== 0)
		{
			Set_Bit(operation,op_contr_b);
		}
		if (tmp==0)
		{
			Set_Bit(operation,op_send_b);
		}
	}
}


void prepare_screen()
{
	ssd1306_init();
	ssd1306_fill(0x00);	// Clear screen
	ssd1306_char_f8x16((x_max-kambarys_lenght)/2, 0, kambarys);
	ssd1306_char_f8x16(0,2,"temp.");
	ssd1306_char_f8x16(0,4,"humid");
	ssd1306_char_f8x16(0,6,"light");
	
	ssd1306_char_f8x16(offset_units,2,"'C");
	ssd1306_char_f8x16(offset_units,4,"%");
	ssd1306_char_f8x16(offset_units,6,"lux");
}

void dempf_i16 (int16_t input, int16_t *value, uint8_t demf)
{
	int16_t input_prev=*value;
	*value= (input-input_prev)/demf +input_prev;
}


void func_send_fw()
{
	Clear_Bit(operation,op_send_b);
}

void func_lum(int16_t *luminence)
{
	//scan luminance + show
	if (Check_Bit(ADCSRA,ADIF))			//conversion complete
	{	
		int16_t luminence_in=ADCL|(ADCH<<8);
		dempf_i16(luminence_in,luminence,5);
		ssd1306_numdec_font8x16(offset_numbers,6,*luminence,1);
		Clear_Bit(operation,op_lum_b);
		ADCSRA|=(1<<ADSC);				// Start ADC
	}
}

void func_temp(int16_t *temperature, int16_t *humidity)
{
	int16_t temperature_in=0,humidity_in=0;
	int8_t rezultatas =dht_getdata(&temperature_in, &humidity_in);
	if( rezultatas==0)
	{
		// show value
		ssd1306_numdec_div10_font8x16(offset_numbers,2,*temperature);
		ssd1306_numdec_div10_font8x16(offset_numbers,4,*humidity);
	}
	else
	{
		//error
		ssd1306_char_f8x16(offset_numbers,2,"error");
		ssd1306_char_f8x16(offset_numbers,4,"error");
		ssd1306_numdec_div10_font8x16(offset_numbers,6,rezultatas*10);
	}
	Clear_Bit(operation,op_temp_b);
}

void func_contr(int16_t luminence)
{
	uint8_t contrast=0;
	if (luminence>265)
		contrast=0xff;
	else if (luminence<10)
		contrast=0x00;
	else
	contrast=luminence*0xff/(265-10);
	ssd1306_contrast(contrast);
	Clear_Bit(operation,op_contr_b);
}


int main(void)
{
	// pinc 3 - data
	DDRD=0x0a;
	
	// Set Timers parameters
	TCCR1B|=(1<<CS11);				// prescaler 8, counts till 65536
	TIMSK|=(1<<TOIE1);				// overflow inteupt enable
	TCNT1=0;
	
	// Set ADC parameters
	ADMUX|=(1<<MUX1)|(1<<REFS0);	// AVcc reference, ADC2
	ADCSRA|=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	// Enable ADC, prescaller 128
	ADCSRA|=(1<<ADSC);				// Start ADC
	
	sei();							// enable global interupts
	_delay_ms(1000);
	
	prepare_screen();	
	
	int16_t temperature = 0,humidity = 0, luminence=0;
	while (1)
	{
		//every 1s scan luminance + show
		//every 2s scan temp, humid + show
		//every 5s check luminance difference + contrast
		//every 10s give data forward
		ssd1306_numdec_div10_font8x16(offset_numbers,0,operation); // for debug purpose
		if(Check_Bit(operation,op_send_b)) // send data forward		
			func_send_fw();		
		else if(Check_Bit(operation,op_contr_b))//change contrast			 
			func_contr(luminence);		
		else if(Check_Bit(operation,op_lum_b))//scan luminance + show		
			func_lum(&luminence);
		else if(Check_Bit(operation,op_temp_b))	//scan temp, humid + show	
//			func_temp(&temperature,&humidity);
			{}
	}
	
	return 0;
}
