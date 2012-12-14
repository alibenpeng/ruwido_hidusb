#ifndef HAVE_UART_H
#define HAVE_UART_H

void uart_init(void);

void uart_putc(unsigned char c);

void uart_puts (char *s);

void uart_print32h(uint32_t i);
void uart_print16h(uint16_t i);
void uart_print8h(uint8_t i);

void uart_print16(uint16_t i);
void uart_print8(uint8_t i);
void uart_println(void) ;
#endif
