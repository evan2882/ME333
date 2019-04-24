#define PLOTPTS 100
volatile int Eint;
volatile float u;
volatile float e;

volatile float kp, ki;
int pwm_var;
volatile int ADCa[PLOTPTS];
volatile int REFarray[PLOTPTS];

void current_init() ;
void generateREF();
