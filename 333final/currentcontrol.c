
#include "NU32.h"
#include "currentcontrol.h"
#include "utility.h"
#include "stdio.h"
#include "position_control.h"

volatile int Eint=0;
volatile float u=0;
volatile float e=0;





void current_init(void) {
// interrupt
  PR2 = 3999;  //5k hz 5000/8
    TMR2 = 0;
    T2CONbits.TCKPS = 2;            //             set prescaler to 4
    T2CONbits.TGATE = 0;
    T2CONbits.ON = 1;
    IPC2bits.T2IP = 5;
    IPC2bits.T2IS = 1;
    IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;


//PWM
  T3CONbits.TCKPS = 0b000;     // Timer3 prescaler N=1
  OC1CONbits.OCTSEL =1;
  PR3 = 3999;              // period = (PR2+1) * N * 12.5 ns = 20khz
  TMR3= 0;                // initial TMR2 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 1000;             // duty cycle = OC1RS/(PR2+1) = 25%
  OC1R = 1000;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3

  OC1CONbits.ON = 1;       // turn on OC1

TRISD=0xf;


}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {

  static int counter = 0;
  static  int desired_current=0;
  static int plotind=0;
  static float adcval=0;


  ki=0.5;
  kp=0.1;


  switch(current_mode){
    case IDLE:

          OC1RS=0;
          break;

    case PWM:
      OC1RS= abs(pwm_var)*(4000)/100;
      if (pwm_var>0)
        LATDbits.LATD4=0;
        else
        LATDbits.LATD4=1;
          break;

    case ITEST:
      adcval=adc_current();
      desired_current=REFarray[counter];
      e= desired_current-adcval;
      Eint=Eint+e;
      u=kp*e +ki*Eint;
      if (u>0)
        LATDbits.LATD4=0;
        else
        LATDbits.LATD4=1;


      if(u>100){
        u = 100;
      }
    else if (u<-100.0){
      u = -100;
    }
    OC1RS = (unsigned int)((u/100.0)*PR3);
      if(counter<PLOTPTS){

        ADCa[counter] = adcval;
        counter++;
      }
      else{
        counter =0;
        Eint=0;
        setmode(IDLE);
      }
      break;

    case HOLD:

      adcval=adc_current();
      desired_current=u_p;
      e= desired_current-adcval;
      Eint=Eint+e;
      u=kp*e +ki*Eint;

      if (u>0)
        LATDbits.LATD4=0;
        else
        LATDbits.LATD4=1;


      if(u>100){
        u = 100;

      }
      else if (u<-100.0){

        u= -100;


        }
      OC1RS = (unsigned int)((u/100.0)*PR3);

      break;
    case TRACK:
      adcval=adc_current();
      desired_current=u_p;
      e= desired_current-adcval;
      Eint=Eint+e;
      u=kp*e +ki*Eint;

      if (u>0)
        LATDbits.LATD4=0;
        else
        LATDbits.LATD4=1;


      if(u>100){
        u = 100;

      }
      else if (u<-100.0){

        u= -100;


        }
      OC1RS = (unsigned int)((u/100.0)*PR3);

      break;

  }

  IFS0bits.T2IF = 0; //clear flag
  }

  void generateREF(){
    int i;
    char buffer[200];
    for ( i=0; i<=24; i++){
      REFarray[i]=200;
    }
    for ( i=25; i<=49; i++){
      REFarray[i]=-200;
    }
    for ( i=50; i<=74; i++){
      REFarray[i]=200;
    }
    for ( i=75; i<=99; i++){
      REFarray[i]=-200;
    }


  }
