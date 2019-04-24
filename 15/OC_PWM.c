#include "NU32.h"          // constants, functions for startup and UART
#include "stdio.h"
#define NUMSAMPS 1000    //number of points in waveform
#define PLOTPTS 200
#define DECIMATION 10
#define CORE_TICK_TIME 25    // nanoseconds between core ticks
#define SAMPLE_TIME 10       // 10 core timer ticks = 250 ns

static volatile int Waveform[NUMSAMPS];   //waveform
static volatile int ADCarray[PLOTPTS];
static volatile int REFarray[PLOTPTS];
static volatile int StoringData = 0; //flag
static volatile int Eint;
static volatile float u;
static volatile float e;

static volatile float kp=0, ki=0;

void makeWaveform();
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller();
unsigned int adc_sample_convert(int pin);

int main(void) {
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init

    char message[100];
    float kptemp =0, kitemp=0;    //temporary local gains
    int ploti=0;
    makeWaveform();
    __builtin_disable_interrupts();

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
    __builtin_enable_interrupts();




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


  while(1) {
    NU32_ReadUART3(message, sizeof(message));
      sscanf(message, "%f %f", &kptemp, &kitemp);

      __builtin_disable_interrupts();
      kp=kptemp;
      ki=kitemp;
      __builtin_enable_interrupts();
      StoringData=1;
      Eint = 0;
      while (StoringData) {
          ;
      }
      for (ploti=0; ploti<PLOTPTS; ploti++){
          sprintf(message, "%d %d %d\r\n", PLOTPTS-ploti, ADCarray[ploti], REFarray[ploti]);
          NU32_WriteUART3(message);
      }
    }

  return 0;
}

void makeWaveform(){
    int i=0, center= 500, A= 300;
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
    static int plotind = 0;
    static int decctr=0;
    static int adcval = 0;

    adcval = adc_sample_convert(0);

    e=Waveform[counter] - adcval;
    Eint = Eint +e;
    u=kp*e +ki*Eint;


    float unew=u+50.0;
    if(unew>100){
      unew = 100;
    } else if (unew<0.0){
      unew = 0.0;
    }

    OC1RS = (unsigned int)((unew/100.0)*PR3);

    if(StoringData){
      decctr++;
      if (decctr == DECIMATION) {
        decctr = 0;
        ADCarray[plotind] = adcval;
        REFarray[plotind] = Waveform[counter];
        plotind++;
      }
      if (plotind == PLOTPTS) {
        plotind = 0;
        StoringData = 0;
      }
    }
    counter++;
    if(counter == NUMSAMPS){
        counter = 0;
    }

    IFS0bits.T2IF = 0;
}

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
