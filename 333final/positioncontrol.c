#include "position_control.h"
#include "NU32.h"
#include "stdio.h"
#include "utility.h"

volatile int Eint_p=0;
volatile float u_p;
volatile float e_p=0;
volatile float eprev_p=0;
volatile float Edot=0;
int pcount=0;

void position_init(void) {

  PR4 = 49999;  //200 hz  5*40
    TMR4 = 0;                //(PR2+1) * N * 12.5 ns = 20khz
    T4CONbits.TCKPS = 3;            //             set prescaler to 8
    T4CONbits.TGATE = 0;
    T4CONbits.ON = 1;


    IPC4bits.T4IP = 5;
    IPC4bits.T4IS = 1;
    IFS0bits.T4IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T4IE = 1;
  }

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) PController(void) {
  //LATDbits.LATD5=!LATDbits.LATD5;

  int ref;
  int angle_now;

  kpp=20;
  kpi=0.005;
  kpd=400;

  switch(current_mode){
    case HOLD:
      

      angle_now=get_angle();
      e_p=desired_angle-angle_now;
      Eint_p=Eint_p+e_p;
      Edot = e_p-eprev_p;
      u_p= kpp*e_p+kpi*Eint_p+kpd*Edot;
      eprev_p=e_p;
      if(u_p>=100){
        u_p = 100;
      }
      else if (u_p<=-100){
      u_p = -100;
      }
      break;
    case TRACK:
          desired_angle=Steparray[pcount];
          angle_now=get_angle();
          angle_data[pcount]=angle_now;
          e_p=desired_angle-angle_now;
          Eint_p=Eint_p+e_p;
          Edot = e_p-eprev_p;
          u_p= kpp*e_p+kpi*Eint_p+kpd*Edot;
          eprev_p=e_p;
          if(u_p>=100){
            u_p = 100;
          }
          else if (u_p<=-100){
          u_p = -100;
          }
          char msg[100];
          pcount++;

          if (pcount>refsize){
            u_p = Steparray[refsize-1];
            setmode(HOLD);
          }

    }



  IFS0bits.T4IF = 0;
}

int get_angle(void) {
  encoder_counts();
  int n= encoder_counts();
  n=    (n-32768)*360/448/4;
  return n;
}
