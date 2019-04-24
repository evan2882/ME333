#include "NU32.h"          // constants, functions for startup and UART
#define NUMSAMPS 1000    //number of points in waveform
static volatile int Waveform[NUMSAMPS];   //waveform

void makeWaveform();
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller();

int main(void) {
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init
    __builtin_disable_interrupts();
    makeWaveform();

    PR2 = 19999;
    TMR2 = 0;
    T2CONbits.TCKPS = 2;            //             set prescaler to 4
    T2CONbits.TGATE = 0;
   
    T2CONbits.ON = 1;
    IPC2bits.T2IP = 5;
    IPC2bits.T2IS = 1;
    IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;              // INT step 6: enable interrupt
    
    AD1PCFGbits.PCFG0 = 0;
    AD1CON3bits.ADCS = 2;
    AD1CON1bits.ADON = 1;
    
    
    T3CONbits.TCKPS = 0b000;     // Timer3 prescaler N=1
    OC1CONbits.OCTSEL =1;
    PR3 = 3999;              // period = (PR2+1) * N * 12.5 ns = 20khz
    TMR3= 0;                // initial TMR2 count is 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 3000;             // duty cycle = OC1RS/(PR2+1) = 75%
    OC1R = 3000;              // initialize before turning OC1 on; afterward it is read-only
    T3CONbits.ON = 1;        // turn on Timer3
    
    OC1CONbits.ON = 1;       // turn on OC1
    
    
    
    
    
  _CP0_SET_COUNT(0);       // delay 4 seconds to see the 25% duty cycle on a 'scope
  while(_CP0_GET_COUNT() < 4 * 40000000) {
    ;
  }
  OC1RS = 3000;            // set duty cycle to 75%
    
__builtin_enable_interrupts();
  while(1) {
    ;                      // infinite loop
  }
  return 0;
}

void makeWaveform(){
    int i=0, center= 2000, A= 2000;
    for(i =0; i<NUMSAMPS; ++i){
        if  (i<NUMSAMPS/2){
            Waveform[i]=center+A;
        }
        else{
            Waveform[i]=center-A;
        }
    }
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {  // INT step 1: the ISR
    
    static int counter = 0;
    OC1RS = Waveform[counter];
    counter++;
    
    if(counter == NUMSAMPS){
        counter = 0;
    }
    IFS0bits.T2IF = 0;
}

