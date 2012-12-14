/* Name: main.c
 * Project: RUWIDO Merlin IR-USB-HID-Receiver
 * Author: Alexander Meyer
 * based on HID-Test by Christian Starkjohann, Objective Development
 * and the RUWIDO Merlin IR decoder by Guenter Bartsch
 * Creation Date: 2012-12-13
 * Tabsize: 2
 * Copyright: (c) 2012 Alexander Meyer
 * License: GNU GPL v2 (see License.txt)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ir/ir.h"
#include "uart/uart.h"
#include "usbdrv.h"
#include "oddebug.h"
#include "keymap.h"


//#define UART // This breaks USB!!

/* ----------------------- hardware I/O abstraction ------------------------ */

/* pin assignments:
PD0	USB-
PD1	debug tx
PD2	USB+ (int0)
PD3 IR (int1)
*/

static void hardwareInit(void)
{
uchar	i, j;

    PORTB = 0xff;   /* activate all pull-ups */
    DDRB = 0;       /* all pins input */
    PORTC = 0xff;   /* activate all pull-ups */
    DDRC = 0;       /* all pins input */
    PORTD = 0xfa;   /* 1111 1010 bin: activate pull-ups except on USB lines */
    DDRD = 0x07;    /* 0000 0111 bin: all pins input except USB (-> USB reset) */
	j = 0;
	while(--j){     /* USB Reset by device only required on Watchdog Reset */
		i = 0;
		while(--i); /* delay >10ms for USB reset */
	}
    DDRD = 0x02;    /* 0000 0010 bin: remove USB reset condition */
    /* configure timer 0 for a rate of 12M/(1024 * 256) = 45.78 Hz (~22ms) */
    //TCCR0 = 5;      /* timer 0 prescaler: 1024 */
}

static void irInit (void) {

	// port directions

	DDRB = 0 ;
	DDRD = (1<<1) ; // TX


	// IR 


	// timer1 will be used basically as a real time clock
	
	TCCR1A  = 0; // normal mode
        TCCR1B  = (1 << CS12) | (0 << CS11) | (0 << CS10); // prescaler: 256, normal mode

	// IR trigger interrupt

  #if defined (__AVR_ATmega8__) || defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega64__) || defined (__AVR_ATmega162__)

 	GIMSK  |= (1<<INT1);     // enable pin change interrupt on PIND3
	MCUCR  |= (1<<ISC10);    // INT1 triggers on any level change

	#else

 	//PCMSK0  = (1<<PCINT1);     // enable pin change interrupt on PINB1
 	//PCICR   = (1<<PCIE0);
 	PCMSK2  = (1<<PCINT19);     // enable pin change interrupt on PIND3
 	PCICR   = (1<<PCIE2);       // enable pcint for int group 2

	#endif

}

#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega64__) || defined (__AVR_ATmega162__)
ISR(INT1_vect) { 
	ir_ISR();
} 
#else
//ISR(PCINT0_vect) { 
ISR(PCINT2_vect) { 
	ir_ISR();
} 
#endif

// Lookup Keys from Key Table

uchar mod_key = 0;

static int lookupKey(uint32_t merlinKey) {
	for (int i = 0; i<= NUM_MODS; i++) {
		if (modmap[i].merlin == merlinKey) {
			mod_key |= (modmap[i].usb);
			return 0;
		}
	}
	for (int i = 0; i<= NUM_KEYS; i++) {
		if (keymap[i].merlin == merlinKey) {
			return keymap[i].usb;
		}
	}
	return 0;
}


// Process key press

volatile uint32_t lastIrCode = 0;

static uchar    keyPressed(void) {

	IR_DATA ird;
	if (ir_get_data(&ird)) {

#if DEBUG_LEVEL > 0
		uart_puts ("IR: ");
		uart_puts (" adr=");
		uart_print16h(ird.address);
		uart_puts (" cmd=");
		uart_print16h(ird.command);
		uart_puts (" flags=");
		uart_print8(ird.flags);
		uart_println ();
#endif

		if (ird.address == 0x2aab) {
			if (ird.command != 0 && ird.flags == 0) {
				lastIrCode = lookupKey(ird.command);
			} else {
				lastIrCode = 0;
			}
		}
	}
	return lastIrCode;
}

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

static uchar    reportBuffer[2];    /* buffer for HID reports */
static uchar    idleRate;           /* in 4 ms units */

/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and we only allow one
 * simultaneous key press (except modifiers). We can therefore use short
 * 2 byte input reports.
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */

PROGMEM char usbHidReportDescriptor[35] = { /* USB report descriptor */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};


// Build USB Report

static void buildReport(uchar key) {
	reportBuffer[0] = mod_key;
	reportBuffer[1] = key;
	mod_key = 0;
}

uchar	usbFunctionSetup(uchar data[8]) {
	
		usbRequest_t    *rq = (void *)data;

    usbMsgPtr = reportBuffer;
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    // class request type
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  // wValue: ReportType (highbyte), ReportID (lowbyte)
            // we only have one report type, so don't look at wValue
            buildReport(keyPressed());
            return sizeof(reportBuffer);
        }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = &idleRate;
            return 1;
        }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
            idleRate = rq->wValue.bytes[1];
        }
    }else{
        // no vendor specific requests implemented
    }
	return 0;
}


/* ------------------------------------------------------------------------- */


int	main(void)
{
uchar   key, lastKey = 0, keyDidChange = 0;
uchar   idleCounter = 0;

	wdt_enable(WDTO_2S);
	hardwareInit();
	odDebugInit();
	usbInit();
	irInit();

#if DEBUG_LEVEL > 0
	uart_println();
	uart_println();
	uart_println();
	uart_puts ("IR test\r\n");
	uart_println();
#endif

	sei();
    DBG1(0x00, 0, 0);
	for(;;){	/* main event loop */
		wdt_reset();
		usbPoll();
					key = keyPressed();
					if(lastKey != key){
							lastKey = key;
							keyDidChange = 1;
					}
					#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega64__) || defined (__AVR_ATmega162__)
					if(TIFR & (1<<TOV0)){   // 22 ms timer 
							TIFR = 1<<TOV0;
					#else
					if(TIFR0 & (1<<TOV0)){   // 22 ms timer 
							TIFR0 = 1<<TOV0;
					#endif
							if(idleRate != 0){
									if(idleCounter > 4){
											idleCounter -= 5;   // 22 ms in units of 4 ms
									}else{
											idleCounter = idleRate;
											keyDidChange = 1;
									}
							}
					}
					if(keyDidChange && usbInterruptIsReady()){
							keyDidChange = 0;
							// use last key and not current key status in order to avoid lost changes in key status.
							buildReport(lastKey);
							usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
					}
	}
	return 0;
}

/* ------------------------------------------------------------------------- */
