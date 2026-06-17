/*
 * Name:         Miguel Diaz-Alvarez
 * Date:         6/12/2025
 * Assignment:   Lab 2
 * YouTube:      https://www.youtube.com/watch?v=i2ZxS8_4vCs           
*/

#include <msp430.h>
#include <stdbool.h>

//Function Protypes
void INIT();
static inline void GREEN_LED(bool enable);
static inline void RED_LED(bool enable);
static inline void BOTH_LED(bool enable);
static inline bool BUTTON_PRESSED();

int count = 0;
int main() {               
     
    INIT();
    RED_LED(true);

    while(1){

        
        if(BUTTON_PRESSED()) {
            count = count + 1;
            if ((count % 3) == 1){
                RED_LED(false);
                BOTH_LED(false);
                GREEN_LED(true);
                __delay_cycles(400000); //Software debouncing 
            } else if ((count % 3) == 2) {
                RED_LED(false);
                GREEN_LED(false);
                BOTH_LED(true);
                __delay_cycles(400000); //Software debouncing
            } else if ((count % 3) == 0) {
                GREEN_LED(false);
                BOTH_LED(false);
                RED_LED(true);
                __delay_cycles(400000); //Software debouncing
            }
        } 

    }
}

void INIT() {
    WDTCTL = 0x5A80;    // Stop watchdog timer

    P1DIR = ~BIT3;
    P1REN = BIT3;
    P1OUT = BIT3;
}

static inline void GREEN_LED(bool enable){
    P1OUT = enable ? P1OUT | BIT0 : P1OUT & ~BIT0;
}

static inline void RED_LED(bool enable){
    P1OUT = enable ? P1OUT | BIT6 : P1OUT & ~BIT6;
}

static inline void BOTH_LED(bool enable){
    P1OUT = enable ? P1OUT | BIT0 : P1OUT & ~BIT0;
    P1OUT = enable ? P1OUT | BIT6 : P1OUT & ~BIT6;
}

static inline bool BUTTON_PRESSED(){
    if ((P1IN & BIT3) == 0x00) {
        return true;
    } else {
        return false;
    }
}



