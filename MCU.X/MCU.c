// CONFIG1
#pragma config FOSC = EXTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//******************************************************************************
//Libreria 
//******************************************************************************
#include <xc.h>
#include "I2C.h"
#include "OSC.h"
#include "UART.h"
#include <stdio.h>
#define _XTAL_FREQ 4000000

//******************************************************************************
//Variable 
//******************************************************************************
uint8_t T, D, H, M, S, FS, FM, FH, FD, x, w, y, z, a; 
uint8_t D_W, H_I, H_D, M_I, M_D, S_I, S_D;

//******************************************************************************
//Declaración de funciones
//******************************************************************************
void setup (void);          //Configuración
void dia (void);            //Datos de día
void hora (void);           //Datos de hora
void minuto (void);         //Datos de minutos
void segundo (void);        //Datos de segundos
void conversion (void);     //Para la conversion por algunos desfases
void enviar (void);         //Envio de datos para el ESP32

//******************************************************************************
//Interrupciones
//******************************************************************************
void __interrupt () isr (void) {
    if (PIR1bits.RCIF == 1) {
        T = 0;
        T = RCREG;
        if (T == 0x30) {
            PORTCbits.RC1 = 0;} if (T == 0x31) {
            PORTCbits.RC1 = 1;
        } if (T == 0x32) {
            PORTCbits.RC2 = 0; }
        if (T == 0x33) {
            PORTCbits.RC2 = 1;
        } if (T == 0x34) {
            FS = 1;
        } if (T == 0x35) {
            FM = 1;}
        if (T == 0x36) {
            FH = 1; }
        /*if (T == 0x37) {
            FD = 1; }*/
    }
     if (PIR1bits.TXIF == 1){
        if (FS == 1){
            segundo ();
        }
        if (FM == 1){
            minuto ();
        }
        if (FH == 1){
            hora ();
        }
        /*if (FD == 1){
            dia ();
        }*/
        PIE1bits.TXIE = 0;
     }
    if (INTCONbits.TMR0IF == 1) {
        TMR0 = 236;
        INTCONbits.TMR0IF = 0;
        a ++;
        if (a == 10) {
            a = 0;
        }
    }
    // La interrupciones son para el funcionamiento de las leds haciendo una llamada 
    // al mismo cada 10 ms 
}

//******************************************************************************
//Main
//******************************************************************************
void main(void) {
    //UART
    setup ();
    initUSART ();
    PORTCbits.RC1=1;
    PORTCbits.RC2=1;
    PORTCbits.RC1=0;
    PORTCbits.RC2=0;
    I2C_Master_Init(100000); 
    
    while (1) {
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x00);
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        S =  I2C_Master_Read(1);
        M =  I2C_Master_Read(1);
        H =  I2C_Master_Read(1);
        D = I2C_Master_Read(1);
        I2C_Master_Stop();
        conversion ();
    }
    return;
}

//******************************************************************************
//Funciones
//******************************************************************************
void setup (void) {
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    
    ANSEL = 0;
    ANSELH = 0;
    
    //timer
    INTCONbits.GIE = 1; 
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    PIE1bits.RCIE = 1;
    OPTION_REGbits.T0CS = 0; 
    OPTION_REGbits.T0SE = 0; 
    OPTION_REGbits.PSA = 0; 
    //aca se configuro el timer0 para poder utilizarse en el pic con el clk
    
    //pre scaler
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;
    
    
    //oscilador
    initOsc(6);   
}

void conversion (void) {
    D_W = (D & 0b00000111);
    H_D = ((H & 0b00110000)>>4);
    H_I = (H  & 0b00001111);
    M_D = ((M & 0b01110000)>>4);
    M_I = (M  & 0b00001111);
    S_D = ((S & 0b01110000)>>4);
    S_I = (S  & 0b00001111);
}

/*void dia (void) {
    switch (x) {
        case 0:
            TXREG = (D_W + 0x30);
            x ++;
            break;
        case 1:
            TXREG = (0x3A);
            x = 0;
            FD = 0;
            break;    
            
    }
}*/

void hora (void) {
    switch (w) {
        case 0:
            TXREG = (H_D + 0x30);
            w ++;
            break;
        case 1:
            TXREG = (H_I + 0x31);
            w ++;
            break;
        case 2:
            TXREG = (0x3A);
            w = 0;
            FH = 0;
            break;    
    }
}

void minuto (void) {
    switch (y) {
        case 0:
            TXREG = (M_D + 0x30);
            y ++;
            break;
        case 1:
            TXREG = (M_I + 0x31);
            y ++;
            break;
        case 2:
            TXREG = (0x3A);
            y = 0;
            FM = 0;
            break;    
    }
}

void segundo (void) {
    switch (z) {
        case 0:
            TXREG = (S_D + 0x30);        
            z ++;
            break;
        case 1:
            TXREG = (S_I + 0x31);           
            z ++;
            break;
        case 2:
            TXREG = (0x3A);           
            z = 0;
            FS = 0;
            break;    
    }
}