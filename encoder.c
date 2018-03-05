#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "speedtrap.h"
#include "lcd.h"

ISR(PCINT1_vect) {
    x= PINC;
    a = x & (1<<1);
    b = x & (1<<5);

    if (encoderOldState == 0) {
        // Handle A and B inputs for state 0
        if (a) {
            encoderNewState = 1;
            if (count < 99) {
                count++;
            }
        }
        else if (b) {
            encoderNewState = 2;
            if (count > 0) count--;
        }
    }
    else if (encoderOldState == 1) {
        // Handle A and B inputs for state 1
        if (b) {
            encoderNewState = 3;
            if (count < 99) count++;
        }
        else if (!a) {
            encoderNewState = 0;
            if (count > 0) count--;
        }
    }
    else if (encoderOldState == 2) {
        // Handle A and B inputs for state 2
        if (!b) {
            encoderNewState = 0;
            if (count < 99) count++;
        }
        else if (a) {
            encoderNewState = 3;
            if (count > 0) count--;
        }

    }
    else {   // old_state = 3
        // Handle A and B inputs for state 3
        if (!a) {
            encoderNewState = 2;
            if (count < 99) count++;
        }
        else if (!b) {
            encoderNewState = 1;
            if (count > 0) count--;
        }
    }

    encoderOldState = encoderNewState;

    encoderFlag = 1;
}