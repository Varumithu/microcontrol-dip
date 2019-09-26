/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-09-19

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "stm32f0xx.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/


void SysTick_Handler() {
//TODO
}



//PC13 - SD CD
//PC3 -  SD CS
//
//PB13 - SCK
//PB14 - MISO
//PB15 - MOSI

// If i understand correctly, PB12 would be slave select for the board if it was a slave, but it is used for other stuff
void SPI2_IRQHandler(void) {
//TODO

}
// Most significant bit must be first
void init_sd_spi() {


	RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN;


	GPIOB->MODER |= GPIO_MODER_MODER15_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1;

	GPIOC->MODER |= GPIO_MODER_MODER3_0 | GPIO_MODER_MODER6_0; // slave select and a light

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;


	SysTick->LOAD = 8000000 / 1000 - 1;
	SysTick->VAL = 8000000 / 1000 - 1;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;

	GPIOB->AFR[1] |= (0 << 4 * (15 - 8)) | (0 << 4 * (13 - 8)) | (0 << 4 * (14 - 8)); // AF 1 for 13, 14, 15. 12 is not needed afaik



//	SPI2->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;
//	SPI2->CR2 |= SPI_CR2_DS | SPI_CR2_RXNEIE; // 1111 - 16bit data size

	SPI2->CR1 |= SPI_CR1_BR_2 | SPI_CR1_MSTR;
			// Baud rate f/32 ~ 400 kHz
			// clock phase and polarity 0 0;
	SPI2->CR2 |= SPI_CR2_FRXTH | SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;
				//8-bit FIFO
	SPI2->CR1 |= SPI_CR1_SPE;
//	NVIC_EnableIRQ (SPI2_IRQn);

}

//Send a dummy byte.
//Then send the following 6-byte command continuously
//First byte: 0x40
//Next four bytes: 0x00000000
//CRC byte: 0x95
void SPI_SD_CMD0() {
	while(1) {
		if ((SPI2->SR & SPI_SR_TXE) == SPI_SR_TXE) {
			SPI2->DR = 0xFF;
			break;
		}
	}
	while (1) {
		if ((SPI2->SR & SPI_SR_TXE) == SPI_SR_TXE) {
			SPI2->DR = 0x40;
			break;
		}
	}
	uint8_t i = 0;
	while (i < 4) {
		if ((SPI2->SR & SPI_SR_TXE) == SPI_SR_TXE) {
			SPI2->DR = 0x00;
			++i;
		}
	}
	while (1) {
		if ((SPI2->SR & SPI_SR_TXE) == SPI_SR_TXE) {
			SPI2->DR = 0x95;
			break;
		}
	}


}

int main(void) {

	init_sd_spi();


	GPIOC->BSRR = GPIO_BSRR_BS_3; // slave select high

	for(uint8_t i = 0; i < 100; ++i) {
		//wait a bit just in case
	}
	//GPIOC->BSRR = GPIO_BSRR_BR_3; // drive slave select from high to low
	uint8_t i = 0;
	uint8_t junk = 0;
	while(i < 11) {
		if ((SPI2->SR & SPI_SR_TXE) == SPI_SR_TXE) {
			SPI2->DR = 0xFF; // so we send around 11 dummy bytes i guess1;
			++i;
		}
		if ((SPI2->SR & SPI_SR_RXNE) == SPI_SR_RXNE) {
			junk = SPI2->DR;
		}
	}
	GPIOC->BSRR = GPIO_BSRR_BR_3;
	SPI_SD_CMD0();
	uint8_t message = 0;
	while (1) {
		if ((SPI2->SR & SPI_SR_RXNE) == SPI_SR_RXNE) {
			message = SPI2->DR;
			if (message == 0x01) {
				GPIOC->ODR |= GPIO_ODR_6;
			}
			break;
		}
	}

	while (1) {
		++i; // idle loop
	}


}
























