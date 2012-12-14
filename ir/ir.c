/*
 Copyright (c) 2012 G. Bartsch

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
*/


/* 
 * Infrared decoder for 
 *
 * - RUWIDO Merlin IR Keyboard protocol
 *   56kHz receiver required! (e.g. TSOP 31256)
 *
 * - NEC protocol (e.g. RCX161 in SRB mode)
 *
 *
 * RCX161
 *   TV  => RC5
 *   STB => NEC adr 15172 (only mode in which all buttons work!)
 *   DVD => NEC adr 32512
 *   VCR => NEC16 adr 21
 *
 * MERLIN IR Keyboard
 *   56kHz, RUWIDO protocol
 *
 */


/*
 * NEC protocol 
 * http://wiki.altium.com/display/ADOH/NEC+Infrared+Transmission+Protocol
 *
 * basic time unit is 562.5us since our timer is running at 15.625kHz == 64us,
 * this corresponds to 562.5/64=8.79 timer intervals
 *
 * locial '0': 562.5 us = 8.79 ti pulse followed by  562.5 us = 8.79 ti space
 * locial '1': 562.5 us = 8.79 ti pulse followed by 1687.5 us = 26.3 ti space
 *
 * protocol is as follows:
 *
 * 9   ms = 140 ti : leading pulse burst 
 * 4.5 ms =  70 ti : space 
 * the 8-bit address for the receiving device
 * the 8-bit logical inverse of the address
 * the 8-bit command
 * the 8-bit logical inverse of the command
 * a final 562.5µs pulse burst to signify the end of message transmission.
 *
 *
 * RUWIDO (SIEMENS) protocol
 *
 *   SIEMENS frame:  1 start bit + 22 data bits + no stop bit
 *   SIEMENS data:   13 address bits + 1 repeat bit + 7 data bits + 1 unknown bit
 *
 *   start  bit           data "0":            data "1":
 *   -------_______       _______-------       -------_______
 *    250us  250us         250us  250us         250us  250us
 *
 *
 * Conrad protocol (not supported yet)
 *
 * IR_DIFF: cnt=064 DATA 29483 00023 00108 00023 00056 00024 00029 00024 00056 00023
 *  00030 00023 00056 00024 00055 00024 00055 00024 00030 00023 00056 00023 00056
 *  00023 00030 00023 00030 00024 00144 00024 00108 00023 00056 00023 00030 00023 
 *  00056 00023 00030 00024 00056 00023 00056 00023 00056 00023 00030 00023 00056 
 *  00023 00056 00023 00031 00023 00030 00023 00520 
 * 
 */

#include <avr/io.h>

#include "uart/uart.h"
#include "ir/ir.h"

// input pin IR-receiver is connected to
// has to match interrupt handler setup in main!
#define ir_input()  (!((PIND) & 4))
//#define ir_input()  (!((PINB) & 2))

// debug / logging defines (i.e. re-engineer unknown/new IR protocols)

//#define ANALYZE
//#define IR_ERR
//#define IR_LOG
//#define DEBUG


#if F_CPU == 16000000
	// Timing for 16MHz
	#define RUWIDO_SHORT_MIN 10
	#define RUWIDO_SHORT_MAX 16

	#define RUWIDO_LONG_MIN  23
	#define RUWIDO_LONG_MAX  30

	#define RUWIDO_MIN_NUM_BITS  26
	#define RUWIDO_MAX_NUM_BITS  33

#elif F_CPU == 12000000
	// Timing for 12MHz
	#define RUWIDO_SHORT_MIN 7 // 7.5
	#define RUWIDO_SHORT_MAX 12 // 12.0

	#define RUWIDO_LONG_MIN  17 // 17.25
	#define RUWIDO_LONG_MAX  22 //22.5

	#define RUWIDO_MIN_NUM_BITS  26 // 19.5
	#define RUWIDO_MAX_NUM_BITS  33 // 24.75

#else
	#error "This code runs only with 12 or 16MHz"
#endif


#define NEC_BURST_MIN 545
#define NEC_BURST_MAX 555

#define NEC_SPACE_MIN 275
#define NEC_SPACE_MAX 285

#define NEC_ZERO_MIN 30
#define NEC_ZERO_MAX 40

#define NEC_ONE_MIN 100
#define NEC_ONE_MAX 110

