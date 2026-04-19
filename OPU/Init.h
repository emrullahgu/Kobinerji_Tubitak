/************************************************************************************
*         File: Init.h																															*
*					Description: Contains the declaration for the functions defined in the 		*
*						Init.c file   																													*
*					CPU: STM32F03xx Medium density devices																		*
*					Project:	OPU, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Red Origin Technologies																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/
#include "stm32f0xx.h"                  // Device header
#include "stm32f0xx_ll_i2c.h"
#include "stm32f0xx_ll_rcc.h"
#include "math.h"

//Defined functions in Init.c
void Syst_Init(void);
void InitGPIO(void);
void InitTim3(void);
void I2C_Config_MPF4279x(I2C_TypeDef* I2Cx);
void I2C_Config_MP279x(I2C_TypeDef* I2Cx);
void I2C_Config_MP279x_8MHz(I2C_TypeDef* I2Cx);
void InitUART1(void);
void InitTim6(void);
void InitRTC(void);
void InitIWDG(void);
uint32_t SysTick_Delay_Init(void);
unsigned char Nop(void);
