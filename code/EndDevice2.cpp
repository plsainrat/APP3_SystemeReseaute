#include "mbed.h"

DigitalOut myled(LED1);
DigitalOut pin8(p8);

DigitalOut led1(LED1);
DigitalOut led2(LED2);

DigitalIn button(p30);

Serial pc(USBTX,USBRX);

Serial com(p13,p14);

uint8_t ID_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x49, 0x44, 0x11, 0x58};
uint8_t SC_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x53, 0x43, 0x13, 0x4D};
uint8_t WR_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x57, 0x52, 0x4D};
uint8_t AC_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x41, 0x43, 0x72};

uint8_t TransmitRequest[22] = {0x7E, 0x00, 0x12, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x01, 0x33, 0x32, 0x31, 0x5A};

void sendCMD(uint8_t cmd[], int size){
    for(int i = 0; i < size; i++) {
        com.putc(cmd[i]);
    }
}

void sendTransmitRequest()
{
    for(int i = 0; i < 22; i++) {
        com.putc(TransmitRequest[i]);
    }
}

void readTransmitStatus()
{
    uint16_t msbLength = com.getc() << 8;
    uint16_t lsbLength = com.getc();
    uint16_t length = msbLength + lsbLength;
    if(length != 7) {
        return;
    }
    //pc.printf("length%x\n", length);
    uint8_t frameType = com.getc();
    uint8_t frameID = com.getc();
    uint8_t netAdr[2];
    netAdr[0] = com.getc();
    netAdr[1] = com.getc();
    uint8_t transmitRetryCount = com.getc();
    uint8_t delivery_Status = com.getc();
    uint8_t discovery_Status = com.getc();

    uint8_t checkSum = com.getc();
    uint8_t calc_checkSum = 0xFF - frameType - frameID - netAdr[0] - netAdr[1] - transmitRetryCount - delivery_Status - discovery_Status;
    if(calc_checkSum != checkSum) {
        pc.printf("ERROR");
    } else {
        pc.printf("Good CheckSum");
    }
    pc.printf("\n");
}

int main()
{
    pin8=0;
    wait(0.4);
    pin8=1;
    wait(1);
    com.printf("+++");
    wait(1);
    sendCMD(ID_cmd,9);
    sendCMD(SC_cmd,9);
    sendCMD(WR_cmd,8);
    sendCMD(AC_cmd,8);
    led1 = 1;
    while(1) {
        if(button) {
            led2 = 1;
            sendTransmitRequest();
        }
        if(com.readable()==1) {
            char temp = (char)com.getc();
            if(temp == 0x7E) {
                readTransmitStatus();
                led2 = 0;
            }
        }
        wait(0.05);
    }
}