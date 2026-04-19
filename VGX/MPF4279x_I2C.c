/************************************************************************************
*         File: MPF4279x_I2C.c																												*
*					Description: Contains the functions for the I2Cmaster interface with the	*
*						MPF4279x IC																															*
*					CPU: STM32F03xx Medium density devices																		*
*					Project:VGX, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Emrullah GÜNAY — KOBİNERJİ A.Ş.																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/

#include "MPF4279x.h"
#include "init.h"

//MPF4279x I2C FUNCTIONS********************************************************************************************************
static __inline void SysTick_Delay_us_CRC(volatile uint32_t microseconds);

extern unsigned char SysTick_Delay_Done, frequency;

uint8_t I2C_MPF4279x_Write(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t *Data, uint8_t len, uint8_t Activate, uint8_t CRC_En)
{
	uint32_t CRCInput=0;
	uint16_t errorCont=0;
	uint8_t errorFlag=0, i=0;
	uint8_t trans_len = len+3;
	uint32_t I2C_Sts = 0;
	
	if (CRC_En)
	{
		CRC->CR|=1;								//Reset the CRC calculation unit
		SysTick_Delay_us_CRC(1);
		CRC->DR=len|(Register<<8);
		while (i<len)
		{
			CRCInput|=Data[i]<<(i%4)*8;
			if (((i%4)==3)|(i==(len-1)))
			{
				CRC->DR=CRCInput;
				CRCInput=0;
			}
			i++;
		}
		i=0;
		trans_len+=4;
	}
	
	if (Activate)
	{
		if(MPF4279x_to_Active_Mode(I2Cx, Address, Register, trans_len, NO_STOP)) return 1;
	}
	
	for (char errcnt=0; errcnt<3; errcnt++)
	{
		errorFlag=0;		
		if (!Activate)
		{
			//Set Slave Address and length of transmission:
			I2Cx->CR2&=~(I2C_CR2_SADD | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
			uint8_t add_bytes=3;
			if (CRC_En) add_bytes=7;
			I2Cx->CR2|=((Address<<1) | ((len+add_bytes)<<I2C_CR2_NBYTES_Pos));
			//Set Register address low byte as first byte to transfer:
			I2Cx->TXDR=Register&0xFF;		//Write the register
			I2Cx->ICR|=I2C_ICR_NACKCF;
			LL_I2C_GenerateStartCondition(I2Cx); 
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait until the START condition is generated and address is sent to slave and ACK is received from slave
			errorCont=0;
		}
		if(!errorFlag)
		{
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait acknowledgement by slave
			errorCont=0;
			if (!errorFlag)
			{
				I2Cx->TXDR=(Register&0xFF00)>>8;		//Write Register Address high byte;
				do
				{
					I2C_Sts = I2Cx->ISR;
					errorCont++;
					if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
					{
						errorFlag=1;
						break; }
				}
				while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait acknowledgement by slave
			}
			errorCont=0;
			if(!errorFlag) //If no error, next write register
			{	
				I2Cx->TXDR=len;		//Write length
				do
				{
					I2C_Sts = I2Cx->ISR;
					errorCont++;
					if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
					{
						errorFlag=1;
						break;
					}
				}		
				while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait Aknowledment by slave
			}
			errorCont=0;
			//Start Writing Data:
			if (!errorFlag)
			{
				I2Cx->TXDR=Data[0];
				i=1;
				while ((i<len)&&(errorCont<I2CToutMPF4279x))
				{
					errorCont++;
					I2C_Sts = I2Cx->ISR;
					if (I2C_Sts&I2C_ISR_TXIS)       //Wait Aknowledment by slave
					{
						I2Cx->TXDR=Data[i];
						i++;
						errorCont=0;
					}
					else if (I2C_Sts&I2C_ISR_NACKF)
					{
						errorFlag=1;
						errorCont=I2CToutMPF4279x;
						break;
					}
				}
				if (errorCont>=I2CToutMPF4279x)
				{
					errorFlag=1;
				}
				else
				{
					errorFlag=0;
				}
			}
			if (!errorFlag)
			{
				i=0;
				while ((i<5)&&(errorCont<I2CToutMPF4279x))
				{
					errorCont++;
					I2C_Sts = I2Cx->ISR;
					if (I2C_Sts&(((i==4) || (((i==0)&&(!CRC_En)))) ? I2C_ISR_TC : I2C_ISR_TXIS))
					{
						if ((i==4)||(!CRC_En))
						{
							LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
							break;
						}
						else
						{
							I2Cx->TXDR=CRC->DR>>(8*i);
						}
						i++;
						errorCont=0;
					}
					else if (I2C_Sts&I2C_ISR_NACKF)
					{
						errorFlag=1;
						errorCont=I2CToutMPF4279x;
						break;
					}
				}
				if (errorCont>=I2CToutMPF4279x)
				{
					errorFlag=1;
				}
				else
				{
					errorFlag=0;
				}
			}
		}
		if (errorFlag)
		{
			//Prevent SDA and SCL to get stucked low:
			LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
			LL_I2C_Disable(I2Cx);		//ENABLE the I2C channel
			RCC->APB1ENR&=~RCC_APB1ENR_I2C1EN; 	//DISABLE peripheral I2C1 clock
			
			GPIOB->MODER&=~(0x0F<<12);
			GPIOB->MODER|=(0x05<<12);
			GPIOB->BSRR|=(0xC0);
			GPIOB->MODER&=~(0x0F<<12);
			
			SysTick_Delay_us_CRC(10);
			I2C_Config_MPF4279x(I2Cx);
			LL_I2C_Enable(I2Cx);		//ENABLE the I2C channel
		}
		if (errorFlag) SysTick_Delay_us_CRC(50);
		else
		{
			SysTick_Delay_us_CRC(10);
			break;
		}
	}
	return errorFlag;
}

//This functions reads a register from the MPF4279x
uint8_t I2C_MPF4279x_Read(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t *Data, uint8_t len, uint8_t Activate, uint8_t CRC_En)
{
	uint32_t CRCInput=0;
	uint16_t errorCont=0;
	uint8_t errorFlag=0, i=0, CRCRx[4];
	uint8_t Data_Aux[82];
	uint32_t I2C_Sts = 0;
	
	if (Activate)
	{
		if(MPF4279x_to_Active_Mode(I2Cx, Address, Register, 3, NO_STOP)) return 1;
	}
	
	for (char errcnt=0; errcnt<3; errcnt++)
	{
		errorFlag=0;
		if (!Activate)
		{
			//Firts Ask for the register address. For this mode, a START + (Address+WR) + Register will be send.
			//Set Slave Address and length of transmission:
			I2Cx->CR2&=~(I2C_CR2_SADD | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
			I2Cx->CR2|=((Address<<1) | (3<<I2C_CR2_NBYTES_Pos));
			//Set Register address low byte as first byte to transfer:
			I2Cx->TXDR=Register&0xFF;
			I2Cx->ICR|=I2C_ICR_NACKCF;
			LL_I2C_GenerateStartCondition(I2Cx); 
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait until the START condition is generated and address is sent to slave and ACK is received from slave
			errorCont=0;
		}
		if(!errorFlag) //If no error, next write register
		{
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait acknowledgement by slave
		}
		errorCont=0;
		if (!errorFlag)
		{
			I2Cx->TXDR=(Register&0xFF00)>>8;		//Write Register Address high byte;
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++; 
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait acknowledgement by slave
		}
		errorCont=0;
		if(!errorFlag) //If no error, next write register
		{	
				I2Cx->TXDR=len;		//Write length byte
				do
				{
					I2C_Sts = I2Cx->ISR;
					errorCont++;
					if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
					{
						errorFlag=1;
						break; }
				}
				while (!(I2C_Sts&I2C_ISR_TC)); 							 			//Wait acknowledgement by slave
		}
		errorCont=0;
		//LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
		//for (uint8_t i=0;i<25;i++);
		SysTick_Delay_us_CRC(5);
		i=0;
		
		//Now the content of the register will be read. For this, a change mode to read is needed.
		if(!errorFlag)
		{
		//Firts Ask for the register address. For this mode, a START + (Address+WR) + Register will be send. I2C will be used in Read Mode now.
			I2Cx->CR2&=~(I2C_CR2_SADD | I2C_CR2_NBYTES_Msk);
			uint8_t add_bytes = 0;
			if (CRC_En) add_bytes=4;
			I2Cx->CR2|=((Address<<1) | ((len+add_bytes)<<I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN);
			LL_I2C_GenerateStartCondition(I2Cx); 
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&(((len==1)&&(!CRC_En)) ? I2C_ISR_TC : I2C_ISR_RXNE))); 							 			//Wait acknowledgement by slave
			
			Data_Aux[i]=I2Cx->RXDR;
			i++;
			errorCont=0;
			//Now we are in read mode. The next step is read registers
			if(!errorFlag)
			{
				if (!((len==1)&&(!CRC_En)))
				{
					while ((i<(len+4))&&(errorCont<I2CToutMPF4279x))
					{
						errorCont++;
						if (errorCont>=I2CToutMPF4279x)
						{
							errorFlag=1; 
							break;
						}
						if(I2Cx->ISR&(((i==(len+3)) || (((i==(len-1))&&(!CRC_En)))) ? I2C_ISR_TC : I2C_ISR_RXNE))
						{
							if (i<(len))
							{
								Data_Aux[i]=I2Cx->RXDR;																												//Receive the data from Slave.
							}
							else if (CRC_En)
							{
								CRCRx[i-len]=I2Cx->RXDR;	
							}
							errorCont=0;
							i++;
							if ((i==(len+4))||((!CRC_En)&&(i==len)))
							{
								LL_I2C_GenerateStopCondition(I2Cx);
							}
							if ((!CRC_En) && (i>=len)) break;
						}
					}
				}
				else
				{
					LL_I2C_GenerateStopCondition(I2Cx);
				}
				if (!errorFlag)
				{
					if (CRC_En)
					{
						i=0;
						CRC->CR|=1;								//Reset the CRC calculation unit
						SysTick_Delay_us_CRC(1);
						CRC->DR=len|(Register<<8);
						CRCInput=0;
						while (i<len)
						{
							CRCInput|=Data_Aux[i]<<(i%4)*8;
							if (((i%4)==3)|(i==(len-1)))
							{
								CRC->DR=CRCInput;
								CRCInput=0;
							}
							i++;
						}
						if (CRC->DR!=((CRCRx[3]<<24)|(CRCRx[2]<<16)|(CRCRx[1]<<8)|CRCRx[0]))
						{
							errorFlag=1;		//If CRC don't match, don't accept the data
						}
						else
						{
							for (i=0; i<len; i++) Data[i]=Data_Aux[i];		//If CRC matches, accept the data
						}
					}
					else
					{
						for (i=0; i<len; i++) Data[i]=Data_Aux[i];		//If CRC is disabled, accept the data
					}
				}
			}
		}
		if (errorFlag)
		{
			//Prevent the SCL and SDA lines of getting stucked low:
			LL_I2C_GenerateStopCondition(I2Cx);
			LL_I2C_Disable(I2Cx);		//DISABLE the I2C channel
			RCC->APB1ENR&=~RCC_APB1ENR_I2C1EN; 	//DISABLE peripheral I2C1 clock
			
			GPIOB->MODER&=~(0x0F<<12);
			GPIOB->MODER|=(0x05<<12);
			GPIOB->BSRR|=(0xC0);
			GPIOB->MODER&=~(0x0F<<12);
			
			SysTick_Delay_us_CRC(10);
			I2C_Config_MPF4279x(I2Cx);
			LL_I2C_Enable(I2Cx);		//ENABLE the I2C channel
		}
		if (errorFlag) SysTick_Delay_us_CRC(50);
		else
		{
			SysTick_Delay_us_CRC(10);
			break;
		}
	}
	return errorFlag;
}

//This function puts the FG in Active Mode:
uint8_t MPF4279x_to_Active_Mode(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t len, uint8_t no_stop)
{
	uint16_t errorCont=0;
	uint8_t errorFlag=0;
	uint32_t I2C_Sts = 0;
	
	if (no_stop)
	{
		//Set Slave Address and length of transmission:
		I2Cx->CR2&=~(I2C_CR2_SADD | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
		I2Cx->CR2|=((Address<<1) | (len<<I2C_CR2_NBYTES_Pos));
		//Set Register address low byte as first byte to transfer:
		I2Cx->TXDR=Register&0xFF;		//Write the register
		I2Cx->ICR|=I2C_ICR_NACKCF;
		LL_I2C_GenerateStartCondition(I2Cx); 
		do
		{
			I2C_Sts = I2Cx->ISR;
			errorCont++;
			if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
			{
				errorFlag=1;
				break; }
		}while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait until the START condition is generated and address is sent to slave and ACK is received from slave
	}
	else
	{
		//Check if Acknoledge is returned from MPF4279x to verify it's in Active Mode; if not do a low pulse on SDA
		I2Cx->CR2&=~(I2C_CR2_SADD | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
		I2Cx->CR2|=((Address<<1) | (1<<I2C_CR2_NBYTES_Pos));
		I2Cx->TXDR=0;
		I2Cx->ICR|=I2C_ICR_NACKCF;
		LL_I2C_GenerateStartCondition(I2Cx); 
		do
		{
			I2C_Sts = I2Cx->ISR;
			errorCont++;
			if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
			{
				errorFlag=1;
				break; }
		}
		while (!(I2C_Sts&I2C_ISR_TC)); 							 			//Wait until the START condition is generated and address is sent to slave
		LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
	}
	if(!errorFlag) return 0;
	
	errorCont=0;
	
	for (char errcnt=0; errcnt<3; errcnt++)
	{
		errorFlag=0;
		LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
		LL_I2C_Disable(I2Cx);		//DISABLE the I2C channel
		RCC->APB1ENR&=~RCC_APB1ENR_I2C1EN; 	//DISABLE peripheral I2C1 clock
		
		//Generate a 5ms low pulse on SDA
		GPIOB->MODER&=~(0x0F<<12);
		GPIOB->MODER|=(0x05<<12);
		GPIOB->BSRR|=(0xC0);
		GPIOB->BRR|=(0x80);
		SysTick_Delay_us_CRC(5000);
		GPIOB->MODER&=~(0x0F<<12);
		I2C_Config_MPF4279x(I2Cx);
		LL_I2C_Enable(I2Cx);		//ENABLE the I2C channel
		SysTick_Delay_us_CRC(2000);
		
		if (no_stop)
		{
			//Set Slave Address and length of transmission:
			I2Cx->CR2&=~(I2C_CR2_SADD | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
			I2Cx->CR2|=((Address<<1) | (len<<I2C_CR2_NBYTES_Pos));
			//Set Register address low byte as first byte to transfer:
			I2Cx->TXDR=Register&0xFF;		//Write the register
			I2Cx->ICR|=I2C_ICR_NACKCF;
			LL_I2C_GenerateStartCondition(I2Cx); 
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}while (!(I2C_Sts&I2C_ISR_TXIS)); 							 			//Wait until the START condition is generated and address is sent to slave and ACK is received from slave
		}
		else
		{
			//Check if Acknoledge is returned from MPF4279x to verify it's in Active Mode; if not do a low pulse on SDA
			I2Cx->CR2&=~(I2C_CR2_SADD | I2C_CR2_RD_WRN | I2C_CR2_NBYTES_Msk);
			I2Cx->CR2|=((Address<<1) | (1<<I2C_CR2_NBYTES_Pos));
			I2Cx->TXDR=0;
			I2Cx->ICR|=I2C_ICR_NACKCF;
			LL_I2C_GenerateStartCondition(I2Cx); 
			do
			{
				I2C_Sts = I2Cx->ISR;
				errorCont++;
				if ((I2C_Sts&I2C_ISR_NACKF) || (errorCont>=I2CToutMPF4279x))
				{
					errorFlag=1;
					break; }
			}
			while (!(I2C_Sts&I2C_ISR_TC)); 							 			//Wait until the START condition is generated and address is sent to slave
			LL_I2C_GenerateStopCondition(I2Cx); //At the end, send a STOP.
		}
		errorCont=0;
		if(!errorFlag) return 0;
	}
	return 1;
}

//Generate us delay
static __inline void SysTick_Delay_us_CRC(volatile uint32_t microseconds)
{
  SysTick_Delay_Done = 0;
  SysTick->LOAD = (microseconds*(frequency/8)-5);
	SysTick->VAL = 0;
	SysTick->CTRL &=~ SysTick_CTRL_COUNTFLAG_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	// COUNTFLAG is a bit that is set to 1 when counter reaches 0.
	// It's automatically cleared when read.
	while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
	SysTick_Delay_Done = 1;
}
