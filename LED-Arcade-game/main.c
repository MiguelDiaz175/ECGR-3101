/*
 * Name:         Miguel Diaz-Alvarez
 * Date:         7/8/2025
 * Assignment:   Lab 3
 * YouTube:      https://youtu.be/K__UPkAbo_4
*/

#include "intrinsics.h"
#include "msp430g2553.h"
#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

//Function Prototypes 
void INIT();
static inline bool LEFT_TO_RIGHT();
static inline bool RIGHT_TO_LEFT();
static inline bool delay();
static inline bool INTERRUPT_CHECKER();
static inline bool VICTORY_PATTERN();
static inline void ALL_LEDS(bool enable);
static inline void BLUE_LED(bool enable);
static inline void RED_LED_1(bool enable);
static inline void RED_LED_2(bool enable);
static inline void RED_LED_3(bool enable);
static inline void RED_LED_4(bool enable);
static inline void RED_LED_5(bool enable);
static inline void RED_LED_6(bool enable);

//Global Variables
volatile bool globalButtonFlag = false;
volatile uint16_t solutionCheck = 0;

typedef enum {
    STATE_IDLE = 0,
    STATE_LEFT_TO_RIGHT,
    STATE_RIGHT_TO_LEFT,
    STATE_FROZEN,
    STATE_WINNER,
    STATE_LOSER
} state_t;

int main(){

    uint8_t currentState = STATE_IDLE;
    uint8_t nextState = currentState;

    INIT();

    while(1) {
        switch(currentState){
        case STATE_IDLE: { 
            
            if (INTERRUPT_CHECKER()){
                nextState = STATE_LEFT_TO_RIGHT;
            } 
            break;
            }
        case STATE_LEFT_TO_RIGHT: {  
            bool wasInterrupted = LEFT_TO_RIGHT();
            nextState = wasInterrupted ? STATE_FROZEN : STATE_RIGHT_TO_LEFT;
            break; 
            }  
        case STATE_RIGHT_TO_LEFT: {
            bool wasInterrupted = RIGHT_TO_LEFT();
            nextState = wasInterrupted ? STATE_FROZEN : STATE_LEFT_TO_RIGHT;
            break;
            }
        case STATE_FROZEN: {
            if (solutionCheck == 0) {
                nextState = STATE_WINNER;
            } else {
                nextState = STATE_LOSER;
            }
            break;
            }
        case STATE_WINNER: { 
            bool wasInterrupted = VICTORY_PATTERN();
            if (wasInterrupted) {
                ALL_LEDS(false);
                nextState = STATE_IDLE;
            }
            break;
            }
        case STATE_LOSER: { 
            if (INTERRUPT_CHECKER()) {
                ALL_LEDS(false);
                nextState = STATE_IDLE;
            }
            break;
            }
        }

    currentState = nextState;

    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1ISR(void){
    globalButtonFlag = true;

    P1IFG &= ~BIT3;   //Clear P1.3 IFG

}

void INIT() {
    WDTCTL = 0x5A80; //Stop watchdog timer 
    P2SEL &= ~BIT6;

    //Inputs
    P1DIR &= ~(BIT3);
    P1REN |= BIT3;
    P1OUT |= BIT3;

    //Outputs
    P2DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);  
    P2OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);

    
    //Interupt
    P1IES |= BIT3;
    P1IE |= BIT3;
    __enable_interrupt();
       

}

static inline bool INTERRUPT_CHECKER(){
    if (globalButtonFlag == true) {
        globalButtonFlag = false;

        return true;

    } else {
        return false;
    }
}

static inline bool VICTORY_PATTERN(){
        ALL_LEDS(false);
        BLUE_LED(true);
        if(INTERRUPT_CHECKER()) return true;
        __delay_cycles(125000);

        ALL_LEDS(false);
        BLUE_LED(true);
        RED_LED_3(true);
        RED_LED_4(true);
        if(INTERRUPT_CHECKER()) return true;
        __delay_cycles(125000);

        ALL_LEDS(false);
        BLUE_LED(true);
        RED_LED_2(true);
        RED_LED_5(true);
        if(INTERRUPT_CHECKER()) return true;
        __delay_cycles(125000);

        ALL_LEDS(false);
        BLUE_LED(true);
        RED_LED_1(true);
        RED_LED_6(true);
        if(INTERRUPT_CHECKER()) return true;
        __delay_cycles(125000);
        ALL_LEDS(false);
        return false;
}

static inline void BLUE_LED(bool enable){
    P2OUT = enable ? P2OUT | BIT0 : P2OUT & ~BIT0;
}

static inline void RED_LED_1(bool enable){
    P2OUT = enable ? P2OUT | BIT1 : P2OUT & ~BIT1;
}

static inline void RED_LED_2(bool enable){
    P2OUT = enable ? P2OUT | BIT2 : P2OUT & ~BIT2;
}

static inline void RED_LED_3(bool enable){
    P2OUT = enable ? P2OUT | BIT3 : P2OUT & ~BIT3;
}

static inline void RED_LED_4(bool enable){
    P2OUT = enable ? P2OUT | BIT4 : P2OUT & ~BIT4;
}

static inline void RED_LED_5(bool enable){
    P2OUT = enable ? P2OUT | BIT5 : P2OUT & ~BIT5;
}

static inline void RED_LED_6(bool enable){
    P2OUT = enable ? P2OUT | BIT6 : P2OUT & ~BIT6;
}

static inline void ALL_LEDS(bool enable){
    BLUE_LED (enable);
    RED_LED_1(enable);
    RED_LED_2(enable);
    RED_LED_3(enable);
    RED_LED_4(enable);
    RED_LED_5(enable);
    RED_LED_6(enable);
}

static inline bool LEFT_TO_RIGHT(){
    ALL_LEDS(false);

    RED_LED_1(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_1(false);

    RED_LED_2(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_2(false);

    RED_LED_3(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_3(false);

    BLUE_LED(true);
    solutionCheck = 0;
    if (INTERRUPT_CHECKER() || delay()) return true;
    BLUE_LED(false);

    RED_LED_4(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_4(false);
    
    RED_LED_5(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_5(false);

    RED_LED_6(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_6(false);

    return false;
}

static inline bool RIGHT_TO_LEFT(){
    RED_LED_5(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_5(false);

    RED_LED_4(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_4(false);

    BLUE_LED (true);
    solutionCheck = 0;
    if (INTERRUPT_CHECKER() || delay()) return true;
    BLUE_LED(false);

    RED_LED_3(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_3(false);

    RED_LED_2(true);
    solutionCheck = 1;
    if (INTERRUPT_CHECKER() || delay()) return true;
    RED_LED_2(false);

    return false;
}

static inline bool delay(){
    const uint32_t total = 60000u;
    const uint32_t step =  10000u;
    volatile uint32_t i = 0;

    for (i = 0; i < total; i += step) {
        __delay_cycles(step);
        if (globalButtonFlag) {
            globalButtonFlag = false;   
            return true;                
        }
    }
    return false;  
}


