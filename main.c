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

uchar modKey = 0;
uchar keyType = 1;
volatile uint16_t remoteCounter = 0;

static uint16_t lookupKey(uint32_t address, uint32_t command) {
	if (address == 0x2aab) {
		keyType = 1;
		for (int i = 0; i< NUM_MODS; i++) {
			if (modmap[i].ruwido == command) {
				// FIXME: This works, except for Ctrl-Alt-L, dog knows why..
				modKey |= (modmap[i].usb); // aggregate modifiers
				return 0;
			}
		}
		for (int i = 0; i< NUM_MERLIN_KEYS; i++) {
			if (merlinKeymap[i].ruwido == command) {
				return merlinKeymap[i].usb;
			}
		}
		// Remote Control Keys on the Keyboard like Power, Volume...
		for (int i = 0; i< NUM_MERLIN_REMOTE_KEYS; i++) {
			if (merlinRemoteKeymap[i].ruwido == command) {

#if DEBUG_LEVEL > 0
		uart_puts ("Key: ");
		uart_print16h(merlinRemoteKeymap[i].usb);
		uart_println ();
#endif
				remoteCounter++;
				keyType = 2;
				return merlinRemoteKeymap[i].usb;
			}
		}
	} else if (address == 0x4aad || address == 0x4aac) {
		keyType = 2;
		for (int i = 0; i< NUM_ALICE_KEYS; i++) {
			if (aliceKeymap[i].ruwido == command) {
				remoteCounter++;
				return aliceKeymap[i].usb;
			}
		}
	}
	return 0;
}


// Process key press

volatile uint32_t lastIrCode = 0;

static uint16_t    keyPressed(void) {

	if (remoteCounter > 0) {
		remoteCounter++;
		//lastIrCode = 0;
	}
	if (remoteCounter > 10000) {
		remoteCounter = 0;
		lastIrCode = 0;
	}

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

		if (ird.address == 0x2aab || ird.address == 0x4aad || ird.address == 0x4aac) { // Merlin Keyboard or Alice Remote
			if (ird.command != 0 && ird.flags == 0) {
				lastIrCode = lookupKey(ird.address, ird.command);
			} else if (keyType == 1) {
				lastIrCode = 0;
			}
		}
	}
	return lastIrCode;
}

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

static uchar    reportBuffer[4];    /* buffer for HID reports */
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

PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { // USB report descriptor
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x01,                    //   Report ID (1, Keyboard)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
//  -------------------------------------------------------------------------
  0x95, 0x05,//   REPORT_COUNT (5)                  |
  0x75, 0x01,//   REPORT_SIZE (1)                   |Defines byte 1 form when byte
  0x05, 0x08,//   USAGE_PAGE (LEDs)                 |0 is 1 (keyboard) for data transmission.
  0x19, 0x01,//   USAGE_MINIMUM (Num Lock)              |Represents the status of the PC's LEDs
  0x29, 0x05,//   USAGE_MAXIMUM (Kana)                |(Caps Lock, Num Lock etc).  5-bits.
  0x91, 0x02,//   OUTPUT (Data,Var,Abs)               |
//  -------------------------------------------------------------------------
  0x95, 0x01,//   REPORT_COUNT (1)                  |
  0x75, 0x03,//   REPORT_SIZE (3)                   |3-bits to pad previous section to a byte.
  0x91, 0x03,//   OUTPUT (Cnst,Var,Abs)               |
//  -------------------------------------------------------------------------
  0x95, 0x06,//   REPORT_COUNT (6)                  |
  0x75, 0x08,//   REPORT_SIZE (8)                   |Defines bytes 2-7 form when byte
  0x15, 0x00,//   LOGICAL_MINIMUM (0)                 |0 is 1 (keyboard). Represents
  0x25, 0x65,//   LOGICAL_MAXIMUM (101)               |any of the regular keyboard
  0x05, 0x07,//   USAGE_PAGE (Keyboard)               |characters
  0x19, 0x00,//   USAGE_MINIMUM (Reserved (no event indicated))   |
  0x29, 0x65,//   USAGE_MAXIMUM (Keyboard Application)        |
  0x81, 0x00,//   INPUT (Data,Ary,Abs)                |
  0xc0,//     END_COLLECTION                      |
//  -------------------------------------------------------------------------

/*
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                           // END_COLLECTION

*/
// remote control


    0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                    // USAGE (Consumer Control)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //   Report ID (2, Remote Control)

// keypad (byte 1, bits 0-3)

    0x09, 0x02,                    //   USAGE (Numeric Key Pad)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x0a,                    //     USAGE_MAXIMUM (Button 10)
    0x15, 0x01,                    //     LOGICAL_MINIMUM (1)
    0x25, 0x0a,                    //     LOGICAL_MAXIMUM (10)
    0x75, 0x04,                    //     REPORT_SIZE (4)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x00,                    //     INPUT (Data,Ary,Abs)
    0xc0,                          //   END_COLLECTION

// volume and channel up/down buttons (byte1, bits 4-7)

    0x05, 0x0c,                    //   USAGE_PAGE (Consumer Devices)
    0x09, 0x86,                    //   USAGE (Channel)
    0x09, 0xe0,                    //   USAGE (Volume)
    0x15, 0xff,                    //   LOGICAL_MINIMUM (-1)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x02,                    //   REPORT_SIZE (2)
    0x95, 0x02,                    //   REPORT_COUNT (2)
    0x81, 0x46,                    //   INPUT (Data,Var,Rel,Null)

// custom buttons on merlin and alice remote (byte 2, bits 0-3)

    0x09, 0xe2,                    //   USAGE (Mute)        0x01
    0x09, 0x30,                    //   USAGE (Power)       0x02
    0x09, 0x41,                    //   Menu Pick           0x03
    0x09, 0x42,                    //   Menu Up             0x04
    0x09, 0x43,                    //   Menu Down           0x05
    0x09, 0x44,                    //   Menu Left           0x06
    0x09, 0x45,                    //   Menu Right          0x07
    0x09, 0x46,                    //   Menu Escape         0x08
    0x09, 0x89,                    //   Media Select TV     0x09
    0x09, 0x8b,                    //   Media Select DVD    0x0a
    0x09, 0x95,                    //   Help                0x0b
    0x09, 0xcd,                    //   Play/Pause          0x0c
    0x09, 0xb2,                    //   Record              0x0d
    0x09, 0xb3,                    //   Fast Forward        0x0e
    0x09, 0xb4,                    //   Rewind              0x0f
    0x09, 0xb5,                    //   Next Track          0x10
    0x09, 0xb6,                    //   Previous Track      0x11
    0x09, 0xb7,                    //   Stop                0x12
    0x09, 0x40,                    //   Menu                0x13

/*
    0x09, 0x34,                    //   USAGE (Sleep Mode)
    0x09, 0x60,                    //   USAGE (Data On Screen)
    0x09, 0x64,                    //   USAGE (Broadcast Mode)
    0x09, 0x83,                    //   USAGE (Recall Last)
    0x09, 0x81,                    //   USAGE (Assign Selection)
*/

    0x15, 0x01,                    //   LOGICAL_MINIMUM (1)
    0x25, 0x23,                    //   LOGICAL_MAXIMUM (35)
    0x75, 0x06,                    //   REPORT_SIZE (6)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)

// choice buttons (byte 2, bits 4-5)

/*
    0x09, 0x80,                    //   USAGE (Selection)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x01,                    //     LOGICAL_MINIMUM (1)
    0x25, 0x03,                    //     LOGICAL_MAXIMUM (3)
    0x75, 0x02,                    //     REPORT_SIZE (2)
    0x81, 0x00,                    //     INPUT (Data,Ary,Abs)
    0xc0,                          //   END_COLLECTION
*/

// pad (byte 2, bits 6-7)

    0x15, 0x02,                    //   LOGICAL_MINIMUM (2)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xc0                           // END_COLLECTION
};


