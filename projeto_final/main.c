#include <proc/pic18f4520.h>
#include "config.h"
#include "lcd.h"
#include "adc.h"
#include "rgb.h"
#include "serial.h"
#include "i2c.h"
#include "io.h"

void main(void) {
    unsigned char umidade, limiar;
    char caso;
    
    rgbInit();
    lcdInit();
    adcInit();
    
    serial_init();
    
    TRISCbits.TRISC7 = 1; //RX
    TRISCbits.TRISC6 = 0; //TX
    
    //serial_tx_str("umidade: "); // lê a umidade
    umidade = 0xa5;
    while(umidade == 0xa5){
        umidade = serial_rx(2000);
    }
    
    serial_tx(umidade);
    
    //umidade = 50;
    
    caso = 0;
    for (;;) {
        switch(caso){
        case 0:
            limiar = (adcRead(0)/(1023.0))*100;
            lcdCommand(0xC0);
            lcdCommand(0x80);
            lcdString("Limiar: ");
            lcdChar((limiar/100)%10 + '0'); //imprime as centenas
            lcdChar((limiar/10)%10 + '0'); //imprime as dezenas
            lcdChar(limiar%10 + '0'); //imprime os unitarios
            lcdCommand(0xC0);
            lcdString("Umidade: ");
            lcdChar((umidade/100)%10 + '0'); //imprime as centenas
            lcdChar((umidade/10)%10 + '0'); //imprime as dezenas
            lcdChar(umidade%10 + '0'); //imprime os unitarios
            caso = 1;
            break;
        case 1:
            TRISD = 0x00;
            if (umidade>(limiar-10) && umidade<(limiar+10)){
                if(umidade<limiar-5 || umidade>limiar+5){
                    digitalWrite(LED_BLUE_PIN, LOW); 
                    digitalWrite(LED_RED_PIN, HIGH); //liga amarelo
                    digitalWrite(LED_GREEN_PIN, HIGH);
                }
                else{
                    digitalWrite(LED_GREEN_PIN, HIGH); //liga verde
                    digitalWrite(LED_BLUE_PIN, LOW);
                    digitalWrite(LED_RED_PIN, LOW);
                }
            }
            else{
                digitalWrite(LED_BLUE_PIN, LOW);
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_RED_PIN, HIGH); //vermeio

            }
            caso = 2;
            break;
        case 2:
            umidade = 0xa5;
            while(umidade == 0xa5){
                umidade = serial_rx(2000);
            }
            serial_tx(umidade);
            caso = 0;
            break;
        default:
                caso = 0;
                break;
        }
    }
}