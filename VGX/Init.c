/************************************************************************************
*         File: Init.c																															*
*					Description: Contains a basicly functions for peripherals initialization  *
*					CPU: STM32F03xx Medium density devices																		*
*					Project:VGX, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Emrullah GÜNAY — KOBİNERJİ A.Ş.																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/

#include "Init.h"    //Includes the declaration header
#include "BMS_MP279x.h"

/*Extern Variables*/
LL_I2C_InitTypeDef i2c;	//This is the i2c parameter configuration
extern unsigned char TxBuff[297], RxBuff[92];

/*System Init*/
void Syst_Init(void)
{
	RCC->APB1ENR|=RCC_APB1ENR_PWREN;
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	
	while(!(RCC->CR&RCC_CR_HSERDY)) RCC->CR|=RCC_CR_HSEON;
	
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_6);
  LL_RCC_PLL_Enable();
	
	while(!(RCC->CR&RCC_CR_PLLRDY)) RCC->CR|=RCC_CR_PLLON;
	
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	
	while((RCC->CFGR&RCC_CFGR_SWS)!=0x08) RCC->CFGR |= 0x02;
	
	RCC->CFGR3&=~RCC_CFGR3_USART1SW_Msk;
	RCC->CFGR3|=RCC_CFGR3_I2C1SW;
}

