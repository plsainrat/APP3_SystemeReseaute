#include "mbed.h"

DigitalOut myled(LED1);
DigitalOut pin8(p8);

DigitalOut led1(LED1);
DigitalOut led2(LED2);

Serial pc(USBTX,USBRX);

Serial com(p13,p14);

uint8_t ID_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x49, 0x44, 0x11, 0x58};
uint8_t SC_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x53, 0x43, 0x13, 0x4D};
uint8_t WR_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x57, 0x52, 0x4D};
uint8_t AC_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x41, 0x43, 0x72};

uint8_t TransmitRequest[21] = {0x7E, 0x00, 0x11, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x31, 0x32, 0x33, 0x5B};

void sendTransmitRequest(){
    for(int i = 0; i < 21; i++){
        com.putc(TransmitRequest[i]);   
    }    
}


int main()
{
    pin8=0;
    wait(0.4);
    pin8=1;
    wait(1);
    com.printf("+++");
    wait(2);
    for(int i = 0;i<9;i++) {
        com.putc(ID_cmd[i]);
    }
    wait(1);
    
    for(int i = 0;i<9;i++) {
        com.putc(SC_cmd[i]);
    }
    wait(1);
    for(int i = 0;i<8;i++) {
        com.putc(WR_cmd[i]);
    }
    wait(1);
    
    for(int i = 0;i<8;i++) {
        com.putc(AC_cmd[i]);
    }
    led1 = 1;
    wait(2);
    sendTransmitRequest();
    led2 = 1;
    pc.putc(com.getc());
}