/*
 * File:   main_transmitter.c
 * Author: Ljubica Potrebic
 *
 * Created on March 14, 2022, 8:13 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p30fxxxx.h>
#include <p30F4013.h>

_FOSC(CSW_FSCM_OFF & XT_PLL4);//instrukcioni takt je isti kao i kristal
_FWDT(WDT_OFF);

#define TMR2_period 1000 /*  Fosc = 10MHz,
					          1/Fosc = 0.1us !!!, 1us * 1000 = 1ms  */

unsigned char tempRX;

void __attribute__((__interrupt__)) _U1RXInterrupt(void)
{
IFS0bits.U1RXIF = 0;
tempRX=U1RXREG;
} 

/*
   void Init_T2(void)
{
	TMR2 = 0;
	PR2 = TMR2_period;
	
	T2CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T2IP = 3 // T2 interrupt priority (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS0bits.T2IF = 0; // clear interrupt flag
	IEC0bits.T2IE = 1; // enable interrupt

	T2CONbits.TON = 1; // T2 on 
}

void __attribute__ ((__interrupt__)) _T2Interrupt(void) // svakih 1ms
{
	TMR2 =0;
	IFS0bits.T2IF = 0;       
}
 */

void initUART1(void)
{
    U1BRG=0x207;//ovim odredjujemo baudrate 1200

    U1MODEbits.ALTIO=0;//koristimo primarne pinove UART1 (RF2, RF3)

    IEC0bits.U1RXIE=1;//omogucavamo rx1 interupt

    U1STA&=0xfffc;

    U1MODEbits.UARTEN=1;//ukljucujemo ovaj modul

    U1STAbits.UTXEN=1;//ukljucujemo predaju
}

void WriteUART1(unsigned int data)
{
	  while(!U1STAbits.TRMT);

    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

void PutString(register const char *str)
{
    while((*str)!=0)
    {
        WriteUART1(*str);
        if(*str==13)WriteUART1(10);
        if(*str==10)WriteUART1(13);
        str++;
    }
}

void PinInit() //tasteri (pin je ulazni i digitalni)
{ 
    TRISBbits.TRISB0 = 1; // TURBO
    TRISBbits.TRISB1 = 1; // RIGHT
    TRISBbits.TRISB2 = 1; // UP
    TRISBbits.TRISB3 = 1; // DOWN
    TRISBbits.TRISB4 = 1; // LEFT
    
    ADPCFGbits.PCFG0 = 1;
    ADPCFGbits.PCFG1 = 1;
    ADPCFGbits.PCFG2 = 1;
    ADPCFGbits.PCFG3 = 1;
    ADPCFGbits.PCFG4 = 1;
}


int main(int argc, char** argv) {
    
    PinInit();
    initUART1();
    //Init_T2();

    while(1)
    {
        if(PORTBbits.RB0 && !PORTBbits.RB1 && !PORTBbits.RB3 && !PORTBbits.RB4)
        {
            PutString("000ttt"); // TURBO
        }
        else if(PORTBbits.RB3 && !PORTBbits.RB1 && !PORTBbits.RB4)
        {
            PutString("000ddd"); // DOWN
        }
        else if(PORTBbits.RB4)
        {
            PutString("000lll"); // LEFT, u zagradi pise slovo, a ne jedan
        }
        else if(PORTBbits.RB1)
        {
            PutString("000rrr"); // RIGHT
        }
        else if(PORTBbits.RB2 && !PORTBbits.RB1 && !PORTBbits.RB4)
        {
            PutString("000uuu"); // UP
        }
        else if(PORTBbits.RB2 && PORTBbits.RB4 && !PORTBbits.RB1)
        {
            PutString("000qqq"); // UP LEFT
        }
        else if(PORTBbits.RB2 && !PORTBbits.RB4 && PORTBbits.RB1)
        {
            PutString("000www"); // UP RIGHT
        }
        else if(PORTBbits.RB3 && PORTBbits.RB4 && !PORTBbits.RB1)
        {
            PutString("000xxx"); // DOWN LEFT
        }
        else if(PORTBbits.RB3 && !PORTBbits.RB4 && PORTBbits.RB1)
        {
            PutString("000yyy"); // DOWN RIGHT
        }
        if(PORTBbits.RB0 && !PORTBbits.RB1 && PORTBbits.RB3 && !PORTBbits.RB4)
        {
            PutString("000aaa"); // TURBO DOWN
        }
         if(PORTBbits.RB0 && !PORTBbits.RB1 && !PORTBbits.RB3 && PORTBbits.RB4)
        {
            PutString("000bbb"); // TURBO LEFT
        }
         if(PORTBbits.RB0 && PORTBbits.RB1 && !PORTBbits.RB3 && !PORTBbits.RB4)
        {
            PutString("000ccc"); // TURBO RIGHT
        }
    }
    return 0;
}
