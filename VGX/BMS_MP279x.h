/************************************************************************************
*         File: BMS_MP279x.h																												*
*					Description: Contains pin mapping and BMS parameters										  *
*					CPU: STM32F03xx Medium density devices																		*
*					Project:VGX, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Emrullah GÜNAY — KOBİNERJİ A.Ş.																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/
#ifndef BMS_H_
#define BMS_H_

//Activation Functions. Comment if you want to disable any function
#define WDG_EN					//Activate Watchdog.
//#define Use_PreChg
//#define High_P
#define ADDRESS_MP279x				1
#define MP279x_AutoBalance
//#define Direct_Mode
#define PFET_Enable
#define ENABLE_CC

#define I2CToutMP279x				750
#define I2CToutMPF4279x			3000

#define SETMAX_NCELLS_SER		16

//Uncomment define to use MPF42791/3; Comment it to use MPF42790/2/5/7
#define FG_NewGen

#ifdef FG_NewGen
//Uncomment define to use MPF42791; Comment it to use MPF42793
//#	define MPF42791
#	ifndef MPF42791
#		define MPF42793
#	endif
#endif

//Uncoment to use MP2791; Comment to use MP2797
//#define MP2791

#define CELLS_MASK_6  	(0xFFC0)
#define CELLS_MASK_7  	(0xFF80)
#define CELLS_MASK_8  	(0xFF00)
#define CELLS_MASK_9  	(0xFE00)
#define CELLS_MASK_10  	(0xFC00)
#define CELLS_MASK_11  	(0xF800)
#define CELLS_MASK_12  	(0xF000)
#define CELLS_MASK_13  	(0xE000)
#define CELLS_MASK_14  	(0xC000)
#define CELLS_MASK_15  	(0x8000)
#define CELLS_MASK_16  	(0x0000)

#ifndef MP2791
//Select the number of cells of your battery: Uncomment the adequate number (Default=16)
//#define PACK_CELLS_SERIES_7
#define PACK_CELLS_SERIES_8
//#define PACK_CELLS_SERIES_9
//#define PACK_CELLS_SERIES_10
//#define PACK_CELLS_SERIES_11
//#define PACK_CELLS_SERIES_12
//#define PACK_CELLS_SERIES_13
//#define PACK_CELLS_SERIES_14
//#define PACK_CELLS_SERIES_15
//#define PACK_CELLS_SERIES_16

#ifdef PACK_CELLS_SERIES_7
#define CellsSeries	7
#elif defined PACK_CELLS_SERIES_8
#define CellsSeries	8
#elif defined PACK_CELLS_SERIES_9
#define CellsSeries	9
#elif defined PACK_CELLS_SERIES_10
#define CellsSeries	10
#elif defined PACK_CELLS_SERIES_11
#define CellsSeries	11
#elif defined PACK_CELLS_SERIES_12
#define CellsSeries	12
#elif defined PACK_CELLS_SERIES_13
#define CellsSeries	13
#elif defined PACK_CELLS_SERIES_14
#define CellsSeries	14
#elif defined PACK_CELLS_SERIES_15
#define CellsSeries	15
#else
#define CellsSeries	16
#endif

#define CellOV_Thres			36500			//Default: 42000		Units: 0.1mV
#define CellUV_Thres			20000			//Default: 30000		Units: 0.1mV
#define CellDead_Thres		18000			//Default: 20000		Units: 0.1mV
#define CellMSMT_Thres		3					//Default: 3				Units: 39.0625mV
#define PackUV_Thres			16000			//Default: 48800		Units: mV
#define PackOV_Thres			29200			//Default: 66800		Units: mV
#define OC_THRES_1				45000			//Default: 15000		Units: mA
#define OC_THRES_1_Dgl		20				//Default: 20				Units: ms
#define OC_THRES_2				52500			//Default: 15000		Units: mA
#define OC_THRES_2_Dgl		10				//Default: 20				Units: ms
#define COC_THRES					15000			//Default: 7500			Units: mA
#define COC_THRES_Dgl			20				//Default: 20				Units: ms
#define DSC_THRES					36000			//Default: 22500		Units: mA
#define DSC_THRES_Dgl			8					//Default: 1				Units: 200 us
#define CSC_THRES					36000			//Default: 9375			Units: mA
#define CSC_THRES_Dgl			8					//Default: 8				Units: 200 us
#define BAT_DOT_THRES			5500			//Default: 5800			Units: 0.01 �C
#define BAT_DUT_THRES			-1800			//Default: -1800		Units: 0.01 �C
#define BAT_COT_THRES			4300			//Default: 4300			Units: 0.01 �C
#define BAT_CUT_THRES			200				//Default: 200			Units: 0.01 �C
#define PCB_OT_THRES			8500			//Default: 8500			Units: 0.01 �C
#define Min_Bal_V					30000			//Default: 40000		Units: 0.1mV
#define Bal_Config				0x0B			//Default: 0x0B

#define Current_Gain			32767
#define Current_Offset		0

#define SBY_TIME					600				//Default: 600s.	Time in seconds before going to Stand By mode while I<SBY_I_THR
#define SBY_I_THR					4					//Default: 0. Current threshold to go to Stand By mode. Offset = 250 uV; LSB = 125uV

#define FAULTS_EN							0x1FFF	//Default: 0x1FFF
#define FAULTS_AUTORECOVERY		0x0283	//Default: 0x0283
#define FEATURES_EN						0x0173	//Default: 0x0175
#else
//Select the number of cells of your battery: Uncomment the adequate number (Default=16)
//#define PACK_CELLS_SERIES_7
//#define PACK_CELLS_SERIES_8
//#define PACK_CELLS_SERIES_9
//#define PACK_CELLS_SERIES_10
//#define PACK_CELLS_SERIES_11
//#define PACK_CELLS_SERIES_12
//#define PACK_CELLS_SERIES_13
#define PACK_CELLS_SERIES_14

#ifdef PACK_CELLS_SERIES_7
#define CellsSeries	7
#elif defined PACK_CELLS_SERIES_8
#define CellsSeries	8
#elif defined PACK_CELLS_SERIES_9
#define CellsSeries	9
#elif defined PACK_CELLS_SERIES_10
#define CellsSeries	10
#elif defined PACK_CELLS_SERIES_11
#define CellsSeries	11
#elif defined PACK_CELLS_SERIES_12
#define CellsSeries	12
#elif defined PACK_CELLS_SERIES_13
#define CellsSeries	13
#else
#define CellsSeries	14
#endif

#define CellOV_Thres			42000			//Default: 42000		Units: 0.1mV
#define CellUV_Thres			30000			//Default: 30000		Units: 0.1mV
#define CellDead_Thres		20000			//Default: 20000		Units: 0.1mV
#define CellMSMT_Thres		3					//Default: 3				Units: 39.0625mV
#define PackUV_Thres			42700			//Default: 42700		Units: mV
#define PackOV_Thres			58450			//Default: 58450		Units: mV
#define OC_THRES_1				15000			//Default: 15000		Units: mA
#define OC_THRES_1_Dgl		100				//Default: 100				Units: ms
#define OC_THRES_2				18000			//Default: 18000		Units: mA
#define OC_THRES_2_Dgl		20				//Default: 20				Units: ms
#define COC_THRES					7500			//Default: 7500			Units: mA
#define COC_THRES_Dgl			20				//Default: 20				Units: ms
#define DSC_THRES					22500			//Default: 22500		Units: mA
#define DSC_THRES_Dgl			1					//Default: 1				Units: 200 us
#define CSC_THRES					9375			//Default: 9375			Units: mA
#define CSC_THRES_Dgl			8					//Default: 8				Units: 200 us
#define BAT_DOT_THRES			5800			//Default: 5800			Units: 0.01 �C
#define BAT_DUT_THRES			-1800			//Default: -1800		Units: 0.01 �C
#define BAT_COT_THRES			4300			//Default: 4300			Units: 0.01 �C
#define BAT_CUT_THRES			200				//Default: 200			Units: 0.01 �C
#define PCB_OT_THRES			8500			//Default: 8500			Units: 0.01 �C
#define Min_Bal_V					40000			//Default: 40000		Units: 0.1mV
#define Bal_Config				0x0B			//Default: 0x0B

#define Current_Gain			28574
#define Current_Offset		45

#define SBY_TIME					600				//Default: 600s.	Time in seconds before going to Stand By mode while I<SBY_I_THR
#define SBY_I_THR					4					//Default: 0. Current threshold to go to Stand By mode. Offset = 250 uV; LSB = 125uV

#define FAULTS_EN							0x1FFF	//Default: 0x1FFF
#define FAULTS_AUTORECOVERY		0x0283	//Default: 0x0283
#define FEATURES_EN						0x0173	//Default: 0x0175
#endif

//Port Map Outputs
#define SET_GPIO1 			GPIOA->BSRR|=(1<<3) //Pin PA3, Set
#define RSET_GPIO1	 		GPIOA->BRR|=(1<<3) //Pin PA3, Reset.

#define SET_GPIO2 			GPIOA->BSRR|=(1<<4) //Pin PA4, Set
#define RSET_GPIO2 			GPIOA->BRR|=(1<<4) //Pin PA4, Reset.

#ifdef Direct_Mode
#define ENABLE_DCH 			SET_GPIO1
#define DISABLE_DCH 		RSET_GPIO1
#define ENABLE_CHG 			SET_GPIO2
#define DISABLE_CHG 		RSET_GPIO2
#else
#define ENABLE_FETs			GPIOA->BSRR|=(3<<3)
#define DISABLE_FETs		GPIOA->BRR|=(3<<3)
#endif

#ifdef Use_PreChg
#define SET_PRECHARGE			PreChg_State=1
#define RSET_PRECHARGE		PreChg_State=2
#endif

#define SET_MP_nSHDN 		GPIOB->BSRR|=(1<<1) //Pin PB1, Set
#define RSET_MP_nSHDN 	GPIOB->BRR|=(1<<1) //Pin PB1, Reset.

#ifdef High_P
#define SET_MP_nSHDN_Gate 		GPIOA->BSRR|=(1<<5) //Pin PA5, Set
#define RSET_MP_nSHDN_Gate 		GPIOA->BRR|=(1<<5) //Pin PA5, Reset.
#endif


#ifdef High_P
#define SET_LD0 			GPIOC->BSRR|=(1<<14) //Pin PC14, Set
#define RSET_LD0 			GPIOC->BRR|=(1<<14) //Pin PC14, Reset.

#define SET_LD1 			GPIOC->BSRR|=(1<<15) //Pin PC15, Set
#define RSET_LD1 			GPIOC->BRR|=(1<<15) //Pin PC15, Reset.

#define SET_LD3 			GPIOA->BSRR|=(1<<1) //Pin PA1, Set
#define RSET_LD3 			GPIOA->BRR|=(1<<1) //Pin PA1, Reset.

#define SET_LD4 			GPIOA->BSRR|=(1<<2) //Pin PA2, Set
#define RSET_LD4 			GPIOA->BRR|=(1<<2) //Pin PA2, Reset.

#define SET_LD2
#define RSET_LD2
#else
#define SET_LD0 			GPIOC->BSRR|=(1<<15) //Pin PC15, Set
#define RSET_LD0 			GPIOC->BRR|=(1<<15) //Pin PC15, Reset.

#define SET_LD2 			GPIOC->BSRR|=(1<<14) //Pin PC14, Set
#define RSET_LD2 			GPIOC->BRR|=(1<<14) //Pin PC14, Reset.

#define SET_LD1 			GPIOC->BSRR|=(1<<13) //Pin PC13, Set
#define RSET_LD1 			GPIOC->BRR|=(1<<13) //Pin PC13, Reset.

#define SET_LD4 			GPIOA->BSRR|=(1<<11) //Pin PA11, Set
#define RSET_LD4			GPIOA->BRR|=(1<<11) //Pin PA11, Reset.

#define SET_LD3 			GPIOA->BSRR|=(1<<12) //Pin PA12, Set
#define RSET_LD3			GPIOA->BRR|=(1<<12) //Pin PA12, Reset.
#endif

#define SET_FG_NRST 			GPIOB->BSRR|=(1<<9) //Pin PB9, Set
#define RSET_FG_NRST	 		GPIOB->BRR|=(1<<9) //Pin PB9, Reset.

#define SET_FG_EN 				GPIOB->BSRR|=(1<<5) //Pin PB5, Set
#define RSET_FG_EN 				GPIOB->BRR|=(1<<5) //Pin PB5, Reset.

//Port Map Inputs
#define RD_GPIO1						((GPIOA->IDR & 0x0008)>>3)  	//Read the Status of Pin PA3
#define RD_GPIO2						((GPIOA->IDR & 0x0010)>>4)  	//Read the Status of Pin PA4
#ifdef Direct_Mode
#define DSC_Status					RD_GPIO1
#define CHG_Status					RD_GPIO2
#else
#define FETs_PIN_Status			RD_GPIO1
#endif
#define RD_GPIO3						(GPIOB->IDR & 0x0001)   			//Read the Status of Pin PB0
#define RD_FAULT						RD_GPIO3
#define RD_xALERT						((GPIOB->IDR & 0x0004)>>2)   //Read the Status of Pin PB2
#define FG_IRQ							((GPIOB->IDR & 0x0100)>>8)		//Read the Status of Pin PB8

#ifdef High_P
#define LED0_ON							((GPIOC->IDR & 0x4000)>>14)  //Read the Status of Pin PC14
#define LED1_ON							((GPIOC->IDR & 0x8000)>>15)  //Read the Status of Pin PC15
#else
#define LED0_ON							((GPIOC->IDR & 0x8000)>>15)  //Read the Status of Pin PC15
#endif
//System definition
#define BT_LENGTH_TX				297					 		//Measurement interval. Defines the sampling interval between ADC measurements.
#define START_TIME					20000								//Min current, in bits, that it's considerated 0. (under <100mA). COMMENT FOR DISABLE THIS FUNCTION.

//Interrupt Priorities: 0 = Highest Priority
#define EXTI0_IP						0		//Priority for FAULT Pin Interrupt
#define EXTI2_IP						1		//Priority for xALERT Pin Interrupt
#define UART_IP							3		//Priority for UART reception Interrupt
#define TIM6_IP							4		//Priority for UART reception Timeout Interrupt
#define TIM3_IP							6		//Priority for Start Wait time
#define RTC_IP							2		//Priority for RTC execution
#define EXTI9_5_IP					5		//Priority for the FG_IT pin

//Watchdog Configuration values
#ifdef WDG_EN
#define WDG_KEY_RELOAD			0xAAAA		//Key to return the counter to the reload value
#define WDG_KEY_START				0xCCCC		//Key to start the watchdog counter
#define IWDG_RELOAD_TIME		6250			//Reload IWDG each 5s when in sleep mode using TIM3
#endif

#define FL_Config_ADDRESS					0x0800F800

#ifdef High_P
#define I_SENSE_R_DEF							2
#else
#define I_SENSE_R_DEF							2
#endif

#define I_SENSE_R									(IsenseR*0.025)

#define R_TEMP_MEAS_PCB						10000			//Value in bits (LSb -> 62.5 uV) of the voltage reference for the temperature measurement circuit
#define BETA_NTC_PCB							3380			//Beta parameter value of the PCB NTC for temperature approximation

#define R_TEMP_MEAS_BAT						10000			//Value in Ohms of the resistor in the Battery temperature measurement circuit
#define BETA_NTC_BAT							3380			//Beta parameter value of the Battery NTC for temperature approximation

#define BMSData_size			sizeof(BMS_REG)
#define Config_size				sizeof(BMS_Configs)

//UART Modes
typedef enum
{
	RxMODE,
	TxMODE
}UART_Modes;

//CC Status:
typedef struct __attribute__((packed))
{
	unsigned CC_Cntr: 30;
	unsigned CC_Running: 1;
	unsigned CC_OverFlow: 1;
} Def_CC_Status;

//BMS states definition
typedef enum
{
	WAKE_UP,
	WAKE_UP_CHG,
	CHG,
	DCHG,
	STAND_BY,
	ALARM,
	SAFE,
	RESET_ALARM,
	AUTO_RECOVERY
}states;

//Alarms Indication
typedef struct __attribute__((packed))
{
	unsigned OVC: 1;
	unsigned UVC: 1;
	unsigned DeadC: 1;
	unsigned OVP: 1;
	unsigned UVP: 1;
	unsigned COC: 1;
	unsigned DOC: 1;
	unsigned CSC: 1;
	unsigned DSC: 1;
	unsigned Tchip:1;
	unsigned BatOT:1;
	unsigned BatUT:1;
	unsigned TPCB:1;
	unsigned OW:1;
	unsigned MSMT:1;
	unsigned FET_Drv:1;
	unsigned ChipSystemError:1;
	unsigned ChipSenseError:1;
} AlarmHandlerBit;

//Alarms definition
typedef enum
{
	CELL_OV,
	CELL_UV,
	CELL_DEAD,
	CELL_MSMT,
	PACK_OV,
	PACK_UV,
	COC,
	CSC,
	DOC,
	DSC,
	BAT_OT,
	BAT_UT,
	CHIP_TEMP,
	PCB_TEMP,
	OPEN_W,
	FET_DRIVER,
	CHIP_ERROR,
	CHIP_SENSE_ERROR
}alarms;
//BMSData Registers
typedef struct __attribute__((packed))
{
	uint16_t VTOP;										//2
	int32_t ITOP;											//4
	int32_t CC;												//4
	Def_CC_Status CC_Status;					//4
	uint16_t PACKV;										//2
	int16_t TempPCB;									//2
	int16_t TempIC;										//2
	states BMSstatus;									//1
	uint8_t AFE_Status;								//1
	AlarmHandlerBit BMSAlarms;				//3
	AlarmHandlerBit RT_Alarms;				//3
	uint16_t CellsUVReg;							//2
	uint16_t CellsOVReg;							//2
	uint16_t CellsDeadReg; 						//2
	uint16_t CellsMSMTReg; 						//2
	uint16_t CellsMSMT_Status;				//2
	uint8_t NTC_Status;								//1
	uint8_t OpenWireDet[3];						//3
	uint8_t FETs_Status; 							//1
	uint8_t SelfStatus;								//1
	uint16_t LC_Status;								//2
	uint16_t Bal_Status;							//2
	uint32_t VScan_Int_Counter;				//4
	uint16_t WDT_Int_Counter;					//2
	uint16_t AFE_Mode_Int_Counter;		//2
	uint16_t Pack_I_Int_Counter;			//2
	uint16_t Conn_Int_Counter;				//2
} BMS_REG;

//BMS Configuration Registers
typedef struct __attribute__((packed)) 
{
	uint8_t NumofSeries;						//1
	uint16_t Cell_UV;								//2
	uint8_t CellUV_Hyst;						//1
	uint8_t CellUV_Dgl;							//1
	uint16_t Cell_OV;								//2
	uint8_t CellOV_Hyst;						//1
	uint8_t CellOV_Dgl;							//1
	uint16_t Cell_Dead;							//2
	uint8_t CellDead_Dgl;						//1
	uint8_t Cell_MSMT;							//1
	uint8_t CellMSMT_Dgl;						//1
	uint16_t Pack_UV;								//2
	uint8_t PackUV_Hyst;						//1
	uint8_t PackUV_Dgl;							//1
	uint16_t Pack_OV;								//2
	uint8_t PackOV_Hyst;						//1
	uint8_t PackOV_Dgl;							//1
	uint16_t Pack_DOC1;							//2
	uint16_t PackDOC1_Dgl;					//2
	uint16_t Pack_DOC2;							//2
	uint16_t PackDOC2_Dgl;					//2
	uint16_t Pack_COC;							//2
	uint16_t PackCOC_Dgl;						//2
	uint16_t Pack_DSC;							//2
	uint8_t PackDSC_Dgl;						// Hi
	uint16_t Pack_CSC;							//2
	uint8_t PackCSC_Dgl;						//1
	int16_t Bat_DCH_OT;							//2
	int16_t Bat_CHG_OT;							//2
	int16_t Bat_DCH_UT;							//2
	int16_t Bat_CHG_UT;							//2
	uint8_t Cells_NTC_Config;				//1
	uint16_t PCB_OT;								//2
	uint8_t PCBOT_Hyst;							//1
	uint16_t I_Gain;								//2
	int16_t I_Offset;								//2
	uint16_t BetaNTCBat;						//2
	uint8_t BalConfig;							//1
	uint16_t MinBalV;								//2
	uint16_t Sby_Time;							//2
	uint8_t Sby_I_Thr;							//1
	
	uint16_t Faults_Enable;					//2
	//b0: Cell UV EN; Def=1
	//b1: Cell OV EN; Def=1
	//b2: Cell Dead EN; Def=1
	//b3: Pack UV EN; Def=1
	//b4: Pack OV EN; Def=1
	//b5: DOC1 EN; Def=1
	//b6: DOC2 EN; Def=1
	//b7: COC EN; Def=1
	//b8: DSC EN; Def=1
	//b9: CSC EN; Def=1
	//b10: Cell OT EN; Def=1
	//b11: Cell UT EN; Def=1
	//b12: PCB OT EN; Def=1
	//b13: Cell MSMT EN; Def=0
	//b14: -
	//b15: -
	//Def=0b0001 1111 1111 1111 = 0x1FFF
	uint16_t Faults_AutoRecovery;		//2
	//b0: Cell UV AutoRecovery EN; Def=1
	//b1: Cell OV AutoRecovery EN; Def=1
	//b2: DOC1 AutoRecovery EN; Def=0
	//b3: DOC2 AutoRecovery EN; Def=0
	//b4: COC AutoRecovery EN; Def=0
	//b5: DSC AutoRecovery EN; Def=0
	//b6: CSC AutoRecovery EN; Def=0
	//b7: Cell OTUT AutoRecovery EN; Def=1
	//b8: Reserved; Def=0 (Not used: Reserved in case we want to differentiate between CHG and DCH OT/UT AutoRecovery Enable)
	//b9: PCB OT AutoRecovery EN; Def=1
	//b10: -
	//b11: -
	//b12: -
	//b13: -
	//b14: -
	//b15: -
	//Def=0b0000 0010 1000 0011 = 0x0283
	
	uint16_t Features_Enable;				//2
	//b0: OWD EN; Def=1
	//b1: Chgr Det; Def=1
	//b2: Load Det; Def=0
	//b3: Auto Sleep; Def=0
	//b4: PFET Sby EN; Def=1
	//b5: WDT Rst EN; Def=1
	//b6: ADC Self Test EN; Def=1
	//b7: Bal EN; Def=0
	//b8: SC&OC during SAFE EN; Def=1
	//b9: AutoStart CC after Reset/PowerUp EN; Def=0
	//b10: -
	//b11: -
	//b12: -
	//b13: -
	//b14: -
	//b15: -
	//Def=0b0000 0001 0111 0011 = 0x0173
	
} BMS_Configs;

#endif /* BMS_H_ */
