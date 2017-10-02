#include "mbed.h"
#include "rtos.h"

DigitalOut myled(LED1);
DigitalOut pin8(p8);

DigitalOut led1(LED1);
DigitalOut led2(LED2);

Serial pc(USBTX,USBRX);

Serial com(p13,p14);

uint8_t ID_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x49, 0x44, 0x11, 0x58};
uint8_t SC_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x53, 0x43, 0x13, 0x4D};
uint8_t NJ_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x4E, 0x4A, 0xFF, 0x5F};
uint8_t WR_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x57, 0x52, 0x4D};
uint8_t AC_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x41, 0x43, 0x72};
uint8_t ND_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x4E, 0x44, 0x64};

uint8_t LED_OFF[20] = {0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x6A, 0xEA, 0x02, 0x44, 0x31, 0x04, 0x1A};
uint8_t LED_ON[20] = {0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x6A, 0xEA, 0x02, 0x44, 0x31, 0x05, 0x19};

uint8_t nodeADR[2];

bool toggle;

void sendCMD(uint8_t cmd[], int size)
{
    for(int i = 0; i < size; i++) {
        com.putc(cmd[i]);
    }
}

void readReceivePacket(uint8_t frameType, uint16_t length)
{
    uint8_t macAdr[8];
    for(int i = 0; i<8; i++) {
        macAdr[i] = com.getc();
    }
    uint8_t netAdr[2];
    netAdr[0] = com.getc();
    netAdr[1] = com.getc();
    uint8_t option = com.getc();
    uint8_t data_Type = com.getc();
    if(data_Type == 0x01) {
        pc.printf("Button: ");
    } else if(data_Type == 0x02) {
        pc.printf("Accel: ");
    }
    uint8_t data_RF[length-13];
    for(int i = 0; i < length - 13; i++) {
        data_RF[i] = com.getc();
        pc.putc(data_RF[i]);
    }
    uint8_t checkSum = com.getc();
    uint8_t calc_checkSum = 0xFF - frameType - netAdr[0] - netAdr[1] - data_Type - option;
    for(int i = 0; i < length - 13; i++) {
        calc_checkSum -= data_RF[i];
    }
    for(int i = 0; i < 8; i++) {
        calc_checkSum -= macAdr[i];
    }
    if(calc_checkSum != checkSum) {
        pc.printf("ERROR");
    } else {
        pc.printf("Good CheckSum");
    }
}

void readAtCmdResponse(uint8_t frameType, uint16_t length)
{
    uint8_t frameID = com.getc();
    uint8_t ATCmd[2];
    ATCmd[0] = com.getc();
    ATCmd[1] = com.getc();
    uint8_t cmdStatus = com.getc();

    uint8_t cmd_Data[length-5];
    for(int i = 0; i < length - 5; i++) {
        cmd_Data[i] = com.getc();
        pc.printf("%x",cmd_Data[i]);
    }
    uint8_t checkSum = com.getc();
    uint8_t calc_checkSum = 0xFF - frameType - ATCmd[0] - ATCmd[1] - frameID - cmdStatus;
    for(int i = 0; i < length - 5; i++) {
        calc_checkSum -= cmd_Data[i];
    }
    if(calc_checkSum != checkSum) {
        pc.printf("ERROR ");
        pc.printf("%x,%x", calc_checkSum, checkSum);
    } else {
        pc.printf(" Good CheckSum");
    }
    if(ATCmd[0] == 0x4e && ATCmd[1] == 0x44) {
        nodeADR[0] = cmd_Data[0];
        nodeADR[1] = cmd_Data[1];
    }
    pc.printf("MAC: %x %x", nodeADR[0], nodeADR[1]);
}

void readFrame()
{
    uint16_t msbLength = com.getc() << 8;
    uint16_t lsbLength = com.getc();
    uint16_t length = msbLength + lsbLength;
    uint8_t frameType = com.getc();
    if(frameType == 0x90 ) {
        readReceivePacket(frameType, length);
    } else if(frameType == 0x88) {
        readAtCmdResponse(frameType, length);
    } else {
        return;
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
    sendCMD(NJ_cmd,9);
    sendCMD(WR_cmd,8);
    sendCMD(AC_cmd,8);
}

void broadCast()
{
    sendCMD(ND_cmd, 8);
    
}

void toggleLed()
{
        toggle = !toggle;
}

void readCom()
{

}

int main()
{
    bool Toggle_Led;
    init();
    broadCast();
    Ticker ticker;
    //ticker.attach(&toggleLed, 1);
    while(1) {
        if(com.readable()) {          
            char temp = (char)com.getc();
            if(temp == 0x7E) {
                readFrame();
                led1 = !led1;
                //pc.printf("\n");
            }
            //pc.printf("%x",temp);
        }
        //if(toggle){
//            toggle = !toggle;
//            if(Toggle_Led){
//                sendCMD(LED_ON,20);
//            }
//            else{
//                sendCMD(LED_OFF,20);    
//            }
//        }
        

    }
}