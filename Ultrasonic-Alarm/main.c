/*
 * Name:         Miguel Diaz-Alvarez
 * Date:         7/23/2025
 * Assignment:   Lab 4
 * YouTube:      https://youtu.be/KOiui92-Ffc      
*/

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define TRIGGER_PIN         BIT1
#define ECHO_PIN            BIT4
#define SPEAKER_PIN         BIT0
#define BUTTON_PIN          BIT3
#define YELLOW_LED_PIN      BIT0
#define BLUE_LED_PIN        BIT1
#define RED_LED_PIN         BIT2


bool globalButtonFlag = false;

//Function prototypes
void INIT(void);
void SENSOR_TIMER(void);  
void SPEAKER_INIT(void);
static inline float GET_DISTANCE();
static inline void SET_FREQUENCY(float hertz);
static inline bool BUTTON_PRESSED();
static inline bool TRIP_WIRE_TEST();
static inline void ALARM_SOUND();
static inline void SPEAKER_OFF();
static inline void YELLOW_LED(bool enable);
static inline void BLUE_LED(bool enable);
static inline void RED_LED(bool enable);

//Custom Data Type
typedef enum {
    STATE_IDLE = 0,
    STATE_WATCH,
    STATE_ALARM
} state_t;

//Main
int main(void) {

    uint8_t currentState = STATE_IDLE;
    uint8_t nextState = currentState;

    INIT();
    SPEAKER_OFF();

    YELLOW_LED(true);
    BLUE_LED(false);
    RED_LED(false);
    while(1) {

        
        switch(currentState){
            case STATE_IDLE: 

                if (BUTTON_PRESSED()) {
                    nextState = STATE_WATCH;
                    YELLOW_LED(false);
                    BLUE_LED(true);
                    RED_LED(false);
                }
                break;
            
            case STATE_WATCH: 
                
                if (TRIP_WIRE_TEST()) {
                    nextState = STATE_ALARM;
                    YELLOW_LED(false);
                    BLUE_LED(false);
                    RED_LED(true);
                    SPEAKER_INIT();
                    ALARM_SOUND();
                }
                   
                break;
            
            case STATE_ALARM: 

                if (BUTTON_PRESSED()){
                    nextState = STATE_IDLE;
                    SPEAKER_OFF();
                    YELLOW_LED(true);
                    BLUE_LED(false);
                    RED_LED(false);
                }
                break;

        }

        currentState = nextState; 

    }

}

//Functions
void INIT(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // ECHO (P1.3) as input
    P1DIR  &= ~ECHO_PIN;    
    P1OUT  |= ECHO_PIN;    

    // TRIG (P1.1) as output
    P1DIR |= TRIGGER_PIN;
    P1OUT &= ~TRIGGER_PIN;

    // Button (P1.3) as input 
    P1DIR &= ~BUTTON_PIN;
    P1REN |= BUTTON_PIN;
    P1OUT |= BUTTON_PIN;

    // LED OUTPUTS
    P2DIR |= (YELLOW_LED_PIN|BLUE_LED_PIN|RED_LED_PIN);
    P2OUT &= ~(YELLOW_LED_PIN|BLUE_LED_PIN|RED_LED_PIN);

    //Interupt
    P1IES |= BIT3;
    P1IE |= BIT3;
    P1IFG &= ~BUTTON_PIN; 
    __enable_interrupt();
}

void SENSOR_TIMER(void) {
    TACTL = TASSEL_2 | MC_2 | TACLR;     // SMCLK, continuous, clear 

}

void SPEAKER_INIT(void) {
    P1DIR |= SPEAKER_PIN;
    P1OUT &= ~SPEAKER_PIN;

    TA1CCTL0 = CCIE;                       // CCR0 interrupt enabled
    TA1CCR0  = 4167;                       // Count value for ~1s toggle @ 12kHz
    TA1CTL   = TASSEL_2 | MC_1 | ID_2;     // SMCLK, up mode, divider /4
}

static inline float GET_DISTANCE() {
    TACTL |= TACLR;
    P1OUT |= TRIGGER_PIN;
    __delay_cycles(10);
    P1OUT &= ~TRIGGER_PIN;

    while (!(P1IN & ECHO_PIN));
    SENSOR_TIMER();

    while (P1IN & ECHO_PIN);
    TACTL &= ~MC_3;

    uint16_t count = TA0R;
    float distance = (count * 0.0343) / 2.0;
    __delay_cycles(100000); 
    return distance; 

}

static inline bool TRIP_WIRE_TEST() {
    float baseline = GET_DISTANCE();        // Initial reference distance
    const float dropThreshold = 5.0;        // Minimum drop to trigger
    const float alpha = 0.1;                // Smoothing factor for baseline

    while (1) {
        float current = GET_DISTANCE();

        if ((baseline - current) >= dropThreshold) {
            return true; // Trip detected
        }

        // Smoothing
        baseline = (1.0 - alpha) * baseline + alpha * current;

        __delay_cycles(50000); 
    }
}

static inline void SET_FREQUENCY(float hertz){
    static const float CLOCK_PERIOD_HZ = 1000000.0 / 4.0;  // SMCLK / 4
    volatile float period = 1.0 / (hertz * 2.0);            // half-period for toggle
    volatile uint16_t count = period * CLOCK_PERIOD_HZ;
    TA1CCR0 = count;
} 

static inline bool BUTTON_PRESSED(){
 
    if(globalButtonFlag == true) {
        globalButtonFlag = false;
        __delay_cycles(75000);
        return true;
    } else {
        return false; 
    }

}

static inline void ALARM_SOUND() {
    SET_FREQUENCY(1000.0);
}

static inline void SPEAKER_OFF() {
    TA1CTL = MC_0;           // Stop Timer1_A
    P1OUT &= ~SPEAKER_PIN;   // Speaker pin low
}

static inline void YELLOW_LED(bool enable) {
    P2OUT = enable ? P2OUT | YELLOW_LED_PIN : P2OUT & ~YELLOW_LED_PIN;
}

static inline void BLUE_LED(bool enable) {
    P2OUT = enable ? P2OUT | BLUE_LED_PIN : P2OUT & ~BLUE_LED_PIN;
}

static inline void RED_LED(bool enable) {
    P2OUT = enable ? P2OUT | RED_LED_PIN : P2OUT & ~RED_LED_PIN;
}

// Interrupt for Timer1_A0
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void){
    P1OUT ^= SPEAKER_PIN;   // Toggle P1.0
}

// Interupt for Button 3
#pragma vector=PORT1_VECTOR
__interrupt void Port1ISR(void){
    globalButtonFlag = true;

    P1IFG &= ~BUTTON_PIN;   //Clear P1.3 IFG

} 
