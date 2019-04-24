#include "isense.h"
#include "NU32.h"

void adc_init(void) {  // initialization for B0 (anlag0)
  AD1PCFGbits.PCFG0 = 0;
    AD1CON3bits.ADCS = 2;
    AD1CON1bits.ADON = 1;
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

unsigned int adc_result(){    //get average result
  int a = adc_sample_convert(0);
  int b = adc_sample_convert(0);
  int c = adc_sample_convert(0);
  int d= (a+b+c)/3;
  return d;
}

int adc_current(){
  int e = adc_result();
  int f;
  f= (1.35*e)-687;
  return f;
}