/*Init GPIOs Function*/
void InitGPIO(void)
{
	//Enable GPIO clocks (From APB2 bus..)
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN; //Alternate Function Clock Enable
	RCC->AHBENR|=RCC_AHBENR_GPIOAEN; //Port A Clock Enable
	RCC->AHBENR|=RCC_AHBENR_GPIOBEN; //Port B Clock Enable
	RCC->AHBENR|=RCC_AHBENR_GPIOCEN;	//Port C Clock Enable
	
	//PORT MAP OUTPUTS********************************************
	//Pin PA3 -> MP_GPIO1--------------------------------------
	GPIOA->MODER|=(1<<6);	  	 		//GPIOA, PA3, Output
	GPIOA->OSPEEDR|=(1<<6);				//Max. Speed 10Mhz.
	GPIOA->OTYPER&=~(1<<3); 		//GPIOA, PA3, Push-Pull
	
	//Pin PA4 -> MP_GPIO2-----------------------------------
	GPIOA->MODER|=(1<<8);  			  //GPIOA, PA4, Output
	GPIOA->OSPEEDR|=(1<<8);				//Max. Speed 10Mhz.
	GPIOA->OTYPER&=~(1<<4); 		//GPIOA, PA4, Push-Pull	

	//Pin PB0 -> MP_GPIO3/FAULT PIN----------------------------------
//	GPIOB->CRL|=1;  	 	//GPIOB, PB0, Output
//	GPIOB->CRL&=~(1<<2);  		//GPIOB, PB0, Push-Pull

	//Pin PA0_WAKEUP_FUNCTION----------------------------------	
    GPIOA->MODER &= ~GPIO_MODER_MODER0;  // GPIOA, PA0, Input mode
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0;  // No pull-up, pull-down
	
	//Pin PB1 -> MP_nSHDN------------------------------
	GPIOB->MODER|=(1<<2);  	//GPIOB, PB1, Output
	GPIOB->OSPEEDR|=(1<<2);				//Max. Speed 10Mhz.
	GPIOB->OTYPER&=~(1<<1);  	//GPIOB, PB1, Push-Pull
	
	//Pin PB3 -> MP_nSHDN------------------------------
	GPIOB->MODER|=(1<<6);  	//GPIOB, PB3, Output
	GPIOB->OSPEEDR|=(1<<6);				//Max. Speed 10Mhz.
	GPIOB->OTYPER&=~(1<<3);  	//GPIOB, PB3, Push-Pull
	
	//Pin PB4 -> MP_nSHDN------------------------------
	GPIOB->MODER|=(1<<8);  	//GPIOB, PB4, Output
	GPIOB->OSPEEDR|=(1<<8);				//Max. Speed 10Mhz.
	GPIOB->OTYPER&=~(1<<4);  	//GPIOB, PB4, Push-Pull
	#ifdef High_P
	//Pin PA5 -> MP_nSHDN_Gate-----------------------------------
	GPIOA->MODER|=(1<<10);                //GPIOA, PA5, Output
	GPIOA->OSPEEDR|=(1<<10);                //Max. Speed 10Mhz.
	GPIOA->OTYPER&=~(1<<5);         //GPIOA, PA5, Push-Pull    
	
	//Pin PA8 -> RS485_TXE_RXNE-----------------------------------
	GPIOA->MODER|=(1<<16);                //GPIOA, PA8, Output
	GPIOA->OSPEEDR|=(1<<16);                //Max. Speed 10Mhz.
	GPIOA->OTYPER&=~(1<<8);         //GPIOA, PA8, Push-Pull    
#endif

#ifdef High_P
    //Pin PC14 -> LED 0-----------------------------------------
    GPIOC->MODER|=(1<<28);           //GPIOC, PC14, Output
    GPIOC->OSPEEDR|=(1<<28);                //Max. Speed 10Mhz.
    GPIOC->OTYPER&=~(1<<14);        //GPIOC, PC14, Push-Pull
    
    //Pin PC15 -> LED 1-----------------------------------------
    GPIOC->MODER|=(1<<30);           //GPIOC, PC15, Output
    GPIOC->OSPEEDR|=(1<<30);                //Max. Speed 10Mhz.
    GPIOC->OTYPER&=~(1<<15);        //GPIOC, PC15, Push-Pull

    //Pin PA1 -> LED 3------------------------------------------
    GPIOA->MODER|=(1<<2);          //GPIOA, PA1, Output
    GPIOA->OSPEEDR|=(1<<2);                //Max. Speed 10Mhz.
    GPIOA->OTYPER&=~(1<<1);         //GPIOA, PA1, Push-Pull

    //Pin PA2 -> LED 4------------------------------------------
    GPIOA->MODER|=(1<<4);              //GPIOA, PA2, Output
    GPIOA->OSPEEDR|=(1<<4);                //Max. Speed 10Mhz.
    GPIOA->OTYPER&=~(1<<2);              //GPIOA, PA2, Push-Pull
#else
	//Pin PC15 -> LED 0-----------------------------------------
	GPIOC->MODER|=(1<<30);  	 	//GPIOC, PC15, Output
	GPIOC->OSPEEDR|=(1<<30);				//Max. Speed 10Mhz.
	GPIOC->OTYPER&=~(1<<15);  	  //GPIOC, PC15, Push-Pull

	//Pin PC14 -> LED 1-----------------------------------------
	GPIOC->MODER|=(1<<28);  	 	//GPIOC, PC14, Output
	GPIOC->OSPEEDR|=(1<<28);				//Max. Speed 10Mhz.
	GPIOC->OTYPER&=~(1<<14);  	  //GPIOC, PC14, Push-Pull

	//Pin PB13 -> LED 2-----------------------------------------
	GPIOC->MODER|=(1<<26);  	 	//GPIOC, PC13, Output
	GPIOC->OSPEEDR|=(1<<26);				//Max. Speed 10Mhz.
	GPIOC->OTYPER&=~(1<<13);  	  //GPIOC, PC13, Push-Pull

	//Pin PA11 -> LED 3------------------------------------------
	GPIOA->MODER|=(1<<22);  		//GPIOA, PA11, Output
	GPIOA->OSPEEDR|=(1<<22);				//Max. Speed 10Mhz.
	GPIOA->OTYPER&=~(1<<11); 		//GPIOA, PA11, Push-Pull

	//Pin PA12 -> LED 4------------------------------------------
	GPIOA->MODER|=(1<<24); 	 		//GPIOA, PA12, Output
	GPIOA->OSPEEDR|=(1<<24);				//Max. Speed 10Mhz.
	GPIOA->OTYPER&=~(1<<12);  			//GPIOA, PA12, Push-Pull
#endif

	//Pin PB9 -> FG_NRST------------------------------------------
	GPIOB->MODER|=(1<<18);  		//GPIOB, PB9, Output
	GPIOB->OSPEEDR|=(1<<18);				//Max. Speed 10Mhz.
	//GPIOB->CRH&=~(1<<7); 		//GPIOB, PB9, General Purpose
	GPIOB->OTYPER|=(1<<9); 		//GPIOB, PB9, Open drain
	
	//Pin PB5 -> FG_CE------------------------------------------
	GPIOB->MODER|=(1<<10);  		//GPIOB, PB5, Output
	GPIOB->OSPEEDR|=(1<<10);			//Max. Speed 10Mhz.
	GPIOB->OTYPER&=~(1<<5); 		//GPIOB, PB5, Push-Pull
	
	//PORT MAP INTERRUPTS---------------------------------------
	SYSCFG->EXTICR[0]|=1;  			//EXTI0, Pin PB0 -> GPIO3/Fault
	SYSCFG->EXTICR[0]|=(1<<8);  //EXTI2, Pin PB2 -> xALERT
	SYSCFG->EXTICR[2]|=1;				//EXTI8, Pin PB8 -> FG_IRQ
}
/*Enabe TIMER3 like a simple timer*/
void InitTim3(void)
{
	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;	//ENABLE peripheral Timer2 clock
	TIM3->DIER|=0x01; 	//Update Interrupt Enable (UIE)
	TIM3->PSC=4799;		//Timer Prescaler
	TIM3->CNT=0;
	TIM3->CR1|=1;
}

