/************************************************************************************
*         File: MP279x_I2C.c																												*
*					Description: Contains the MP279x I2C Functions 													  *
*					CPU: STM32F03xx Medium density devices																		*
*					Project:VGX, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Emrullah GÜNAY — KOBİNERJİ A.Ş.																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/

#include "MP279x.h"
#include "Init.h"
#include "BMS_MP279x.h"

//MP279xC VARIABLES********************************************************************************************************
//CRC8 Table:
const uint8_t CRC8Table[256] = {
0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

//MP279xC FUNCTIONS********************************************************************************************************
static __inline void SysTick_Delay_us(volatile uint32_t microseconds);

extern unsigned char SysTick_Delay_Done, frequency;

//This function Writes a 16b register of the MPQ26454A. Error flag return if there is any problem.*****************************************************
uint8_t I2C_MP279xC_Write16B(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t Register, uint8_t DataL, uint8_t DataH, uint8_t usecrc, uint8_t add_delay)
{
	uint32_t errorCont=0;
	uint8_t errorFlag=0;
	uint8_t count=0, CRCInput[4], CRCResult;
	uint32_t I2C_Sts = 0;

	if (add_delay) SysTick_Delay_us(10);
	
	if (usecrc)
	{
		CRCInput[0]=(Address<<1);
		CRCInput[1]=Register;
		CRCInput[2]=DataL;
		CRCInput[3]=DataH;
		CRCResult=CRC8(CRCInput,4);
	}
	
	do
	{
		errorFlag=0;
		I2Cx->CR2&=~(I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
		if (usecrc) I2Cx->CR2|=((Address<<1) | (4<<I2C_CR2_NBYTES_Pos));
		else I2Cx->CR2|=((Address<<1) | (3<<I2C_CR2_NBYTES_Pos));
		I2Cx->ICR|=I2C_ICR_NACKCF;
		LL_I2C_GenerateStartCondition(I2Cx); 
		do
		{
			I2C_Sts = I2Cx->ISR;
			errorCont++;
			if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
			{
				errorFlag=1;
				break; }
		}
		while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait until the START condition is generated and address is sent to slave and ACK is received from slave
		errorCont=0;
		if(!errorFlag)
		{
			I2Cx->TXDR=Register;		//Write register
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait Aknowledment by slave
			errorCont=0;
			if(!errorFlag) //If no error, write the LSB content of the register
			{
				I2Cx->TXDR=DataL; //Transmit the LSB byte
				do
				{
					I2C_Sts = I2Cx->ISR;
					errorCont++;
					if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
					{
						errorFlag=1;
						break; }
				}
				while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait Aknowledment by slave
			}
			errorCont=0;
			if(!errorFlag) //If no error, write the HSB content of the register
			{
				I2Cx->TXDR=DataH; //Transmit the HSB Byte
				do
				{
					I2C_Sts = I2Cx->ISR;
					errorCont++;
					if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
					{
						errorFlag=1;
						break; }
				}
				while (!(I2C_Sts&(usecrc ? I2C_ISR_TXIS : I2C_ISR_TC))); 							 			//Wait Aknowledment by slave
			}
			if ((!errorFlag)&&usecrc) //If no error, write the CRC byte
			{
				I2Cx->TXDR=CRCResult; //Transmit the CRC Byte
				do
				{
					I2C_Sts = I2Cx->ISR;
					errorCont++;
					if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
					{
						errorFlag=1;
						break; }
				}
				while (!(I2C_Sts&I2C_ISR_TC)); 							 			//Wait Aknowledment by slave
			}
			LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
		}
		count++;
		if (errorFlag)
		{
			LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
			LL_I2C_Disable(I2Cx);		//DISABLE the I2C channel
			RCC->APB1ENR&=~RCC_APB1ENR_I2C2EN; 	//DISABLE peripheral I2C2 clock
			
			GPIOB->MODER&=~(0x0F<<20);
			GPIOB->MODER|=(0x05<<20);
			GPIOB->BRR|=(0x0C00);
			SysTick_Delay_us(2);
			GPIOB->BSRR|=(0x0400);
			SysTick_Delay_us(2);
			GPIOB->BSRR|=(0x0800);
			GPIOB->MODER&=~(0x0F<<20);
			
			I2C_Config_MP279x(I2Cx);
			LL_I2C_Enable(I2Cx);		//ENABLE the I2C channel
		}
		if (count>9)
			break;
	}
	while (errorFlag);
	return errorFlag;
}

//This function Reads a 16b register from MPQ2645A BMS chip. Error flag return if there is any problem.***************************************************
uint8_t I2C_MP279xC_Read16B(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t Register, uint8_t* DataL, uint8_t* DataH, uint8_t usecrc, uint8_t add_delay)
{
	uint16_t errorCont=0;
	uint8_t errorFlag=0;
	uint8_t count=0, CRCInput[6], CRCResult, CRCReceived;
	uint8_t Data_Aux[2];
	uint32_t I2C_Sts;

	if (add_delay) SysTick_Delay_us(10);
	
	if (usecrc)
	{
		CRCInput[0]=(Address<<1);
		CRCInput[1]=Register;
		CRCInput[2]=(Address<<1)|0x01;
		CRCInput[3]=Register;		
	}
	do
	{
		errorFlag=0;
		//Firts Ask for the register addres. For this mode, a START + (Address+WR) + Register will be send. The I2C line will be for read mode.
		I2Cx->CR2&=~(I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
		I2Cx->CR2|=((Address<<1) | (1<<I2C_CR2_NBYTES_Pos));
		I2Cx->ICR|=I2C_ICR_NACKCF;
		LL_I2C_GenerateStartCondition(I2Cx); 
		do
		{
			I2C_Sts = I2Cx->ISR;
			errorCont++;
			if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
			{
				errorFlag=1;
				break; }
		}
		while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait until the START condition is generated and address is sent to slave and ACK is received from slave
		errorCont=0;
		if(!errorFlag) //If no error, next write register
		{
			I2Cx->TXDR=Register;		//Write register
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TC)); 							 			//Wait Aknowledment by slave
		}
		errorCont=0;
		
		//Now the content of the register will be read. For this, a change mode to read is needed.
		if(!errorFlag)
		{
			I2Cx->CR2&=~I2C_CR2_NBYTES_Msk;
			if (usecrc) I2Cx->CR2|=((Address<<1) | (3<<I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN);
			else I2Cx->CR2|=((Address<<1) | (2<<I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN);
			LL_I2C_GenerateStartCondition(I2Cx);
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMP279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_RXNE)); 							 			//Wait Response from slave
			errorCont=0;
			//Now we are in read mode, and TRA=0, BUSY=1, MSL=1. The next step is read registers
			if(!errorFlag)
			{
				if(usecrc) Data_Aux[0]=I2Cx->RXDR;
				else *DataL=I2Cx->RXDR;																												//Recive the data from Slave.
				//I2Cx->CR1 |= I2C_CR1_ACK; 																		 					   //Send a AKC for confirm to slave that the data is correctly send
				while (!(I2Cx->ISR&(usecrc ? I2C_ISR_RXNE : I2C_ISR_TC))) 					   																//Wait for receiving next byte
				{
					errorCont++;
					if (errorCont>=I2CToutMP279x)
					{
					errorFlag=1;
					break; }
				}
				if(!errorFlag)
				{
					if(usecrc) Data_Aux[1]=I2Cx->RXDR;
					else *DataL=I2Cx->RXDR;																												//Recive the data from Slave.
				}
			}
			errorCont=0;
			if ((!errorFlag)&&usecrc)
			{
				//I2Cx->CR1 &= ~(I2C_CR1_ACK); 																	 						//Send a NACK for confirm to slave that the data is correctly send and it's the last send.
				while (!(I2Cx->ISR&I2C_ISR_TC)) 													//Wait for receiving next byte
				{
					 errorCont++;
					 if (errorCont>=I2CToutMP279x)
					{
					errorFlag=1;
					break; }
				}
				if(!errorFlag)
				{
					CRCReceived=I2Cx->RXDR;																												//Receive the data form slave
				}
			}
			LL_I2C_GenerateStopCondition(I2Cx); 														     				  	//Generates a STOP condition
		}
		if ((!errorFlag)&&usecrc) //If no error, write the CRC byte
		{
			CRCInput[4]=Data_Aux[0];
			CRCInput[5]=Data_Aux[1];
			CRCResult=CRC8(CRCInput,6);
			if (CRCResult!=CRCReceived)
				errorFlag=1;
			else
			{
				*DataL=Data_Aux[0];
				*DataH=Data_Aux[1];
			}
		}
		count++;
		if (errorFlag)
		{
			LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
			LL_I2C_Disable(I2Cx);		//DISABLE the I2C channel
			RCC->APB1ENR&=~RCC_APB1ENR_I2C2EN; 	//DISABLE peripheral I2C2 clock
			
			GPIOB->MODER&=~(0x0F<<20);
			GPIOB->MODER|=(0x05<<20);
			GPIOB->BRR|=(0x0C00);
			SysTick_Delay_us(2);
			GPIOB->BSRR|=(0x0400);
			SysTick_Delay_us(2);
			GPIOB->BSRR|=(0x0800);
			GPIOB->MODER&=~(0x0F<<20);
			
			I2C_Config_MP279x(I2Cx);
			LL_I2C_Enable(I2Cx);		//ENABLE the I2C channel
		}
		if (count>9)
			break;
	}
	while (errorFlag);
	return errorFlag;
}

//Calculate CRC8 for MP279xC
uint8_t CRC8(uint8_t* data, uint16_t length)
{
  uint16_t crc= 0x00;
  uint16_t j;
  for (uint16_t i=0; i<length; i++)
  {
    j = (crc ^ *data++) & 0xFF;
    crc = (CRC8Table[j] ^ (crc << 8)) & 0xFF;
  }
  return (uint8_t) (crc & 0xFF);
}

//Generate us delay
static __inline void SysTick_Delay_us(volatile uint32_t microseconds)
{
  SysTick_Delay_Done = 0;
  SysTick->LOAD = microseconds*(frequency/8);
	SysTick->VAL = 0;
	SysTick->CTRL &=~ SysTick_CTRL_COUNTFLAG_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	// COUNTFLAG is a bit that is set to 1 when counter reaches 0.
	// It's automatically cleared when read.
	while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
	SysTick_Delay_Done = 1;
}
