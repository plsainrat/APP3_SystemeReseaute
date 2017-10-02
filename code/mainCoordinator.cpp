#include "mbed.h"

DigitalOut myled(LED1);
DigitalOut pin8(p8);

DigitalOut led1(LED1);

Serial pc(USBTX,USBRX);

Serial com(p13,p14);

uint8_t ID_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x49, 0x44, 0x11, 0x58};
uint8_t SC_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x53, 0x43, 0x13, 0x4D};
uint8_t NJ_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x4E, 0x4A, 0xFF, 0x5F};
uint8_t WR_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x57, 0x52, 0x4D};
uint8_t AC_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x41, 0x43, 0x72};

uint8_t valueCharArray[128];


void Reset(){
  pin8=0;
  wait(0.4);
  pin8=1;
  wait(1);
}

void Init(){
com.printf("+++");
wait(2);
for(int i = 0; i<9; i++) {
    com.putc(ID_cmd[i]);
}
wait(1);

for(int i = 0; i<9; i++) {
    com.putc(SC_cmd[i]);
}
wait(1);

for(int i = 0; i<9; i++) {
    com.putc(NJ_cmd[i]);
}
wait(1);

for(int i = 0; i<8; i++) {
    com.putc(WR_cmd[i]);
}
wait(1);
for(int i = 0; i<8; i++) {
    com.putc(AC_cmd[i]);
}
}

int main()
{
  Reset();
  Init();
  wait(1);
    led1 = 1;
    int counter = 0;
    while(1) {
        if(com.readable()==1) {
            valueCharArray[counter] = (char)com.getc();

            pc.putc((int)valueCharArray[counter]);
            pc.printf(".%d\n",counter);
            counter++;
        }

    }
}
