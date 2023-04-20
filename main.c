/*
 * File:   main8.c
 * Author: Oscar Escriba
 *
 * Created on 13 de abril de 2023, 10:04 PM
 */

// CONFIG1
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

#include <xc.h>
#include <stdint.h> 
#include <stdio.h> 
#define _XTAL_FREQ 1000000  
void USART_send(const char data); 
void USART_print(const char *string);
//VARIABLES 
const char data =65;  
char mensaje[]="hola mundo";  
// VARIABLES GLOBALES 
int state_flag =0; 
uint8_t pot1; 
uint8_t opt_sel;
uint8_t portb_char; 
unsigned char vacio = '\r';  

//PROTOFUNCIONES 
void setup(void); 
void item_list(void); 
void enter(int a); 

//prototipo de funciones 
void setup(void); 
//void __interrupt () isr(void) { 
 //   if (PIR1bits.RCIF) { 
   //     PORTD = RCREG; 
   // }
//}   
void USART_print(const char *string)
{
    int i = 0;
    
    for(i; string[i] != '\0'; i++)
    {
        USART_send(string[i]);
    }
}  
void USART_send(const char data)
{
    while(!TRMT);
    TXREG = data;
} 
//FUNCION PARA ENTERS DE CANTIDAD ESPECIFICADA  
void enter(int a) { 
    while (a>0) { 
        a--; 
        __delay_us(40); 
        TXREG= vacio; 
    }
} 
//INTERRUPCIONES 
void __interrupt() isr(void){ 
    if (PIR1bits.ADIF) { 
        if (ADCON0bits.CHS==0) {
            pot1 = ADRESH; 
        } 
        PIR1bits.ADIF=0; 
    } 
    if (RCIF) { 
        if (state_flag ==0) {
            opt_sel=RCREG; 
            RCREG=0;
        }
        else if (state_flag ==1){ 
            state_flag =0;
            portb_char=RCREG; 
            TXREG = portb_char; 
            PORTB= portb_char; 
            RCREG=0; 
            enter(1); 
            item_list(); 
        }
    } 
    return; 
}
void main(void) {
    setup();  
    item_list(); 
    //loop principal 
    while (1){ 
        if (ADCON0bits.GO==0){ 
            ADCON0bits.GO=1; 
        } 
        if (opt_sel == 0x61){
            opt_sel = 0x00; 
            USART_print("valor del potenciometro: "); 
            __delay_ms(500); 
            TXREG = pot1; 
            __delay_ms(500); 
            item_list();
        } 
        else if(opt_sel ==0x62) {
            state_flag=1; 
            USART_print("Ingrese el caracter en ASCCI: "); 
            opt_sel =0x00; 
        }
    }
    return;
}  
//configuracion 
void setup(void) { 
    //configuracion de los puertos 
    ANSEL =0b00000001; //ans0 como entrada analogica 
    ANSELH =0; 
    TRISD=0; 
    PORTD=0x00;  
    
    TRISA =0b00000001; 
    TRISB =0; 
    PORTA=0; 
    PORTB=0; 
    //configuracion del oscilador a 1Mhz 
    OSCCONbits.IRCF=0b100; 
    OSCCONbits.SCS=1; 
    //SERIAL CONFIG 
    TXSTAbits.SYNC=0; 
    TXSTAbits.BRGH=1;  
    BAUDCTLbits.BRG16=1; 
    SPBRG=25; 
    SPBRGH=0;  
    RCSTAbits.SPEN=1; 
    RCSTAbits.RX9=0; 
    RCSTAbits.CREN=1; 
    TXSTAbits.TXEN=1;  
    
    //CONFIGURACION DE INTERRUPCIONES 
    PIR1bits.RCIF=0; 
    PIE1bits.RCIE=1;  
    PIR1bits.ADIF=0; 
    PIE1bits.ADIE=1; 
    INTCONbits.PEIE=1; 
    INTCONbits.GIE=1;  
    
    //CONFIGURACION DEL ADC 
    ADCON0bits.CHS= 0b0000; 
    ADCON1bits.ADFM=0;
    ADCON0bits.ADON=1; 
    __delay_us(40); 
} 
//MENU DE LA TERMINAL 
void item_list() { 
    enter(2); 
    USART_print("----------Main menu----------"); 
    enter(1); 
    USART_print("a) lectura del potenciometro"); 
    enter(1); 
    USART_print("b) enviar ASCII"); 
    enter(2); 
}
