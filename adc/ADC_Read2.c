#include "NU32.h"          // constants, functions for startup and UART

#define VOLTS_PER_COUNT (3.3/1024)
#define CORE_TICK_TIME 25    // nanoseconds between core ticks
#define SAMPLE_TIME 10       // 10 core timer ticks = 250 ns

unsigned int sample0 = 0, elapsed = 0;
char msg[100] = {};

unsigned int adc_sample_convert(int pin) { // sample & convert the value on the given 
                                           // adc pin the pin should be configured as an 
                                           // analog input in AD1PCFG
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) { 
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) Timer1ISR(void) {  // INT step 1: the ISR
    
    sample0 = adc_sample_convert(0);    // sample and convert pin 0
    
    
    sprintf(msg,"pin B0: %4u (%5.3f volts) \r\n", sample0, sample0 * VOLTS_PER_COUNT);
    NU32_WriteUART3(msg);
    
    IFS0bits.T1IF = 0;              // clear interrupt flag
}

int main(void) {


  NU32_Startup();                 // cache on, interrupts on, LED/button init, UART init
    
    __builtin_disable_interrupts(); // INT step 2: disable interrupts at CPU
    // INT step 3: setup peripheral
    PR1 = 31249;                    //             set period register
    TMR1 = 0;                       //             initialize count to 0
    T1CONbits.TCKPS = 3;            //             set prescaler to 256
    T1CONbits.TGATE = 0;            //             not gated input (the default)
    T1CONbits.TCS = 0;              //             PCBLK input (the default)
    T1CONbits.ON = 1;               //             turn on Timer1
    IPC1bits.T1IP = 5;              // INT step 4: priority
    IPC1bits.T1IS = 0;              //             subpriority
    IFS0bits.T1IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T1IE = 1;              // INT step 6: enable interrupt
    AD1PCFGbits.PCFG0 = 0;                 // AN0 is an adc pin
    
    AD1CON3bits.ADCS = 2;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
    //                           2*3*12.5ns = 75ns
    AD1CON1bits.ADON = 1;      
    __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU
               // turn on A/D converter

  while (1) {
    
  }
  return 0;
}
