/*
 * Setup.h
 *
 * Created: 1/3/2016 14:36:56 
 *  Author: Aldis
 */ 

// --------------------------DHT22--------------------------------------------------
#ifndef SETTUP_H_
#define SETTUP_H_
//setup port
#define DHT_DDR DDRC
#define DHT_PORT PORTC
#define DHT_PIN PINC
#define DHT_INPUTPIN PC3

// --------------------------SSD1306--------------------------------------------------

#define SSD1306_SCL		PC5	// SCL,	Pin 3 on SSD1306 Board
#define SSD1306_SDA		PC4	// SDA,	Pin 4 on SSD1306 Board
#define red_led			PD3
#define red_blue		PD1

#define SSD1306_SA		0x78	// Slave address


#define x_max 128
#define y_max 64

#define char8x16_sizex 8
#define char6x8_sizex 6
// ----------------------------------------------------------------------------

#define MHZ 1000000UL
#define F_CPU 2*MHZ

#define kambarys "Seneliu k."
#define kambarys_lenght 10*char8x16_sizex	// size of kambarys string in pixels


#define offset_numbers 7*char8x16_sizex		// offset for value of measurement
#define offset_units 12*char8x16_sizex		// offset of units of measurement

// ----------------------------------------------------------------------------
#define Set_Bit(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define Clear_Bit(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define Flip_Bit(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))
#define Send_Bit(ADDRESS_in,BIT_in,ADDRESS_out,BIT_out) (ADDRESS_out ^= (-Check_Bit(ADDRESS_in,BIT_in) ^ ADDRESS_out) & (1 << BIT_out))

#define Send_Bit_mask(check_number, mask_check, number, bit_to_set) (number ^= (-((check_number  & mask_check) == mask_check) ^ number) & (1 << bit_to_set))

#define Check_Bit(ADDRESS,BIT) (ADDRESS & (1<<BIT))

#define Set_Bit_Mask(x,y) (x |= (y))
#define Clear_Bit_Mask(x,y) (x &= (~y))
#define Flip_Bit_Mask(x,y) (x ^= (y))
#define Check_Bit_Mask(x,y) (x & (y))

//operation bits
#define op_io_b			0
#define op_lum_b		1
#define op_temp_b		2
#define op_contr_b		3
#define op_send_b		4


#endif /* SETTUP_H_ */