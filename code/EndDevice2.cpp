#include "mbed.h"
#include "MMA8452.h"


Accelerometer_MMA8452 acc(p28,p27,100000);

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
uint8_t asdasda[20] = {0x7e, 0x00, 0x10, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00,0x00 ,0x00 ,0x00 ,0x00 ,0xff ,0xfe ,0x00 ,0x00 ,0x01 ,0x00 ,0xf0};


void sendCMD(uint8_t cmd[], int size)
{
    for(int i = 0; i < size; i++) {
        com.putc(cmd[i]);
    }
}

void sendTransmitRequest()
{
    for(int i = 0; i < 22; i++) {
        com.putc(TransmitRequest[i]);
        pc.printf("%x ",TransmitRequest[i]);
    }
}

void sendTransmitRequest(uint16_t length,uint8_t* data)
{

    uint8_t startBit = 0x7E;
    //start
    com.putc(startBit);
    //bits de poids fort du poid du message
    uint8_t lengthMSB = ((length + 14) >> 8);
    com.putc(lengthMSB);
    //bits de poids faible du poid du message
    uint8_t lengthLSB = ((length + 14)&0xFF);
    com.putc(lengthLSB);
    //transmission request
    uint8_t frameType = 0x10;
    uint8_t frameID = 0x01;

    com.putc(frameType);
    com.putc(frameID);

    //on envois le message en broadcast
    for(int i=0; i<8; i++) {
        com.putc(0x00);
    }
    uint8_t macadr[2];
    macadr[0] = 0xFF;
    macadr[1] = 0xFE;
    com.putc(macadr[0]);
    com.putc(macadr[1]);


    //broadcast radius
    uint8_t radius = 0x00;
    uint8_t options = 0x00;
    com.putc(radius);
    //option
    com.putc(options);
    uint8_t checksum = 0xFF - macadr[0]  - macadr[1] - frameType - frameID;
    //envoie du message
    for(int i = 0; i < length; i++) {
        checksum -= *(data+i);
        com.putc(*(data+i));
    }
    //calcul du checksum
    com.putc(checksum);
}

void sendData()
{

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
        led1=1;


        //envoi des données de l'accelerometre
        uint8_t dataAcc[4]= {0x02,0,0,0};
        uint8_t dataBut[2]= {0x01,0};
        acc.read_xyz((char*)dataAcc+1,(char*)dataAcc+2,(char*)dataAcc+3);
        sendTransmitRequest(4,dataAcc);

        //envoi de la donnée du bouton
        if(button) {
            //sendTransmitRequest();
            sendTransmitRequest(2,dataBut);
            //sendCMD(asdasda,20);
        }
        if(com.readable()==1) {
            char temp = (char)com.getc();
            if(temp == 0x7E) {
                readTransmitStatus();
                led2 = 0;
            }
        }
        led1=0;
        wait(0.5);
    }
}
