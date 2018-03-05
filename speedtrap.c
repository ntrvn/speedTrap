/********************************************
*
*  Name: Nhan Tran
*  Section: Wednesday 12:30pm
*  Assignment: Final Project - Speed Trap
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
# include <stdio.h>
#include "speedtrap.h"
#include "lcd.h"
#include "encoder.h"
#include "serial.h"

#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

int main(void) {

    // Initialize the LCD
    lcd_init();

    // Initialize Phototransistor
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);
    PORTB |= (1 << PB3) | (1 << PB4);

    // Initialize time function
    init_timer1(2000);

    // initialize interupt
    PCICR |= (1<<PCIE2);
    PCMSK2 |= (1<<PCINT19) | (1<<PCINT18);
    sei();

    // initialize encoder
    DDRB |= (1<<4);
    DDRC &= ~(1<<1);
    DDRC &= ~(1<<5);
    PORTC |= (1<<1) | (1<<5);
    PCICR |= (1<<PCIE1);
    PCMSK1 |= ((1 << PCINT9)| (1 << PCINT13));

    y = PINC;
    c = x & (1<<1);
    d = x & (1<<5);

    if (!d && !c)
    encoderOldState = 0;
    else if (!d && b)
    encoderOldState = 1;
    else if (d && !b)
    encoderOldState = 2;
    else
    encoderOldState = 3;

    encoderNewState = encoderOldState;

    // initialize serial
    serial_init(MYUBRR);

    //initialize variables
    second = 0;
    bufferCount=0;
    oldState = 0;
    newState = 0;
    count = eeprom_read_byte((void *) 100);
    flag = 0;
    serialFlag=0;
    encoderFlag=0;
    speed = 0;
    int charCount=0;
    char recievingBuffer[5];
    char timeBuffer[7];
    char maxBuffer[6];
    char speedBuffer[7];
    char transmitBuffer[4];

    // Write splash screen
    lcd_writecommand(1);
    lcd_stringout("Nhan Tran");
    lcd_moveto(1,0);
    lcd_stringout("Final Project");
    _delay_ms(1000);
    lcd_writecommand(1);


    while(1) {
        // display maximum speed
        if (encoderFlag == 1) {
            eeprom_update_byte((void *) 100, count);
            lcd_moveto(1,0);
            snprintf(maxBuffer,6,"m %d",count);
            lcd_stringout(maxBuffer);
            encoderFlag = 0;

        }

        // display speed
        if (flag == 1) {
            lcd_moveto(0,0);
            snprintf(timeBuffer,7,"%dms",second);
            lcd_stringout(timeBuffer);
            lcd_moveto(0,9);

            speed = 45720/second;         // calculate speed in mm/s

            // transmit speed
            snprintf(transmitBuffer,4,"%d",speed);
            serial_stringout(transmitBuffer);

            // print out speed in cm/s
            snprintf(speedBuffer,7,"%3d.%d",speed/10,speed%10);
            lcd_stringout(speedBuffer);
            flag = 0;
        }

        // display the recieving speed
        if (serialFlag == 1) {
            int output=0;
            char outputBuff[7];
            sscanf (serialBuffer, "%d", &output);

            // play note if speed > maximum speed
            if (output > count * 10) {
                play_note(494);
            }

            // print out recieving speed in cm/s
            snprintf(outputBuff,7,"%3d.%d",output/10,output%10);
            lcd_moveto(1,10);
            lcd_stringout(outputBuff);

            serialFlag = 0;
        }

    }


    return 0;
}

// ISR for phototransmitter
ISR (PCINT2_vect) {
    x= PIND;
    a = x & (1<<PD2);
    b = x & (1<<PD3);

    if (oldState == 0) {
        if (!a) {
            second = 0;
            newState = 1;
            PORTB |= (1 << PB5);
        }
    }
    else if (oldState == 1) {
        if (!b) {
            newState = 0;
            flag = 1;
            PORTB &= ~(1 << PB5);
        }
    }
    oldState = newState;

}

// ISR for timer
ISR(TIMER1_COMPA_vect) {
    if (newState == 1) {
        second++;
    }
}

// intialize timer
void init_timer1(unsigned short m) {
    TCCR1B &= 0xe0;
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = m;
    TCCR1B |= (1 << CS11);

}

// function to play note
void play_note(unsigned short freq) {

    int playtime = 60;
    while (playtime--) {
    // Make PB4 high
    PORTB |= (1<<4);
    // Use variable_delay_us to delay for half the period
    variable_delay_us(5000);
    // Make PB4 low
    PORTB &= ~(1<<4);
    // Delay for half the period again
    variable_delay_us(5000);
    }
}

// delaying between playing note
void variable_delay_us(int delay)
{
    int i = (delay + 5) / 10;

    while (i--) _delay_us(10);
}



