#include "mbed.h"
#include "MMA8452.h"


Accelerometer_MMA8452 acc(p9,p10,100000);

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


void sendCMD(uint8_t cmd[], int size)
{
    for(int i = 0; i < size; i++) {
        com.putc(cmd[i]);
    }
}



void sendTransmitRequest(uint16_t length,uint8_t* data)
{
    uint8_t checksum=0;


    //start
    com.putc(0x7E);
    //bits de poids fort du poid du message
    com.putc((uint8_t)(length >> 8));
    //bits de poids faible du poid du message
    com.putc((uint8_t)(length&0xF));
    //transmission request
    com.putc(0x10);
    com.putc(0x01);
    //broadcast radius
    com.putc(0x00);
    //option
    com.putc(0x00);


//on envois le message en broadcast
    for(int i=0; i<8; i++) {
        com.putc(0x00);
    }
    com.putc(0xFF);
    com.putc(0xFE);

    //envoie du message
    for(int i = 0; i < length; i++) {
        checksum = *(data+i) + checksum;
        com.putc(*(data+i));
    }
    //calcul du checksum
    checksum=0xFF-checksum;
    com.putc(checksum);

}

void sendData()
{
    led1=1;


    //envoi des données de l'accelerometre
    uint8_t data[3]= {0,0,0};
    acc.read_xyz((char*)data,(char*)data+1,(char*)data+2);
    sendTransmitRequest(3,data);



    //envoi de la donnée du bouton
    if(button==1) {
        data[2]=0x01;
        sendTransmitRequest(1,data+3);
    } else {
        data[2]=0x00;
        sendTransmitRequest(1,data+3);
    }


    led1=0;
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

void init()
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
}

int main()
{
    led1 = 1;
    init();
    led1 = 0;
    while(1) {
        sendData();
        wait(0.25);
    }
}
