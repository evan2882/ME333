  #include "NU32.h"          // config bits, constants, funcs for startup and UART
// include other header files here
#include "encoder.h"
#include "isense.h"
#include "currentcontrol.h"
#include "stdio.h"
#include "utility.h"
#include "position_control.h"
#define BUF_SIZE 200



int main()
{
  char buffer[BUF_SIZE];
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;

  setmode(IDLE);
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  encoder_init();
  adc_init();
  current_init();
  position_init();

  __builtin_enable_interrupts();

  _CP0_SET_COUNT(0);       // delay 4 seconds to see the 25% duty cycle on a 'scope
  while(_CP0_GET_COUNT() < 4 * 40000000) {
    ;
  }
  //OC1RS = 1000;

  //LATDbits.LATD2=0;

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a':                     //get adc count
      { sprintf(buffer,"%d %d\r\n", adc_result(), adc_current()); //

        NU32_WriteUART3(buffer);
        break;
      }
      case 'b':                     //get adc count
      {

        sprintf(buffer,"%d\r\n", adc_current()); //
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c':
      { encoder_counts();
        sprintf(buffer,"%d\r\n", encoder_counts());
        NU32_WriteUART3(buffer);
        break;
      }

      case 'd':
      { int n=get_angle();
        sprintf(buffer,"%d\r\n", n); //
      NU32_WriteUART3(buffer);
      break;
      }


      case 'e':
      {
          encoder_reset();
       sprintf(buffer,"%d\r\n", encoder_reset()); //
           NU32_WriteUART3(buffer);
           break;
      }

      case 'f':
      {
        int a;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &a);
        pwm_var=a;

        setmode(PWM);
        break;


        //NU32_ReadUART3(buffer,BUF_SIZE);
        //sscanf(buffer, "%d ", &pwm_var);
        //
        }
        case 'g': //set current gain
        {
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f", &kp, &ki);

        break;


        }
        case 'h': //get current gain
        { sprintf(buffer,"%f %f\r\n", kp, ki); //
        NU32_WriteUART3(buffer);
        break;


        }
        case 'i': //set position gain
        {
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f %f", &kpp, &kpi, &kpd );

        break;


        }
        case 'j': //get position gain
        { sprintf(buffer,"%f %f %f\r\n", kpp, kpi, kpd); //
        NU32_WriteUART3(buffer);
        break;


        }

        case 'k':  //ITEST
        { generateREF();
          setmode(ITEST);
          while (getmode()==ITEST) {
            ;
          }

          sprintf(buffer,"%d\r\n", 100);
          NU32_WriteUART3(buffer);
          int i;

          for ( i=0; i<100; i++){

          sprintf(buffer, "%d %d\r\n", REFarray[i],  ADCa[i]);
          NU32_WriteUART3(buffer);
          }
          break;


        }

      case 'l':{

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d ", &desired_angle);

        setmode(HOLD);
        sprintf(buffer, "%f\r\n", u);
        NU32_WriteUART3(buffer);
        break;


          }

      case 'p':
      { setmode(IDLE);
        sprintf(buffer, "IDLE\r\n");
        NU32_WriteUART3(buffer);
        break;


      }
      case 'y':
      { setmode(TRACK);

        sprintf(buffer, "TRACK\r\n");
        NU32_WriteUART3(buffer);
        break;
      }
      case 'q':
      {
        setmode(IDLE);
        break;
      }
      case 'r':  //get mode
      {

        sprintf(buffer,"%d\r\n", getmode()); //
        NU32_WriteUART3(buffer);
        break;
      }

      case 'm':{ //load step traj

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d ", &refsize);
        int i;

        for ( i=0; i<refsize; i++){
          NU32_ReadUART3(buffer,BUF_SIZE);

          sscanf(buffer, "%d ", &Steparray[i]);}
        // for ( i=0; i<10; i++){
        //     sprintf(buffer, "%d\r\n ", Steparray[i]);
        //     NU32_WriteUART3(buffer);}
            break;
      }
  case 'n':{ //load cubic traj
    NU32_ReadUART3(buffer,BUF_SIZE);
    sscanf(buffer, "%d ", &refsize);
    int i;

    for ( i=0; i<refsize; i++){
      NU32_ReadUART3(buffer,BUF_SIZE);

      sscanf(buffer, "%d ", &Steparray[i]);}
      break;
}


      case 'o':
      {

        setmode(TRACK);
        while (getmode()==TRACK) {
          ;
        }
        // for ( i=0; i<10; i++){
        //     sprintf(buffer, "%d\r\n ", Steparray[i]);
        //     NU32_WriteUART3(buffer);}
        // sprintf(buffer,"%d\r\n", angle_data[5]);
        // sprintf(buffer,"%d\r\n", angle_data[6]);
        // sprintf(buffer,"%d\r\n", angle_data[8]);
        // NU32_WriteUART3(buffer);
        // sprintf(buffer, "%d %d\r\n", Steparray[5],  angle_data[5]);
        __builtin_disable_interrupts();
        sprintf(buffer, "%d\r\n",refsize);
        NU32_WriteUART3(buffer);
        int i;
        for ( i=0; i<refsize; i++){
          sprintf(buffer, "%d %d\r\n", Steparray[i],  angle_data[i]);
          NU32_WriteUART3(buffer);
        }
        __builtin_enable_interrupts();
        break;

        }
        case 'x': // add two integer
        {
            int a =0, b=0;
            NU32_ReadUART3(buffer,BUF_SIZE);
            sscanf(buffer, "%d %d", &a, &b);
            sprintf(buffer,"%d\r\n", a+b);
            NU32_WriteUART3(buffer);
            break;
        }

      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
