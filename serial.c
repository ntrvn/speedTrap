#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "speedtrap.h"
#include "lcd.h"

// inialize serial
void serial_init(unsigned short ubrr_value)
{
    // Set up USART0 registers
    UBRR0 = ubrr_value;
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0); // Enable RX and TX
    UCSR0C = (3 << UCSZ00); // Async., no parity, 1 stop bit, 8 data bits
    UCSR0B |= (1<<RXCIE0);
    // Enable tri-state
    DDRC |= (1<<3);
    PORTC &= ~(1<<3);
}

void serial_txchar(char next_character)
{
    while ((UCSR0A & (1<<UDRE0)) == 0);
    UDR0 = next_character;
}

void serial_stringout(char* s)
{
    // Call serial_txchar in loop to send a string
    char serialCount=0;
    serial_txchar('@');
    while (s[serialCount] != '\0') {
        serial_txchar(s[serialCount]);
        serialCount++;
    }
    serial_txchar('$');
}

ISR(USART_RX_vect)
{
    // Get the received charater
    char ch;
    ch = UDR0;

    if (ch == '@') {
        bufferCount=0;
    }

    else if (ch >= '0' && ch <= '9') {
        serialBuffer[bufferCount] = ch;
        bufferCount++;
    }

    else if (ch == '$') {
        serialBuffer[bufferCount] = NULL;
        serialFlag = 1;
        bufferCount=0;
    }
}
