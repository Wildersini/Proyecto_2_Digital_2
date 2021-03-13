#ifndef UART_H
#define UART_H
#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdint.h>

//void baud (void);
//void conf_tx (void);
//void conf_rc (void);
//uint8_t R_UART ();
//void W_UART_S (char a);/
void TX (void);
void RX (void);
///void Wr_UART (uint8_t a);
#endif