// Build USB Report

static void buildReport(uint16_t key) {
	reportBuffer[0] = keyType;
	if (keyType == 1) {
		reportBuffer[1] = modKey;
		reportBuffer[2] = key;
		modKey = 0;
	} else if (keyType == 2) {
		reportBuffer[1] = key;
		reportBuffer[2] = key >> 8;
#if DEBUG_LEVEL > 0
		uart_puts ("reportBuf1: ");
		uart_print16h(reportBuffer[1]);
		uart_println ();
		uart_puts ("reportBuf2: ");
		uart_print16h(reportBuffer[2]);
		uart_println ();
#endif
	}
}

uchar	usbFunctionSetup(uchar data[8]) {
	
		usbRequest_t    *rq = (void *)data;

    usbMsgPtr = reportBuffer;
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    // class request type
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  // wValue: ReportType (highbyte), ReportID (lowbyte)
            // we only have one report type, so don't look at wValue
#if DEBUG_LEVEL > 0
		uart_puts ("wValue: ");
		uart_print16h(rq->wValue.bytes[0]);
		uart_puts (", ");
		uart_print16h(rq->wValue.bytes[1]);
		uart_puts (", ");
		uart_print16h(rq->wValue.bytes[2]);
		uart_puts (", ");
		uart_print16h(rq->wValue.bytes[3]);
		uart_puts (", ");
		uart_print16h(rq->wValue.bytes[4]);
		uart_puts (", ");
		uart_print16h(rq->wValue.bytes[5]);
		uart_println ();
#endif
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
uint16_t   key, lastKey = 0;
uchar   idleCounter = 0, keyDidChange = 0;

	wdt_enable(WDTO_2S);
	hardwareInit();
	odDebugInit();

/*
usbDeviceDisconnect();
uchar i = 0;
while(--i){         // fake USB disconnect for > 250 ms
    wdt_reset();    // if watchdog is active, reset it
    _delay_ms(1);   // library call -- has limited range
}
*/
usbDeviceConnect();

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
			#if DEBUG_LEVEL > 0
			uart_puts ("ReportBuffer: ");
			uart_print16h(reportBuffer[0]);
			uart_puts (", ");
			uart_print16h(reportBuffer[1]);
			uart_puts (", ");
			uart_print16h(reportBuffer[2]);
			uart_puts (", ");
			uart_print16h(reportBuffer[3]);
			uart_println ();
			#endif

		}

	}
	return 0;
}

/* ------------------------------------------------------------------------- */