//This Functions configure the I2C channels
void I2C_Config_MPF4279x(I2C_TypeDef* I2Cx)
{
	//RCC->APB2ENR|=(0x01); //Alternate Function Clock Enable //Uncoment this line if GPIO are not inicialized
	RCC->APB1ENR|=RCC_APB1ENR_I2C1EN; 	//ENABLE peripheral I2C1 clock
	GPIOB->MODER|=(0x0A<<12);	 //Congiure pins PB6, PB7 as Alternate function.
	GPIOB->OTYPER|=(0x03<<6);		//Congiure pins PB6, PB7 as Open Drain.
	GPIOB->OSPEEDR|=(0x0F<<12);		//Configure PB6&7 as High Speed (50MHz)
	GPIOB->AFR[0]|=0x11000000;	//Cofigure PB6&7 as AF1 (I2C1); PB7->SDA, PB6->SCL
	
	i2c.PeripheralMode = LL_I2C_MODE_I2C;
	i2c.OwnAddrSize = LL_I2C_ADDRESSING_MODE_7BIT;
	i2c.OwnAddress1 = LL_I2C_OWNADDRESS1_7BIT;
	i2c.Timing =	0x2010091A; //0xF000FFFF; //~5kHz			//0xF000F8F9; //6kHz		//0x70002AFF; //20kHz		//0x9010DEFF; //10kHz		//0x20103EFD;	//50kHz			//0x2010091A; //Value calculated by the STM32CubeMX software to configure I2C Fast Mode at 400 kHz	with SYSCLK @ 48MHz
	i2c.TypeAcknowledge = I2C_CR2_NACK;
	
	LL_I2C_Init(I2Cx,&i2c);		//Init I2C with the given parameters
	LL_I2C_Enable(I2Cx);		//ENABLE the I2C chanel
}

void I2C_Config_MP279x(I2C_TypeDef* I2Cx)
{
	//RCC->APB2ENR|=(0x01); //Alternate Function Clock Enable //Uncoment this line if GPIO are not inicialized
	RCC->APB1ENR|=RCC_APB1ENR_I2C2EN; 	//ENABLE peripheral I2C2 clock
	LL_I2C_Disable(I2Cx);		//DISABLE the I2C chanel
	GPIOB->MODER|=(0x0A<<20);	 //Congiure pins PB10, PB11 as Alternate function.
	GPIOB->OTYPER|=(0x03<<10);		//Congiure pins PB10, PB11 as Open Drain.
	GPIOB->OSPEEDR|=(0x0F<<20);		//Configure PB10&11 as High Speed (50MHz)
	GPIOB->AFR[1]|=0x1100;	//Cofigure PB10&11 as AF1 (I2C1); PB7->SDA, PB8->SCL
		
	i2c.PeripheralMode = LL_I2C_MODE_I2C;
	i2c.OwnAddrSize = LL_I2C_ADDRESSING_MODE_7BIT;
	i2c.OwnAddress1 = LL_I2C_OWNADDRESS1_7BIT;
	i2c.Timing = 0x20100843; //Value calculated by the STM32CubeMX software to configure I2C Fast Mode at 200 kHz	with APB1 CLK @ 48MHz
	i2c.TypeAcknowledge = I2C_CR2_NACK;

	LL_I2C_Init(I2Cx,&i2c);		//Init I2C with the given parameters
	LL_I2C_Enable(I2Cx);		//ENABLE the I2C chanel
}

