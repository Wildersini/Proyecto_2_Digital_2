#include <pic16f887.h>
#include"OSC.h"
//******************************************************************************
//         oscilador interno setup PG62 
//******************************************************************************
void initOsc(uint8_t IRCF){
switch (IRCF) {
            case 0://OSCILADOR DE 31KhZ
                OSCCONbits.IRCF2=0;
                OSCCONbits.IRCF1=0;
                OSCCONbits.IRCF0=0;
                break;
            case 1://OSCILADOR DE 125KhZ
                OSCCONbits.IRCF2=0;
                OSCCONbits.IRCF1=0;
                OSCCONbits.IRCF0=1;
                break;
            case 2://OSCILADOR DE 250KhZ
                OSCCONbits.IRCF2=0;
                OSCCONbits.IRCF1=1;
                OSCCONbits.IRCF0=0;
                break;
            case 3://OSCILADOR DE 500KhZ
                OSCCONbits.IRCF2=0;
                OSCCONbits.IRCF1=1;
                OSCCONbits.IRCF0=1;
                break;
            case 4://OSCILADOR DE 1MhZ
                OSCCONbits.IRCF2=1;
                OSCCONbits.IRCF1=0;
                OSCCONbits.IRCF0=0;
                break;
            case 5://OSCILADOR DE 2MhZ
                OSCCONbits.IRCF2=1;
                OSCCONbits.IRCF1=0;
                OSCCONbits.IRCF0=1;                
                break;
            case 6://OSCILADOR DE 4MhZ
                OSCCONbits.IRCF2=1;
                OSCCONbits.IRCF1=1;
                OSCCONbits.IRCF0=0;
                break;
            case 7://OSCILADOR DE 8MhZ
                OSCCONbits.IRCF2=1;
                OSCCONbits.IRCF1=1;
                OSCCONbits.IRCF0=1;
                break;
            default://OSCILADOR DE 4MhZ
                OSCCONbits.IRCF2=1;
                OSCCONbits.IRCF1=1;
                OSCCONbits.IRCF0=0;
                break;
        }
OSCCONbits.SCS = 1;
}