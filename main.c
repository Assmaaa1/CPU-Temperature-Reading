#include "stm32f4xx.h"                  // Device header
#include <stdio.h>
#include <inttypes.h>



/* ADC Setup Parameters */

#define ADC_EN (1<<0)
#define ADC_DIS (0<<0)
#define ADC_CLK_EN (1<<8)
#define ADC_CLK_SOURCE
#define ADC_INT_EN (1<<5)
#define CONT_CONVER_EN (1<<1)
#define AUTOMATIC_CONV_EN (1<<8)
#define ADC_CHANNEL_EN (1<<15) // channel 16 is selected
#define START_CONV (1<<30)
#define TEMP_SENSOR_EN (1<<23)
#define ADC_SAMP_TIME (1<<20)
#define Vbat_En (1<<22)

/* UART Setup Parameters */

#define GPIOD_EN_CLK (1<<3)
#define USART2_CLK_EN (1<<17)
#define USART_TX_EN (1<<3)
#define USART_TX_DI (0<<3)
#define USART_RX_EN (1<<2)
#define USART_RX_DI (0<<2)
#define USART_EN (1<<13)
#define USART_PIN1 (1<<5)
#define USART_PIN2 (1<<6)
#define GPIO_D_MODE_5 (1<<11)
#define GPIO_D_MODE_6 (1<<13)



void ADC_INIT(void);
void USART2_INIT(void) ;
void USART_Write(int ch);
int USART_Read(void);
int fputc (int c, FILE *f);
int fgetc(FILE *f);

int val;
double Vsense , Temp; 
// Calibration coerfficients from the data datasheet 

double TempAmb = 0.76;
double AvgSlope = 2.5;

int main (void)
	
{
	
	
	USART2_INIT();
  ADC_INIT();

	while (1)
	
{
     
     ADC1->CR2 |= START_CONV;
     while(!(ADC1->SR & 0x2)) {}
		 val= ADC1->DR  ;
		 Vsense= (double) (val * 3)/4096; 
		 Temp = ((Vsense - TempAmb) / AvgSlope) + 25;
		 printf("\n\r CPU Temperature = %.2f\370C",Temp);

}

}

void ADC_INIT(void)
	
{

		ADC1->CR2 |= ADC_DIS;
	  RCC->APB2ENR |= ADC_CLK_EN ;
	  ADC1->CR1 |= ADC_INT_EN ;
	  ADC1->CR2 |= CONT_CONVER_EN ;
	  ADC1->CR1 |= AUTOMATIC_CONV_EN ;
	  ADC->CCR |= TEMP_SENSOR_EN;
	  ADC->CCR &= ~Vbat_En ;
    ADC1->CR2 |= ADC_EN;
	  ADC1->SMPR1 |= ADC_SAMP_TIME ; // 84 cycles -> sampling time = 12us
	  ADC1->SQR1 |= ADC_CHANNEL_EN;

}

void USART_Write(int ch) // int
	
{ // wait while TX buffer is empty
	while(!(USART2->SR & 0x80)) {}
	USART2->DR = (ch & 0xFF);
}


int USART_Read(void)
	
{ 
	while(!(USART2->SR & 0x20)) {}  // wait while RX buffer is empty
	
  
	return USART2->DR ;
}

void USART2_INIT(void) 
{
	RCC->AHB1ENR |= GPIOD_EN_CLK; // because USART is connected to PIN PD5 and PD6
	RCC->APB1ENR |= USART2_CLK_EN  ;
	GPIOD->AFR[0] |= 0x07700000;  // USART connected to PD5 and PD6
	GPIOD->MODER |=GPIO_D_MODE_5; // PIN PD5 alternate function mode
	GPIOD->MODER |=GPIO_D_MODE_6; // PIN PD6 alternate function mode
	USART2->CR1 |=USART_EN ;
	USART2->CR1 ^=USART_TX_EN;
	USART2->CR1 ^=USART_RX_EN;
	USART2->BRR = 0x0683; //9600
	
}


struct __FILE

{

int handle;

};



FILE __stdin = {0};
FILE __stdout = {1};
FILE __stderr = {2};

int fgetc(FILE *f)
{
	int c;
	c = USART_Read();
	if (c == '\r'){ // if enter key is pressed (a carriage return, writing sarts from the begining of the line 
		USART_Write(c); // then write the entred caracter c into the terminal
			c = '\n';   
  }		
	USART_Write(c);
	
	return c;

}

int fputc (int c, FILE *f)
{
	
	USART_Write(c);
	return USART2->DR ;
}


	