void I2C_Config_MP279x_8MHz(I2C_TypeDef* I2Cx)
{
	//RCC->APB2ENR|=(0x01); //Alternate Function Clock Enable //Uncoment this line if GPIO are not inicialized
	RCC->APB1ENR|=RCC_APB1ENR_I2C2EN; 	//ENABLE peripheral I2C2 clock
	LL_I2C_Disable(I2Cx);		//DISABLE the I2C chanel
	GPIOB->MODER|=(0x0A<<20);	 //Congiure pins PB10, PB11 as Alternate function.
	GPIOB->OTYPER|=(0x03<<10);		//Congiure pins PB10, PB11 as Open Drain.
	GPIOB->OSPEEDR|=(0x0F<<20);		//Configure PB10&11 as High Speed (50MHz)
	GPIOB->AFR[1]|=0x1100;	//Cofigure PB10&11 as AF1 (I2C1); PB7->SDA, PB8->SCL
		
	i2c.PeripheralMode = LL_I2C_MODE_I2C;
	i2c.OwnAddrSize = LL_I2C_ADDRESSING_MODE_7BIT;
	i2c.OwnAddress1 = LL_I2C_OWNADDRESS1_7BIT;
	i2c.Timing = 0x20000215; //Value calculated by the STM32CubeMX software to configure I2C Fast Mode at 100 kHz	with APB1 CLK @ 8MHz
	i2c.TypeAcknowledge = I2C_CR2_NACK;

	LL_I2C_Init(I2Cx,&i2c);		//Init I2C with the given parameters
	LL_I2C_Enable(I2Cx);		//ENABLE the I2C chanel
}

