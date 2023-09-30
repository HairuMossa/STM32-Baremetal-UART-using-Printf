#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>

// first we need to check how many uart modules we have in this uC
// in the datasheet uasart2 is connected to APB1 and to enable it
	// we neet to make bit number 17 to 1
#define UART2EN 		1<<17

// for this example we want the UART to transmit data only(Tx)
	// after checking the Alternate function Mapping table from
	// the datasheet of the uC, Tx is at PA2 pin
#define GPIOAEN			1<<0






// the name of the registers that allows the usart to be as transmitter is called control register(USART_CR1)
// and I need to set the  3rd bit. this can be found in the datasheet
#define USART_CR1_TE1		1<<3
// TO ENABLE UART  I need to set the UART Ebable register(datasheet)
#define USART_CR1_UE1 		1<<13
#define SR_TXE		 		1<<7



#define UART_BAUDRATE 		115200 //IS A POPULAR. IT CAN ALSO BE 9600

#define SYS_FREQ 		16000000 // THE DEFAULT SYSTEM CLOCK IS 16Mhz IF NOT CONFIGURED
#define APB1CLK			SYS_FREQ // if there is no prescaller for the clk they are the same


static uint16_t compute_uart_bd(uint32_t periphClk, uint32_t Baudrate);
static void uart_set_baudrate (USART_TypeDef * USARTx, uint32_t periphClk, uint32_t Baudrate);

void UART2_Tx_initt(void);
void uart2_write(int ch);

int __io_putchar(int ch){

	uart2_write(ch);
	return ch;
}

int main(void)
{
	UART2_Tx_initt();

	while(1){

		printf("Hello from stm32 ------- \n\r");

	}

}

void uart2_write(int ch){

	//make sure the transmit deta register is empty
	while (!(USART2->SR & SR_TXE)) {
		// wait untill it is free
	}
	// write to transmit data register

	USART2->DR = (ch & 0xFF);
}


void UART2_Tx_initt(void){

	/*configure uart gpioi pin */
		/*1. Enable clock access to gpioa */
	RCC->AHB1ENR |= GPIOAEN;
		/*2. set PA2 mode to alternate funciton mode */
	// 10: Alternate function, mode on pin 2
	GPIOA->MODER |= 1<<5; // SET bit  5
	GPIOA->MODER &= ~(1<<4); // reset bit  4

		/*3. set PA2 alternate function type to UART_TX (AF07) - I got this from the uC datasheet
		 * 		alternate function map table where usart2_tx is at AF07, to enable it 0111: AF7*/
	// st made a array afr[0] is low an afr[1] is high
	GPIOA->AFR[0] |= 1<<8;
	GPIOA->AFR[0] |= 1<<9;
	GPIOA->AFR[0] |= 1<<10;
	GPIOA->AFR[0] &= ~(1<<11);



	/*configure uart module */
		/*1. Enable clock access to UART2 */
	// uart2 is connected to APB1 REGISTER
	RCC->APB1ENR |= UART2EN;
		/*configure the baudrate */
	uart_set_baudrate(USART2,APB1CLK, UART_BAUDRATE );
		/*configure the transfer direction*/
	USART2->CR1 = USART_CR1_TE1; // here we set every bit to zero and then just one. the reason
	// is that the we want the default settings for this uart.

		/*enable uart module*/
	USART2->CR1 |= USART_CR1_UE1; // we only want to change ue bit to one;


}

static void uart_set_baudrate (USART_TypeDef * USARTx, uint32_t periphClk, uint32_t Baudrate){

	USARTx->BRR = compute_uart_bd(periphClk,Baudrate);

}


static uint16_t compute_uart_bd(uint32_t periphClk, uint32_t Baudrate){


	return ((periphClk + (Baudrate/2U))/Baudrate);

}
