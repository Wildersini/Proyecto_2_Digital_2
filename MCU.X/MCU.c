#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>
#include "UART.h"
#include "OSC.h"
#include "I2C.h"
#include <stdio.h>
#define _XTAL_FREQ 4000000

//**************************
// Prototipos de funciones
//**************************
void Setup       (void);
void send_hora   (void);
void send_min    (void);
void send_seg    (void);
void CONVET      (void);
void first_send  (void);

//**************************
// Variables
//**************************
uint8_t L,Z,z,r,C,q,h,m,s,M,b,g;
uint8_t temp;
uint8_t mou,day,hor,min,seg,week,year;
uint8_t mou_u,day_u,hor_u,min_u,seg_u ;
uint8_t mou_t,day_t,hor_t,min_t,seg_t ;
uint8_t segundos ;
//**************************
//                            interuption 
//**************************
void __interrupt ( ) isr(void){ 
    if (PIR1bits.RCIF==1){ 
        temp = 0;
        temp = RCREG;
        if (temp == 0X30){PORTAbits.RA0=0;} //turn off led1
        if (temp == 0X31){PORTAbits.RA0=1;} //turn on led1
        if (temp == 0X32){PORTAbits.RA1=0;} //turn off led2
        if (temp == 0X33){PORTAbits.RA1=1;} //turn on led2
        if (temp == 0X34){C=1;} //seg
        if (temp == 0X35){b=1;} //min
        if (temp == 0X36){g=1;} //hora
    }
    if (INTCONbits.TMR0IF==1){//este if esta revisando la bandera del timer0
        TMR0=236;             //le cargamos un valor al timer0
        INTCONbits.TMR0IF=0;  //reset la bandera
        r++;
        if(r==10){
            r=0;
        }
    }
}
//**************************
// Ciclo principal
//**************************
void main(void) {
    Setup();   // general set up
    PORTAbits.RA0=1;//indicadores del  pic  esta on
    PORTAbits.RA1=1;//indicadores del  pic  esta on
    PORTAbits.RA0=0;//indicadores del  pic  esta on
    PORTAbits.RA1=0;//indicadores del  pic  esta on
    I2C_Master_Init(100000); //set up of clk rate 
    //**************************
    // Loop principal
    //**************************
    while (1){
// sequence to read that i2c 
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x00);
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        seg =  I2C_Master_Read(1);
        min =  I2C_Master_Read(1);
        week =  I2C_Master_Read(1);
        hor =  I2C_Master_Read(1);
        day =  I2C_Master_Read(1);
        mou =  I2C_Master_Read(1);
        year=  I2C_Master_Read(0);
        I2C_Master_Stop();
        CONVET();//extraction of usable bits from location 
        first_send();// 
    }}
//**************************
// Configuración
//**************************
void Setup(void){
//puertos on clear 
    PORTA =  0; //PORTA EN 0
    PORTB =  0; //PORTB EN 0
    PORTC =  0; //PORTC EN 0
    PORTD =  0; //PORTD EN 0
    PORTE =  0; //PORTE EN 0
// inputs y otputs
    TRISA =  0B00000000; //INPUT EN porta
    TRISB =  0B00000000; //INPUT EN portb
    TRISC =  0B00000000; //INPUT EN portc
    TRISD =  0B00000000; //INPUT EN portd
    TRISE =  0B0000;     //INPUT EN porte
// analog inputs 
    ANSEL =  0B00000000; // solo estamos usando el RA0 como analogico 
    ANSELH = 0B00000000;
// Configuración de timer0------------------------------------------------------
    INTCONbits.GIE=1; //on todas las interrupts global 
    INTCONbits.PEIE=1;//on periferal
    INTCONbits.TMR0IE=1;//on timer 0
    INTCONbits.TMR0IF=0;// clean flags
    PIE1bits.RCIE = 1;
    OPTION_REGbits.T0CS=0; //internal instructuion cycle clock 
    OPTION_REGbits.T0SE=0; // edge selection 
    OPTION_REGbits.PSA=0; //
//SELECT pre_scaler -----------------------------------------------------------
    OPTION_REGbits.PS0=1;
    OPTION_REGbits.PS1=1;
    OPTION_REGbits.PS2=1;
//set up of libery  -----------------------------------------------------------   
    initOsc(6);
    RX(); //set up to send data 
    TX(); //set up to recive  
}
//**************************
// funciones 
//**************************
void CONVET      (void){
    mou_t = ((mou & 0b00010000)>>4);
    mou_u = (mou  & 0b00001111);   
    day_t = ((day & 0b00110000)>>4); 
    day_u = (day  & 0b00001111);   
    hor_t = ((hor & 0b11000000)>>4);
    hor_u = (hor  & 0b00000011);
    min_t = ((min & 0b01110000)>>4);
    min_u = (min  & 0b00001111);
    seg_t = ((seg & 0b01110000)>>4);
    seg_u = (seg  & 0b00001111);    
}

void first_send  (void){
    if (C==1){send_seg();}
    if (b==1){send_min();}
    if (g==1){send_hora();}
}
void send_hora   (void){
       // this send the tenths and units of hour
    switch (h){
        case 0:
            TXREG = (hor_t+0x30);
            while(!TXSTAbits.TRMT);
            h++;
            break;
        case 1:
            TXREG = (hor_u+0x30);
            while(!TXSTAbits.TRMT);
            h++;
            break;
        case 2:
            TXREG = (0x3A);
            while(!TXSTAbits.TRMT);
            h=0;
            g=0;
            break;
    }}
void send_min    (void){
       // this send the tenths and units of min
    switch (m){
        case 0:
             TXREG = (min_t+0x30);
             while(!TXSTAbits.TRMT);
             m++;
            break;
        case 1:
             TXREG = (min_u +0x30);
             while(!TXSTAbits.TRMT);
             m++;
            break;
        case 2:
            TXREG = (0x03);
            while(!TXSTAbits.TRMT);
            m=0;
            b=0;
            break;
    }}         
void send_seg    (void){
    // this send the tenths and units of seg 
    switch (s){
        case 0:
            TXREG = (seg_t+0x30);
            while(!TXSTAbits.TRMT);
            s++;
            break;
        case 1:
            TXREG = (seg_u+0x30);
            while(!TXSTAbits.TRMT);
            s++;
            s=0;
            C=0;
            break;
        case 2:
            TXREG = (0x03);
            while(!TXSTAbits.TRMT);
            s=0;
            C=0;
            break;
    }}