//This Function configures and enables a UART1
void InitUART1(void)
{
	//RCC->APB2ENR|=(0x01); //Alternate Function Clock Enable //Uncoment this line if GPIO are not inicialized
	RCC->APB2ENR|=RCC_APB2ENR_USART1EN; //Enable USART Clock
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; //DMA clock Enable
	
	GPIOA->MODER|=(0x0A<<18);	 //Congiure pins PA9, PA10 as Alternate function.
	#ifndef High_P
	GPIOA->OTYPER|=(0x03<<9);		//Congiure pins PA9, PA10 as Open Drain.
	#else
	GPIOA->OTYPER&=~(0x01<<9);        //Congiure pins PA9 as Push-Pull.
	GPIOA->OTYPER|=(0x01<<10);        //Congiure pins PA10 as Open Drain.
	GPIOA->PUPDR&=~(0x03<<20);        //Congiure pins PA10 with a pull-up.
	GPIOA->PUPDR|=(0x01<<20);        //Congiure pins PA10 with a pull-up.
	#endif
	GPIOA->OSPEEDR|=(0x03<<18);		//Configure PA9 as High Speed (50MHz)
	GPIOA->AFR[1]|=0x0110;	//Cofigure PA9&10 as AF1 (USART1); PA9->Tx, PA10->Rx
	
	USART1->BRR=0x340;			//Baud rate 115200: See Reference manual table 87 (with OVER8=1)
#ifdef High_P
	USART1->CR2&=~USART_CR2_ADD_Msk;
	USART1->CR2|=(0x0A<<USART_CR2_ADD_Pos);
#endif
	USART1->CR1=USART_CR1_TE			//Tranmistion Enable	
						| USART_CR1_RE			//Reception 	 Enable
						| USART_CR1_IDLEIE	//Idle line interrupt Enable
#ifdef High_P
						| USART_CR1_CMIE	//Idle line interrupt Enable
#endif
						| USART_CR1_UE			//UART Enable
						| USART_CR1_OVER8;	//Oversampling by 8 (allows to reduce the error on the Baudrate; See Reference manual table 87)
	USART1->CR3|=(USART_CR3_DMAR | USART_CR3_DMAT); 					//Enable DMA mode for USART1 (receive and trasmit)
	DMA1_Channel3 -> CCR &= ~(DMA_CCR_EN);//DMA_CCR5_EN);
	DMA1_Channel3 -> CPAR = (uint32_t) (&(USART1->RDR)); 			//DMA reads the received data from the data register of the USART1
  DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address 
	DMA1_Channel3 -> CNDTR = 0x5C;														//The receive buffer has a 4 byte size
	DMA1_Channel3 -> CCR |= DMA_CCR_MINC;										//DMA memory increment mode enabled
	DMA1_Channel3 -> CCR &= ~(DMA_CCR_MSIZE);								//Memory size: 8 bits
  DMA1_Channel3 -> CCR &= ~(DMA_CCR_PSIZE);								//Peripheral size: 8 bits
  DMA1_Channel3 -> CCR &= ~(DMA_CCR_DIR);									//Read from peripheral
	DMA1_Channel3 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 5
	DMA1_Channel2 -> CCR &= ~(DMA_CCR_EN);
	DMA1_Channel2 -> CPAR = (uint32_t) (&(USART1->TDR));				//DMA writes the buffer data to the data register of the USART1
  DMA1_Channel2 -> CMAR = (uint32_t) (TxBuff); 							//DMA reads the values in this buffer to the above address
	DMA1_Channel2 -> CNDTR = BT_LENGTH_TX;										//The transmit buffer has a byte size equals to the defined as BT_LENGTH_TX
	DMA1_Channel2 -> CCR |= DMA_CCR_MINC;										//DMA memory increment mode enabled
	DMA1_Channel2 -> CCR &= ~(DMA_CCR_MSIZE);								//Memory size: 8 bits
  DMA1_Channel2 -> CCR &= ~(DMA_CCR_PSIZE);								//Peripheral size: 8 bits
  DMA1_Channel2 -> CCR |= DMA_CCR_DIR;											//Read from memory
	USART1->ISR &= ~(USART_ISR_TC);															//Clear the TC bit in the UART_ISR register
	USART1->ICR|=USART_ICR_IDLECF;
}

//Enable TIMER6 like a simple timer for UART reception timeout
void InitTim6(void)
{
	RCC->APB1ENR|=RCC_APB1ENR_TIM6EN; //Enable peripheral Timer6 clock
	TIM6->DIER|=1; 					    			//Update Interrupt Enable (UIE)
	TIM6->PSC=479;										//Timer Prescaler
	TIM6->ARR=1000;		  							//Timer Preset 1000 units (10ms)
	TIM6->CR1|=0x01;									//Counter Enable
}

//This functions contains None. Is to put brakpoints in the debbuging session
unsigned char Nop(void)
{
	return 0;
}

