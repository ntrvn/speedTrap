char checkInput(char bit);
void  init_timer1(unsigned short m);
void play_note(unsigned short freq);
void variable_delay_us(int delay);
void serial_txchar(char ch);


volatile int second, speed,count, bufferCount;
volatile unsigned char x,y,i;
volatile unsigned char a, b, c, d;
volatile unsigned char newState, oldState;
volatile unsigned char flag;
volatile unsigned char encoderOldState, encoderNewState;
volatile unsigned char encoderFlag, serialFlag;
volatile char serialBuffer[5];

