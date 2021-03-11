/*#ifndef _UART_H
#define _UART_H
#include <xc.h> // include processor files - each processor file is guarded.  
void initUSART (void);
#endif*/
#ifndef _UART_H_
#define _UART_H_
#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdint.h>
void initUSART (void);
void baud (void);
void conf_tx (void);
void conf_rc (void);
uint8_t R_UART ();
void W_UART_S (char *a);
///void Wr_UART (uint8_t a);
#endif	