//Inits RTC with 100ms of timebase.
void InitRTC(void)
{
	RCC->APB1ENR|=RCC_APB1ENR_PWREN;//(0x03<<27); 		//Enable the power and backup interface clocks by setting the PWREN and BKPEN
	PWR->CR|=PWR_CR_DBP;							//Enable access to the Backup registers and RTC
	RCC->BDCR|=RCC_BDCR_RTCSEL_HSE;	  		//Select RTC clock source: HSE oscillator clock divided by 32 used as RTC clock
	RCC->BDCR|=RCC_BDCR_RTCEN;			  		//Enable RTC clock
	
	RTC->WPR=0xCAU;
	RTC->WPR=0x53U;
	
	RTC->ISR |= RTC_ISR_INIT;
	while(!(RTC->ISR&RTC_ISR_INITF)); //Poll RTC_ISR_INITF, wait until its value goes to �1�

	RTC->PRER=0x007C00C7;
	
	RTC->CR &=~ RTC_CR_ALRAE;		//Reset RTC_CR_ALRAE to Disable Alarm A.
	while(!(RTC->ISR&RTC_ISR_ALRAWF)); //Poll RTC_ISR_ALRAWF, wait until its value goes to �1�
	
	RTC->CR |= RTC_CR_ALRAIE;
	
	RTC->ALRMAR |= (RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1);
	
	RTC->ALRMASSR=0x00000000;
	
	RTC->CR |= RTC_CR_ALRAE;			//ENABLE Alarm A and Alarm A interrupt

	RTC->DR=0x00202101;
	
	RTC->ISR &=~ RTC_ISR_INIT;
	RTC->ISR&=~RTC_ISR_ALRAF;				//Reset the RTC wkup timer flag
	
	EXTI->PR|=(1<<17);										//Clears any pending interrupt for EXTI17 (RTC Alarm)
	EXTI->IMR|=(1<<17);										//Unmask EXTI17 (RTC Alarm)
	EXTI->RTSR|=(1<<17);										//Set EXTI17 (RTC Alarm)
	
	RTC->WPR = 0xFE;
	RTC->WPR = 0x64;
}

//Inits IWDG
void InitIWDG(void)
{
	IWDG->KR=0x5555;							//This key give write acces to Prescaler (PR) and reload value (RLR) registers
	IWDG->PR=0x04;								//IWDG LSI 40kHz CLK prescaler (0x04 -> /64)
	IWDG->RLR=0x0753;							//IWDG Reload Value (WDG_PR=0x04 & WDG_RLR=0x753 -> if not reloaded reset in 3s)
	IWDG->KR=0xAAAA;							//This key is to reload the RLR value to the counter and prevent write acces to Prescaler (PR) and reload value (RLR) registers
	RCC->APB2ENR |= RCC_APB2ENR_DBGMCUEN;
	DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;						//Disable the counter while debugging 
}

//Inits us delay controller
uint32_t SysTick_Delay_Init(void) {
	SysTick->VAL = 0;
	SysTick->CTRL &=~ SysTick_CTRL_CLKSOURCE_Msk;		//SysTick source is SYSCLK/8
	return 1;
}


//ADC External NTC I�IN
	void ADC_Configuration(void) 
{
    // RCC (Reset and Clock Control) register'ini etkinlestir
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // GPIOA i�in
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // ADC i�in
    // GPIOA_MODER register'ini ayarla (PA6'i analog moduna ayarla)
    GPIOA->MODER |= GPIO_MODER_MODER6 | GPIO_MODER_MODER2;
    // ADC1 i�in AHB frekansini kullan
    //RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV1; // ADC1 i�in AHB frekansini b�l 1
    // ADC'nin kalibrasyonunu yap
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);  // Kalibrasyon bitene kadar bekle
    // ADC'yi etkinlestir
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // ADC'nin hazir oldugunu bekleyin
}
// ADC okuma fonksiyonu
uint16_t adc_pa7(void) 
{
    ADC1->CHSELR = 0; // T�m kanallari kapat
    ADC1->CHSELR |= ADC_CHSELR_CHSEL7; // Kanal 6'i se� (PA1)
    // ADC baslat
    ADC1->CR |= ADC_CR_ADSTART;
    // ADC'nin d�n�s�n� bekleyin
    while (!(ADC1->ISR & ADC_ISR_EOC));
    return ADC1->DR; // ADC verisini oku
}

uint16_t adc_pa5(void) 
{
    ADC1->CHSELR = 0; // T�m kanallari kapat
    ADC1->CHSELR |= ADC_CHSELR_CHSEL5; // Kanal 2'i se� (PA1)
    // ADC baslat
    ADC1->CR |= ADC_CR_ADSTART;
    // ADC'nin d�n�s�n� bekleyin
    while (!(ADC1->ISR & ADC_ISR_EOC));
    return ADC1->DR; // ADC verisini oku
}

