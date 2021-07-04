// -----------------------------------------------------------------------
//   Copyright (C) Yuri Moreeuw Fernandes
// -----------------------------------------------------------------------
//   File: lcd.c
//            Manipulation lib of the LCD SPLC780D
//   Author:  Yuri Claus Moreeuw Fernandes
//            yuri_morreuw@hotmail.com
//   License: GNU GPL 2
//   Base:    https://sites.google.com/site/rmaalmeida/home
// -----------------------------------------------------------------------
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; version 2 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// -----------------------------------------------------------------------
#include <util/delay.h>

unsigned char rs_pin;
unsigned char rw_pin;
unsigned char e_pin;
unsigned char db4_pin;
unsigned char db5_pin;
unsigned char db6_pin;
unsigned char db7_pin;


//Must include 'rw' value: 
//	0 write:
//		TODO: change the function to work even if the rw parameter is not
//		presented by user. So it will be read by default.
//		The parameter cmd: 
//			0b00000001 clear display
//			0b0000001- return home
//			0b000001xy mode set, 
//				x = 0/1 decr/incr
//				y = 0/1 doestshift/shift 
//			0b00001xyz display on,
//				x = 0/1 ?/entiredisplayon,
//				y = 0/1 cursoroff/cursoron
//				z = 0/1 blinkoff/blinkon
//			0b0001xy-- cursor or display shift,
//				x = 0/1 display/cursor
//				y = 0/1 right/left
//			0b001xyz-- function set,
//				x = 0/1 4bits/8bits
//				y = 0/1 1line/2lines
//				z = 0/1 font5x8/font5x11
//			0b01cccccc CGRAM address (useful to draw a character,
//				cccccc = 5 bits of the character line, to send a full character
//						 you need to send the 8 columns
//			0b1xdddddd DDRAM address,
//				x = 0/1 1stline/2ndline 
//				ddddd = 0~F 1column~16column
//
//	1 read:
//		The parameter cmd:
//			0bxccccccc busy flag and address,
//				x = 0/1 notbusy/busy
//				ccccccc = address
//
unsigned char LcdCommand(unsigned char rw, unsigned char cmd){
	unsigned char address_counter = 0;
	
	//set rs to command (0)
	PORTD &= ~(1<<&rs_pin);	
	//read	
	if(rw){
		PORTD |= (1<<rw_pin);//read mode 1
		PORTD |= (1<<e_pin);//set enable to 1
		_delay_us(500);
		PORTD &= ~(1<<e_pin);//set enable to 0
		_delay_us(500);
		address_counter |= (db7_pin<<7);//set bit 7 to value at db7_pin
		address_counter |= (db6_pin<<6);//set bit 6 to value at db6_pin
		address_counter |= (db5_pin<<5);//set bit 5 to value at db5_pin
		address_counter |= (db4_pin<<4);//set bit 4 to value at db4_pin
		PORTD |= (1<<e_pin);//set enable to 1
		_delay_us(500);
		PORTD &= ~(1<<e_pin);//set enable to 0
		_delay_us(500);
		address_counter |= (db7_pin<<3);//set bit 3 to value at db7_pin
		address_counter |= (db6_pin<<2);//set bit 2 to value at db6_pin
		address_counter |= (db5_pin<<1);//set bit 1 to value at db5_pin
		address_counter |= (db4_pin<<0);//set bit 0 to value at db4_pin
		return address_counter;
	}
	//write
	else{
		//high 4 bits to be sent
		db7_pin = cmd>>7;
		db6_pin = cmd>>6;
		db5_pin = cmd>>5;
		db4_pin = cmd>>4;
		PORTD &= ~(1<<rw_pin);//write mode 0
		PORTD |= (1<<e_pin);//set enable to 1
		_delay_us(500);
		PORTD &= ~(1<<e_pin);//set enable to 0
		_delay_us(500);
		//low 4 bits to be sent
		db7_pin = cmd>>3;
		db6_pin = cmd>>2;
		db5_pin = cmd>>1;
		db4_pin = cmd>>0;
		PORTD |= (1<<e_pin);//set enable to 1
		_delay_us(500);
		PORTD &= ~(1<<e_pin);//set enable to 0
		_delay_us(500);
		return 1;
	}
}

//implemented for PORTD pins only
void LcdInit(unsigned char rs_pin,
			unsigned char rw_pin,
			unsigned char e_pin,
			unsigned char db4_pin,
			unsigned char db5_pin,
			unsigned char db6_pin,
			unsigned char db7_pin,){
	unsigned char lcd_ready = 0;
				
	//guarantee that the OUTPUT is zero before enable it
	/*
	PORTD &= ~(1<<rs_pin);//set the pin rs_ping of port D to zero
	PORTD &= ~(1<<rw_pin);//set the pin rw_pin of port D to zero
	PORTD &= ~(1<<e_pin);//set the pin e_pin of port D to zero
	*/
				
	DDRD |= (1<<rs_pin);//set the pin rs_ping of port D to OUTPUT (1 is OUT, 0 is IN)
	DDRD |= (1<<rw_pin);//set the pin rw_pin of port D to OUTPUT (1 is OUT, 0 is IN)
	DDRD |= (1<<e_pin);//set the pin e_pin of port D to OUTPUT (1 is OUT, 0 is IN)
	DDRD |= (1<<db4_pin);//set the pin db4_pin of port D to OUTPUT (1 is OUT, 0 is IN)
	DDRD |= (1<<db5_pin);//set the pin db5_pin of port D to OUTPUT (1 is OUT, 0 is IN)
	DDRD |= (1<<db6_pin);//set the pin db6_pin of port D to OUTPUT (1 is OUT, 0 is IN)
	DDRD |= (1<<db7_pin);//set the pin db7_pin of port D to OUTPUT (1 is OUT, 0 is IN)
	
	
	//guarantees that the lcd is ready
	while(!lcd_ready){
		_delay_ms(1);
		lcd_ready = (LcdCommand(1,0x80)>>7)
	}
}

