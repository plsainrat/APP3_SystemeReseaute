#include "mbed.h"
#include "MMA8452.h"


/*------------------------------------------------------------------------------
                            Configuration des I/O
-------------------------------------------------------------------------------*/


DigitalOut myled(LED1);
DigitalOut pin8(p8);

Accelerometer_MMA8452 acc(p9,p10,100000);

DigitalIn button(p15);

DigitalOut led1(LED1);
DigitalOut led2(LED2);

Serial pc(USBTX,USBRX);

Serial com(p13,p14);

/*------------------------------------------------------------------------------
                            FRAME API
-------------------------------------------------------------------------------*/

uint8_t ID_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x49, 0x44, 0x11, 0x58};
uint8_t SC_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x53, 0x43, 0x13, 0x4D};
uint8_t NJ_cmd[9] = {0x7E, 0x00, 0x05, 0x08, 0x01, 0x4E, 0x4A, 0xFF, 0x5F};
uint8_t WR_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x57, 0x52, 0x4D};
uint8_t AC_cmd[8] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x41, 0x43, 0x72};



/*------------------------------------------------------------------------------
                        fonctions de commandes API
-------------------------------------------------------------------------------*/


void sendTransmitRequest(uint16_t length,uint8_t* data)
{
    uint8_t checksum=0;

    //entrée dans le mode commande
    com.printf("+++");

    wait(1);
    //start
    com.putc(0x7E);
    //bits de poids fort du poid du message
    com.putc((uint8_t)(length >> 8));
    //bits de poids faible du poid du message
    com.putc((uint8_t)(length&0xF));
    //transmission request
    com.putc(0x10);
    com.putc(0x10);
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

    //sortie du mode Commande
    com.printf("ACTN\r");
}








void Reset()
{

    pin8=0;
    wait(0.4);
    pin8=1;
    wait(1);

}






void Init()
{

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
    for(int i = 0; i<8; i++) {
        com.putc(WR_cmd[i]);
    }
    wait(1);

    for(int i = 0; i<8; i++) {
        com.putc(AC_cmd[i]);
    }

    com.printf("ACTN\r");

}


/*------------------------------------------------------------------------------
                            routine du ticker
-------------------------------------------------------------------------------*/


void sendData()
{
    led1=1;


    //envoi des données de l'accelerometre
    uint8_t data[3]= {0,0,0};
    acc.read_xyz((char*)data,(char*)data+1,(char*)data+2);
    sendTransmitRequest((uint16_t)3,data);



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

/*------------------------------------------------------------------------------
                            Point d'entrée du programme
-------------------------------------------------------------------------------*/



int main()
{
    Ticker tick;
    Reset();
    Init();
    tick.attach(&sendData,1);
    while(1) {

    }
}
