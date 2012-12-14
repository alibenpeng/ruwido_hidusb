
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


#ifndef HAVE_IR_H
#define HAVE_IR_H

#include <inttypes.h>

// flags

#define IRF_KEY_RELEASE  1

// protocol
#define IRP_RUWIDO      1
#define IRP_NEC         2

typedef struct
{
	uint8_t               protocol;
	uint16_t              address;
	uint16_t              command;
	uint8_t               flags;

} IR_DATA;


// call this on each input pin change interrupt
void ir_ISR(void);

// get next ir data item
// returns 0 if no new data available, 1 otherwise
uint8_t ir_get_data(IR_DATA *data);


#endif

