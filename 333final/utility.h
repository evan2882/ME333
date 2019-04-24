enum mode{IDLE, PWM, ITEST, HOLD, TRACK};
enum mode getmode(void);
void setmode(new_mode);
enum mode current_mode;