#define NEC_NUM_BITS 32

// states

#define IRST_START0    0
#define IRST_START1    1
#define IRST_RW_LOW    2
#define IRST_RW_HIGH   3

#define IRST_NEC_SPACE 4
#define IRST_NEC_LOW   5
#define IRST_NEC_HIGH  6
#define IRST_NEC_FINI  7

static uint8_t state = IRST_START0;

#ifdef ANALYZE
#define IR_LOG
#endif

#ifdef IR_LOG

#define LOG_SIZE 64

static uint16_t log[LOG_SIZE];
static uint8_t log_n = 0;

#endif


#define IRD_FIFO_SIZE 8

static volatile IR_DATA ird[IRD_FIFO_SIZE];
static volatile uint8_t ird_in  = 0; 
static volatile uint8_t ird_out = 0; 
static volatile uint8_t ird_n   = 0; 

#ifndef ANALYZE

static uint32_t data=0;
static uint32_t data_cnt=0;
static uint16_t diff;

static void ird_queue(uint8_t protocol, uint16_t address, uint16_t command, uint8_t flags) {
	if (ird_n >= IRD_FIFO_SIZE) 
		return;

#ifdef DEBUG
	uart_puts ("QUEUE ird_in=");
	uart_print8 (ird_in);
	uart_puts (", ird_n =");
	uart_print8 (ird_n);
	uart_println();
#endif
	ird[ird_in].protocol = protocol;
	ird[ird_in].command = command;
	ird[ird_in].address = address;
	ird[ird_in].flags = flags;

	ird_in = (ird_in + 1) & 7;
	ird_n++;


}


static void ruwido_dump(char *msg) {
#ifdef IR_ERR
	uart_puts (msg);
	uart_puts (", state=");
	uart_print8(state);
	uart_puts (", cnt=");
	uart_print8(data_cnt);
	uart_puts (", diff=");
	uart_print16(diff);
	uart_puts (", data=");
	uart_print32h(data);
	uart_puts (", adr=");
	uart_print16(data >> (data_cnt - 11));
	uart_puts (", cmd=");
	uart_print16(data & 0xfff);
	uart_println();

#ifdef IR_LOG
	uart_puts ("IR: LOG ");
	for (uint8_t i = 0; i<LOG_SIZE; i++) {
		uint8_t idx = (i + log_n) & 63;
		uint8_t dd = log[idx];
		if (dd>32) {
			uart_putc ('|');
		}
		uart_print8 (dd);
		uart_putc (' ');
	}
	uart_println ();
#endif
#endif
}



