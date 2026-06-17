/*
 * Name:         Miguel Diaz-Alvarez
 * Date:         8/4/2025
 * Assignment:   Lab 5
 * YouTube:      https://youtu.be/2aKTIeYFAE4        
*/

#include <stdint.h>
#include <msp430.h>
#include <stdbool.h>

//Macros 
#define BUTTON_PIN          BIT3

//Function Prototypes
void UART_INIT();
void ADC_INIT();
void BUTTON_INIT();
static inline bool BUTTON_PRESSED();
static inline float READ_VOLTAGE();
static inline uint8_t UART_VAL();
static inline void FIRE();
static inline void MOVE();

bool globalButtonFlag = false;

//Main
int main() {

    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer
    UART_INIT();
    ADC_INIT();
    BUTTON_INIT();

    while(1){
        
        FIRE();
        MOVE();                     

    }
}

//Initialization Functions
void BUTTON_INIT() {
    
    //Button Input 
    P1DIR &= ~BUTTON_PIN;
    P1REN |= BUTTON_PIN;
    P1OUT |= BUTTON_PIN;
    P1IES |= BIT3;
    P1IE |= BIT3;
    P1IFG &= ~BUTTON_PIN;

}

void ADC_INIT() {
    ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON;
    ADC10CTL1 = INCH_4 | ADC10DIV_3;
}

void UART_INIT() {

    P1SEL = BIT1 | BIT2;                    // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 | BIT2;
    UCA0CTL1 |= UCSSEL_2;                   // SMCLK
    UCA0BR0 = 104;                          // 1MHz 9600
    UCA0BR1 = 0;                            // 1MHz 9600
    UCA0MCTL = UCBRS2 | UCBRS1;             // Modulation UCBRSx = 6
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                        // Enable USCI_A0 RX interrupt

    __enable_interrupt();

} 

// Other Functions 
static inline void MOVE(){
    UCA0TXBUF = UART_VAL();
}

static inline void FIRE(){
    if (BUTTON_PRESSED()) {
        UCA0TXBUF = 255;
    }
}

static inline uint8_t UART_VAL() {
    volatile float volts = READ_VOLTAGE();
    volatile uint8_t numVal = volts * 77;
    
    return numVal;
}

static inline float READ_VOLTAGE() {
    ADC10CTL0 |= ENC | ADC10SC;         // Sampling and conversion start 
    while(ADC10CTL1 & ADC10BUSY);

    uint16_t ADCVal = ADC10MEM;
    volatile float Voltage = (3.3/1024.0) * ((float)ADCVal);          
    return Voltage;
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

// Interrups
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
    UCA0TXBUF = UCA0RXBUF;                  // TX -> RXed character
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1ISR(void){
    globalButtonFlag = true;

    P1IFG &= ~BUTTON_PIN;   //Clear P1.3 IFG

} 
