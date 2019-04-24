volatile float kpp, kpi, kpd;

volatile int Eint_p;
volatile float u_p;
volatile float e_p;
volatile float eprev_p;
volatile float Edot;
int refsize;
int Steparray[2000];
int angle_data[2000];

int desired_angle;


void position_init(void);
int get_angle(void);
