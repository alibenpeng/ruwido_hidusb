#include <inttypes.h>
#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>

#include "uart.h"

#define UART_BAUD_RATE 38400

#define UART_BAUD_SELECT(baudRate) ((F_CPU)/((baudRate)*16l)-1)


void uart_init(void) {

#ifdef UCSRC
        UBRRL = UART_BAUD_SELECT(UART_BAUD_RATE) & 0xff;
        UBRRH = UART_BAUD_SELECT(UART_BAUD_RATE) >> 8;

        UCSRC = 1<<UCSZ1^1<<UCSZ0            // 8 Bit
#ifdef URSEL
                ^1<<URSEL                    // if UCSR0C shared with UBRR0H
#endif
                ;
        UCSRB = 1<<RXEN | 1<<TXEN;           // enable RX, TX
        UCSRA = 0;                           // 1x speed
#else
        UBRR0 = UART_BAUD_SELECT(UART_BAUD_RATE) ;

        UCSR0C = 1<<UCSZ01^1<<UCSZ00         // 8 Bit
#ifdef URSEL
                ^1<<URSEL0                   // if UCSR0C shared with UBRR0H
#endif
                ;
        UCSR0B = 1<<RXEN0 | 1<<TXEN0;        // enable RX, TX
        UCSR0A = 0;                          // 1x speed
#endif

}

void uart_putc(unsigned char c) {
#ifdef UDR
    while (!(UCSRA & (1<<UDRE)));
 
    UDR = c;
#else
    while (!(UCSR0A & (1<<UDRE0)));
 
    UDR0 = c;
#endif
}

void uart_puts (char *s) {
	while (*s) {
		uart_putc(*s);
		s++;
	}
}

void uart_print4h(uint8_t n) {
	if (n<10)
		uart_putc('0' + n);
	else
		uart_putc ('A' + (n-10));
}

void uart_print8h(uint8_t n) {

	uart_puts("0x");
	uart_print4h((n >>  4) & 0xf);
	uart_print4h(n & 0xf);
}

void uart_print16h(uint16_t n) {

	uart_puts("0x");
	uart_print4h((n >> 12) & 0xf);
	uart_print4h((n >>  8) & 0xf);
	uart_print4h((n >>  4) & 0xf);
	uart_print4h(n & 0xf);
}

void uart_print32h(uint32_t n) {

	uart_puts("0x");
	uart_print4h(n >> 28);
	uart_print4h((n >> 24) & 0xf);
	uart_print4h((n >> 20) & 0xf);
	uart_print4h((n >> 16) & 0xf);
	uart_print4h((n >> 12) & 0xf);
	uart_print4h((n >>  8) & 0xf);
	uart_print4h((n >>  4) & 0xf);
	uart_print4h(n & 0xf);
}

void uart_print16(uint16_t n) {

	uart_putc('0' + n / 10000);
	uart_putc('0' + (n / 1000) % 10);
	uart_putc('0' + (n / 100) % 10);
	uart_putc('0' + (n / 10) % 10);
	uart_putc('0' + (n ) % 10);

}

void uart_print8(uint8_t n){
	uart_putc('0' + (n / 100) % 10);
	uart_putc('0' + (n / 10) % 10);
	uart_putc('0' + (n ) % 10);
}

void uart_println(void) {
  	uart_putc ('\r');
  	uart_putc ('\n');
}