void ir_ISR(void) {

	// compute diff, compensate timer wraps
	
	static uint16_t otmr = 0;
	uint16_t tmr = TCNT1 & 0x7FFF;

	diff = tmr > otmr ? tmr - otmr : 32768 - otmr + tmr;

#ifdef IR_LOG
	log[log_n] = diff;
	log_n = (log_n + 1) & 63;
#endif

	otmr = tmr;

	switch (state) {

		case IRST_START0:
			if (ir_input() ) {
				state = IRST_START1;
			}
			break;

		case IRST_START1:
			if ((diff >= RUWIDO_SHORT_MIN) && (diff <= RUWIDO_SHORT_MAX)) {
				state = IRST_RW_LOW;
				data = 0;
				data_cnt = 0;
			} else if ((diff >= NEC_BURST_MIN) && (diff <= NEC_BURST_MAX))  {
				state = IRST_NEC_SPACE;
				data = 0;
				data_cnt = 0;
			} else 
				state = IRST_START0;
			break;

		case IRST_RW_LOW:
			{

				if ((diff >= RUWIDO_SHORT_MIN) && (diff <= RUWIDO_SHORT_MAX)) {


					data = (data << 1) | 1;
					data_cnt++;

				} else if ((diff >= RUWIDO_LONG_MIN) && (diff <= RUWIDO_LONG_MAX)) {

					data = (data << 1) ;
					data_cnt++;

				} else if ((diff > RUWIDO_LONG_MAX) && (data_cnt >= 16)) {

					// try to make sense of the data we have

					if (data_cnt == 16) {

						// key release
						ird_queue (IRP_RUWIDO, data, 0, IRF_KEY_RELEASE);

					} else if (data_cnt >= RUWIDO_MIN_NUM_BITS) {

						ird_queue (IRP_RUWIDO,  data >> (data_cnt - 16), data & 0xfff, 0);

					} else {
						ruwido_dump ("IR: RUWIDO PROTOCOL ERR LOW II");
					}
					state = IRST_START1;
					break;

				} else if ((diff < RUWIDO_SHORT_MIN) || (diff > RUWIDO_SHORT_MAX && diff < RUWIDO_LONG_MIN) || (diff > RUWIDO_LONG_MAX)) {
					ruwido_dump ("IR: RUWIDO PROTOCOL ERR LOW");
					state = IRST_START1;
					break;
				}

				state = IRST_RW_HIGH;
			}
			break;

		case IRST_RW_HIGH:

			if ((diff >= RUWIDO_SHORT_MIN) && (diff <= RUWIDO_SHORT_MAX)) {

				data = (data << 1);
				data_cnt++;

			} else if ((diff >= RUWIDO_LONG_MIN) && (diff <= RUWIDO_LONG_MAX)) {

				data = (data << 1) | 1;
				data_cnt++;

			} else if ((diff < RUWIDO_SHORT_MIN) || (diff > RUWIDO_SHORT_MAX && diff < RUWIDO_LONG_MIN) || (diff > RUWIDO_LONG_MAX)) {
				ruwido_dump ("IR: RUWIDO PROTOCOL ERR HIGH");
				state = IRST_START0;
				break;
			}
			
			state = IRST_RW_LOW;

			break;

		case IRST_NEC_SPACE:
			if ((diff >= NEC_SPACE_MIN) && (diff <= NEC_SPACE_MAX))  {
				state = IRST_NEC_HIGH;
				data_cnt = 0;
			} else {
				//uart_puts ("IR: NEC SPACE ERR\r\n");
				state = IRST_START0;
			}
			break;


		case IRST_NEC_HIGH:
			state = IRST_NEC_LOW;	
			break;

		case IRST_NEC_LOW:

			if ((diff >= NEC_ZERO_MIN) && (diff <= NEC_ZERO_MAX))  {
				data = (data << 1) ;
				data_cnt++;
			} else 	if ((diff >= NEC_ONE_MIN) && (diff <= NEC_ONE_MAX))  {
				data = (data << 1) | 1;
				data_cnt++;
			} else {
				uart_puts ("IR: NEC DATA ERR diff=");
				uart_print16(diff);
				uart_puts (", cnt=");
				uart_print8(data_cnt);
				uart_puts (", data=");
				uart_print32h(data);
				uart_println();
				state = IRST_START0;
				break;
			}

			if (data_cnt >= NEC_NUM_BITS) {
				ird_queue (IRP_NEC, data >> (data_cnt - 16), data & 0xfff, 0);
				state = IRST_NEC_FINI;
				break;
			}

			state = IRST_NEC_HIGH;	
			break;

		case IRST_NEC_FINI:
			if (ir_input() ) {
				state = IRST_START0;
			}
			break;

		default:
			uart_puts ("UNKNOWN STATE\r\n");
			state = IRST_START0;
	}
}

#else


void ir_ISR(void) {

	static uint16_t otmr = 0;
	uint16_t tmr = TCNT1 & 0x7FFF;

	uint16_t diff = tmr > otmr ? tmr - otmr : 32768 - otmr + tmr;

	log[log_n++] = diff;
	otmr = tmr;

	if (log_n >= LOG_SIZE) {
		if (log_n) {
			uart_puts ("IR_DIFF: cnt=");
			uart_print8 (log_n);
			uart_puts (" DATA ");
			for (uint8_t i = 0; i<log_n; i++) {
				uart_print16 (log[i]);
				uart_putc (' ');
			}
			uart_println ();

			uart_println ();
		}
		log_n = 0;
	}
}
#endif


uint8_t ir_get_data(IR_DATA *data) {

	if (!ird_n)
		return 0;

#ifdef DEBUG
	uart_puts ("GET ird_out=");
	uart_print8 (ird_out);
	uart_puts (", ird_n =");
	uart_print8 (ird_n);
	uart_println();
#endif


	data->address = ird[ird_out].address;
	data->command = ird[ird_out].command;
	data->flags = ird[ird_out].flags;

	ird_out = (ird_out + 1) & 7;
	ird_n--;

	return 1;
}

