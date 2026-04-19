/************************************************************************************
*         File: maim.c																															*
*					Description: Contains a main functions																	  *
*					CPU: STM32F03xx Medium density devices																		*
*					Project:VGX, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Emrullah GÜNAY — KOBİNERJİ A.Ş.																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/

#include "Init.h"
#include "MP279x.h"
#include "BMS_MP279x.h"
#include "MPF4279x.h"
#include "FLASH_Config.h"

//Function declarations**************************************************************
static __inline void SysTick_Delay_us(volatile uint32_t microseconds);
void SendFGDataMPF4279x(void);
void ReadFGResultsMPF4279x(void);
void Fault_Handle(void);
void xALARM_Handle(void);
void InitConfiguration(void);
void MP279xCInit(void);
void MP279xCReConfig(void);
void MP279xCConfigCalc(void);
uint8_t Calc_OC_SC_Lim (uint16_t conf_lim, uint32_t res_chng, uint16_t res_basic_red, uint16_t res_ext_red);
void ReadADCResults(void);
void SaveConfigtoFLASH(void);
void BMSAlarmHandler(alarms alarm);
void ResetAlarms(void);
void CheckUARTRx(void);
void Read_UART(void);
void UARTReadResponse(void);
void SendMeasVFG(void);
void ReadRsltsVFG(void);
void ADC_Configuration(void);
float ADC_to_Temperature(float adcValue);



// Sabitler
#define R1 10000.0    // 10k NTC direnci
#define BETA 3950.0   // NTC sensorunun Beta degeri
uint16_t adc_pa7(void);
uint16_t adc_pa5(void);

float heaterNtc = 0, heaterNtcCelcius = 0, totalHeaterNtc= 0;
float externalNtc = 0, externalNtcCelcius = 0, totalExternalNtc= 0;
float avarageCardNtc = 0;


int ntcC = 0;
int heaterOn = 0;  // heater durumunu izlemek icin flag
int fanOn = 0;  // fan durumunu izlemek icin flag

//Global Varaiables declarartion*****************************************************

//State variables:
unsigned char start=0, Wake_up_CHG_done, Wake_up_DSC_done, Reset_Alarms_done, Tim3first=0, StandByFlag, SafeFlag,
		FirstSample=0, DefaultConfig=0, ITCount=0, ADCNew=0, Fault, xALARM_Pending, RTC_IT=0, RTC_IT2= 0, Reconfig=0,
		Sby_Exit=0, DisableSby=0, frequency=48, LD_CH_Det, OWD, New_Meas, SysTick_Delay_Done, PFET_On, ADC_Req=0, ADC_WDG=0,
		DOC_AutoRec=0, DSC_AutoRec=0, COC_AutoRec=0, CSC_AutoRec=0;

#ifdef Use_PreChg
unsigned char PreChg_State=0;
#endif
uint32_t xALARMIT=0, GPIO3IT=0, ErrorCount=0, Interrupts, Faults, SbyCounter=0, BalCounter=0, Reset_Flags;

//Comms auxiliar variables:
unsigned char I2Cerror=0, MP279x_I2C_Busy=0, CommError,
		Read=1, Write=0, Register=0x5D, DataRx[2]={0,0}, WDT_STS[2]={0,0}, DataTx[2]={0,0};

//Auxiliar Measurement Variables:
uint8_t MinVID, MaxVID, IsenseR;
uint16_t CellsBalance=0, VBatRdiv, V3P3, V1P8, V5, MinVoltage, MaxVoltage, *CellsVoltage;
uint32_t CellsOVUV=0, CELLS_MASK=0;
int32_t ITOP_Temp;

//FG Variables:
uint8_t FirstSoC=0, FG_In_Send_Pend=0, FGReady=0, FG_Cntr, FG_Ts=40, FG_IT;
uint32_t Last_FG_IT;

//Coloumb Counter Variables:
uint8_t CCCount=0;
int32_t CC_Accum=0;
int64_t CC_BMS_Accum=0;
uint32_t CC_BMS_Cntr=0;

//Reset Cell Dead Variable
uint8_t CellDead_CPending=0;

//Reset faults in safe mode Variables
uint8_t CellMask_Fail_OV;
uint8_t CellMask_Fail_UV;
uint8_t CellMask_Fail_DeadC;
uint8_t CellMask_Fail_MSMT;

//Add here temporary debugging variables:
uint32_t FG_CC_Cntr=0;

//Register Structures:
Meas_REG Measurements;		//Measurements for FG
def_fg_out ResultsFG;		//FG results
BMS_REG BMSData;			//BMS Data
BMS_Configs Config;			//BMS Configuration

//Internal structure for MP279x registers:
MP279xConfig_REGs MP279xConfig;

//Comms auxiliar variables:
unsigned char TxBuff[fg_out_size+10], RxBuff[92+2] __attribute__((aligned (4))), CmdMsg=0x01, Tx_ACK[10]={0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A},
		Tx_NACK[10]={0x7E,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x0A}, *MemMapAddr, RXcont=0;
	
#ifdef High_P
#	ifdef MP2791
		unsigned char ID_BMS[2] = {0x91, 0x01};
#	else
		unsigned char ID_BMS[2] = {0x97, 0x01};
#	endif
#else
#	ifdef MP2791
		unsigned char ID_BMS[2] = {0x91, 0x00};
#	else
		unsigned char ID_BMS[2] = {0x97, 0x00};
#	endif
#endif
		
const unsigned short ID_DONGLE = 0x0BA0;			//This code means the GUI is talking with the MCU directly with a transparent USB to UART dongle, not the MPS EVKT-USB_I2C/UART Dongle
unsigned short UART_DataLength=0;
unsigned long CRCInputUART=0, *RxAddress=(unsigned long *)RxBuff;
UART_Modes UART_Mode;
		
// Virtual FG structure declaration 
typedef struct __attribute__((packed))
{
	unsigned VFG_En : 1;
	unsigned VFG_IT_Running : 1;
	unsigned VFG_IT_Done : 1;
	unsigned VFG_Reading : 1;
	unsigned VFG_First : 1;
	unsigned long VFG_Last_IT;
}VirtualFG_Struct;

VirtualFG_Struct VirtualFG = 
{
	.VFG_En = 0,
	.VFG_IT_Running = 0,
	.VFG_IT_Done = 0,
	.VFG_Reading = 0,
	.VFG_First = 0,
	.VFG_Last_IT = 0
};

Meas_REG MeasurementsVFG;		//Measurements for VFG

#define Def_Addr					(unsigned char*) 0x20000000	

typedef struct __attribute__((packed))
{
	uint16_t Address;
	uint16_t size;
	uint8_t* Mem_Address;
	unsigned Read				:1;
	unsigned Write			:1;
	unsigned FG					:1;
	unsigned Command		:1;
	unsigned Special		:1;
}Reg_struct;

#ifdef MPF42791
#	define Reg_Struct_Count 	70
	Reg_struct Reg_Map_info[Reg_Struct_Count] = {
	//FG_RegMap:
	{0x0000, Measurements_size, (uint8_t*) &Measurements, 1, 1, 0, 0, 1},
	{0x0200, 0x80, Def_Addr, 1, 1, 1, 0, 0},
	{0x02A0, 0x40, Def_Addr, 1, 1, 1, 0, 0},
	{0x0300, 0x40, Def_Addr, 1, 1, 1, 0, 0},
	{0x0360, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0370, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0380, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0390, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x03A0, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x03B0, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x03C0, 0x20, Def_Addr, 1, 1, 1, 0, 0},
	{0x03F0, 0x01, Def_Addr, 0, 1, 1, 0, 0},
	{0x0400, fg_out_size, (uint8_t*) &ResultsFG, 1, 1, 0, 0, 1},
	{0x0C00, LTL_REG_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x0F00, FG_Status_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x1000, fgset_main_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1040, fgset_en_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1080, fgset_intr_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x10C0, fgset_vbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1100, fgset_ibat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1140, fgset_rbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x11C0, fgset_Tbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1200, fgset_nbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1240, 7, Def_Addr, 1, 1, 1, 0, 0},
	{0x1280, fgset_load_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x12C0, fgset_chgr_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1300, fgset_empty_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1340, fgset_full_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1380, fgset_ichg_cc_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x13C0, fgset_ichg_end_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1400, fgset_vchg_cv_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1440, fgset_idis_avg_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1480, fgset_idis_end_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x14C0, fgset_soh_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1500, 9, Def_Addr, 1, 1, 1, 0, 0},
	{0x1540, fgset_resr_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1580, fgset_rcxn_cells_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x15C0, fgset_hconv_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x2000, 46, Def_Addr, 1, 1, 1, 0, 0},
	{0x2080, 34, Def_Addr, 1, 1, 1, 0, 0},
	{0x2100, 30, Def_Addr, 1, 1, 1, 0, 0},
	{0x2200, 4920, Def_Addr, 1, 1, 1, 0, 0},
	{0x4000, LEDs_REG_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x4100, I2Cset_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x4200, I2Csts_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x5000, fgcust_pass_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x6000, 9, Def_Addr, 1, 0, 1, 0, 0},
	{0x6100, 13, Def_Addr, 1, 0, 1, 0, 0},
	{0x6200, 32, Def_Addr, 1, 0, 1, 0, 0},
	{0x7FFF, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFE, 1, Def_Addr, 0, 1, 1, 1, 1},
	{0x7FFD, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFC, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFB, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFA, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FF9, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FF8, 1, Def_Addr, 0, 1, 1, 1, 0},
	//BMS_RegMap:
	{0x7F00, BMSData_size, (uint8_t*) &BMSData, 1, 0, 0, 0, 0},
	{0x7F80, Config_size+1, (uint8_t*) &Config, 1, 1, 0, 0, 0},
	{0x7FE0, 2, (uint8_t*) ID_BMS, 1, 0, 0, 0, 0},
	{0x7FE2, 5, (uint8_t*) &VirtualFG, 1, 1, 0, 0, 1},
	{0x7FE8, 2, (uint8_t*) &ID_DONGLE, 1, 0, 0, 0, 0},
	{0x7FEE, 1, (uint8_t*) &IsenseR, 1, 1, 0, 0, 1},
	{0x7FF0, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF1, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF2, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF3, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF4, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF5, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF6, 1, Def_Addr, 0, 1, 0, 1, 1}};
#elif defined MPF42793
#	define Reg_Struct_Count 	72
	Reg_struct Reg_Map_info[Reg_Struct_Count] = {
	//FG_RegMap:
	{0x0000, Measurements_size, (uint8_t*) &Measurements, 1, 1, 0, 0, 1},
	{0x0200, 0x80, Def_Addr, 1, 1, 1, 0, 0},
	{0x02A0, 0x40, Def_Addr, 1, 1, 1, 0, 0},
	{0x0300, 0x40, Def_Addr, 1, 1, 1, 0, 0},
	{0x0360, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0370, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0380, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0390, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x03A0, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x03B0, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x03C0, 0x20, Def_Addr, 1, 1, 1, 0, 0},
	{0x03F0, 0x01, Def_Addr, 0, 1, 1, 0, 0},
	{0x0400, fg_out_size, (uint8_t*) &ResultsFG, 1, 1, 0, 0, 1},
	{0x0C00, LTL_REG_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x0F00, FG_Status_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x1000, fgset_main_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1040, fgset_en_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1080, fgset_intr_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x10C0, fgset_vbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1100, fgset_ibat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1140, fgset_rbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x11C0, fgset_Tbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1200, fgset_nbat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1240, 7, Def_Addr, 1, 1, 1, 0, 0},
	{0x1280, fgset_load_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x12C0, fgset_chgr_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1300, fgset_empty_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1340, fgset_full_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1380, fgset_ichg_cc_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x13C0, fgset_ichg_end_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1400, fgset_vchg_cv_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1440, fgset_idis_avg_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1480, fgset_idis_end_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x14C0, fgset_soh_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1500, 9, Def_Addr, 1, 1, 1, 0, 0},
	{0x1540, fgset_resr_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1580, fgset_rcxn_cells_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x15C0, fgset_hconv_lrn_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x2000, 48, Def_Addr, 1, 1, 1, 0, 0},
	{0x2100, 62, Def_Addr, 1, 1, 1, 0, 0},
	{0x21F0, 1, Def_Addr, 1, 1, 1, 0, 0},
	{0x2200, 4096, Def_Addr, 1, 1, 1, 0, 0},
	{0x3200, 4096, Def_Addr, 1, 1, 1, 0, 0},
	{0x4200, 4096, Def_Addr, 1, 1, 1, 0, 0},
	{0x5300, LEDs_REG_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x5400, I2Cset_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x5500, I2Csts_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x5800, fgcust_pass_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x6000, 9, Def_Addr, 1, 0, 1, 0, 0},
	{0x6100, 13, Def_Addr, 1, 0, 1, 0, 0},
	{0x6200, 32, Def_Addr, 1, 0, 1, 0, 0},
	{0x7FFF, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFE, 1, Def_Addr, 0, 1, 1, 1, 1},
	{0x7FFD, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFC, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFB, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFA, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FF9, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FF8, 1, Def_Addr, 0, 1, 1, 1, 0},
	//BMS_RegMap:
	{0x7F00, BMSData_size, (uint8_t*) &BMSData, 1, 0, 0, 0, 0},
	{0x7F80, Config_size+1, (uint8_t*) &Config, 1, 1, 0, 0, 0},
	{0x7FE0, 2, (uint8_t*) ID_BMS, 1, 0, 0, 0, 0},
	{0x7FE2, 5, (uint8_t*) &VirtualFG, 1, 1, 0, 0, 1},
	{0x7FE8, 2, (uint8_t*) &ID_DONGLE, 1, 0, 0, 0, 0},
	{0x7FEE, 1, (uint8_t*) &IsenseR, 1, 1, 0, 0, 1},
	{0x7FF0, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF1, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF2, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF3, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF4, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF5, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF6, 1, Def_Addr, 0, 1, 0, 1, 1}};
#else
#	define Reg_Struct_Count 	50
	Reg_struct Reg_Map_info[Reg_Struct_Count] = {
	//FG_RegMap:
	{0x0000, Measurements_size, (uint8_t*) &Measurements, 1, 1, 0, 0, 1},
	{0x0200, 0x40, Def_Addr, 1, 1, 1, 0, 0},
	{0x0280, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0290, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x02A0, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x02B0, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x02C0, 0x02, Def_Addr, 1, 1, 1, 0, 0},
	{0x0300, 0x01, Def_Addr, 0, 1, 1, 0, 0},
	{0x0400, fg_out_size, (uint8_t*) &ResultsFG, 1, 1, 0, 0, 0},
	{0x0C00, LTL_REG_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x0F00, FG_Status_size, Def_Addr, 1, 0, 1, 0, 0},
	{0x1000, fgset_main_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1100, fgset_en_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1200, fgset_bat_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1300, fgset_vm_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1400, fgset_empty_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1500, fgset_full_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1600, fgset_chg_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1700, fgset_dis_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1800, fgset_soh_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x1900, 7, Def_Addr, 1, 1, 1, 0, 0},
	{0x1A00, fgset_esr_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x2000, 81, Def_Addr, 1, 1, 1, 0, 0},
	{0x2100, 4920, Def_Addr, 1, 1, 1, 0, 0},
	{0x4000, LEDs_REG_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x4100, I2Cset_size, Def_Addr, 1, 1, 1, 0, 0},
	{0x6000, 9, Def_Addr, 1, 0, 1, 0, 0},
	{0x6100, 13, Def_Addr, 1, 0, 1, 0, 0},
	{0x6200, 32, Def_Addr, 1, 0, 1, 0, 0},
	{0x7FFF, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFE, 1, Def_Addr, 0, 1, 1, 1, 1},
	{0x7FFD, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFC, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFB, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FFA, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FF9, 1, Def_Addr, 0, 1, 1, 1, 0},
	{0x7FF8, 1, Def_Addr, 0, 1, 1, 1, 0},
	//BMS_RegMap:
	{0x7F00, BMSData_size, (uint8_t*) &BMSData, 1, 0, 0, 0, 0},
	{0x7F80, Config_size+1, (uint8_t*) &Config, 1, 1, 0, 0, 0},
	{0x7FE0, 2, (uint8_t*) ID_BMS, 1, 0, 0, 0, 0},
	{0x7FE2, 5, (uint8_t*) &VirtualFG, 1, 1, 0, 0, 1},
	{0x7FE8, 2, (uint8_t*) &ID_DONGLE, 1, 0, 0, 0, 0},
	{0x7FEE, 1, (uint8_t*) &IsenseR, 1, 1, 0, 0, 1},
	{0x7FF0, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF1, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF2, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF3, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF4, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF5, 1, Def_Addr, 0, 1, 0, 1, 1},
	{0x7FF6, 1, Def_Addr, 0, 1, 0, 1, 1}};
#endif

//MAIN PROGRAM***********************************************************************
int main(void)
{
	//Uncomment to debug Reset cause:
	Reset_Flags = RCC->CSR;
	RCC->CSR |= (1<<24);
	
	Syst_Init();
	
	//Initializes all peripherals
	InitGPIO();																//Initialize GPIO peripherals
	ADC_Configuration();// adc init

	SET_FG_NRST;															//Releases the MPF4279x RST
	SET_FG_EN;																//Wakes up the MPF4279x
	RSET_MP_nSHDN;														//Puts MP279x in Shutdown
	
	InitTim3();																//Initializes and configures the TIM3
	TIM3->ARR=START_TIME;											//Configure the TIM3 for the start time
	NVIC_EnableIRQ(TIM3_IRQn);			  				//Enable TIMER 3 Interrupt
	NVIC_SetPriority(TIM3_IRQn,TIM3_IP);			//Set TIMER 3 Interrupt priority 
	
	I2C_Config_MP279x(I2C2);									//Initialize I2C2 Peripheral for communication with MP279xC
	I2C_Config_MPF4279x(I2C1);									//Initialize I2C1 Peripheral for communication with MPF4279x

	InitUART1();																//Initialize the UART for user/GUI communication
	NVIC_EnableIRQ(USART1_IRQn);			  				//Enable USART1 Interrupt
	NVIC_SetPriority(USART1_IRQn, UART_IP); 			//Set USART1 Interrupt priority
	
	InitTim6();																	//Initialize the Timer 6 peripheral
	NVIC_EnableIRQ(TIM6_IRQn);			  					//Enable TIMER 6 Interrupt
	NVIC_SetPriority(TIM6_IRQn, TIM6_IP);				//Set TIMER 6 Interrupt priority

	
	RCC->AHBENR|=(1<<6);												//Enable CRC peripheral
	
	#ifdef WDG_EN
	InitIWDG();																	//Initialize Independent Watchdog peripheral
	#endif
	
	//Initialize flash values for configuration (if empty it uses default configurations)
	InitConfiguration();

	SysTick_Delay_Init();														//Initialize delay controller
	
	//Start time: wait for the power up to continue with the program
	TIM3->CR1|=1; 																	//Start TIM3 and wait until interrupt

	//Turn on All LEDS  
	SET_LD0;
	SET_LD1;
	SET_LD2;
	SET_LD3;
	SET_LD4;
	
	//Waits 1s for the power up.
	while(!start) Nop();														//Wait for the start time
	TIM3->CR1=0;																		//STOPS timer 3
	
	SET_MP_nSHDN;																		//Releases MP279x from Shutdown
	SysTick_Delay_us(1000);													//Wait 1ms before starting I2C transactions
	{
		uint8_t ErrFlg;
		do
		{
			ErrFlg=I2C_MP279xC_Write16B(I2C2,1,REG_FT_CLR, 0xFF, 0xFF, NO_CRC, 1);			// Clear all turn-on faults
		}while(ErrFlg);
	}
	I2C_MP279xC_Write16B(I2C2,1,REG_GPIO_OUT, 0x08, 0x00, NO_CRC, 1);								// MP279x GPIO Configuration
	MP279xCInit();																																	// MP279x Initialization
	if (Config.Features_Enable&0x0200) BMSData.CC_Status.CC_Running=1;
	
	#ifdef High_P
	SET_MP_nSHDN_Gate;												//De-activates nSHDN circuitry to fix 3.3V behaviour
	#endif

	EXTI->PR|=0x0105;											//Clears any pending interrupt for EXTI0, EXTI2, EXTI8.
	
	EXTI->IMR|=0x0105;										//Unmask EXTI0, EXTI2, EXTI8.
	EXTI->RTSR|=0x0101;										//Set EXTI0, EXTI8 rising edge.
	EXTI->FTSR|=0x0004;										//Set  EXTI2 falling edge.
	
	//Clear any WDT related interrupt
	I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_INT0, ((uint8_t *)&Interrupts), ((uint8_t *)&Interrupts)+1, USE_CRC, 1);
	I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_INT1, ((uint8_t *)&Interrupts)+2, ((uint8_t *)&Interrupts)+3, USE_CRC, 1);
	if (Interrupts&0x0100) I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x01, USE_CRC, 1);
	
	NVIC_EnableIRQ(EXTI2_3_IRQn);										//SET EXTI 2 interrupt for xALARM
	NVIC_SetPriority(EXTI2_3_IRQn,EXTI2_IP);	  		//Set EXTI 2 Interrupt priority
	
	NVIC_EnableIRQ(EXTI0_1_IRQn);										//SET EXTI 0 interrupt for MP_GPIO3/Fault Indicator
	NVIC_SetPriority(EXTI0_1_IRQn,EXTI0_IP);	  		//Set EXTI 0 Interrupt priority
	
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_CONFIG, 0x5D, 0x2E, USE_CRC, 1);
	I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_WDT_STATUS, &WDT_STS[0], &WDT_STS[1], USE_CRC, 1);
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);
	
	NVIC_EnableIRQ(EXTI4_15_IRQn);										//SET EXTI 0 interrupt for FG_IRQ
	NVIC_SetPriority(EXTI4_15_IRQn,EXTI9_5_IP);	  		//Set EXTI 0 Interrupt priority

	I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x1000, (uint8_t *) &FG_Ts, 1, 1, MPF4279x_CRC_EN);	//Read Fuel Gauge's Period of execution

	//Init RTC interrupt each 100ms for threading control
	InitRTC();																//Init RTC configuration						
	NVIC_SetPriority(RTC_IRQn,RTC_IP);				//SET RTC Interrupt priority
	NVIC_EnableIRQ(RTC_IRQn);									//ENABLE RTC Interrupt
	
	//Wait until inital data is measured
	while (!FirstSample)	//Wait until inital measurements are obtained from MP279x
	{
		//While waiting verify xALERT pin is high, otherwise schedule xALARM_Handler
		SysTick_Delay_us(1000);
		if (!RD_xALERT) xALARM_Pending=1;
	}
	//Check for errors:
	if (RD_FAULT) Fault=1;
	else
	{
		RSET_LD4;
		//Which wake up to use:
		if (BMSData.PACKV>(BMSData.VTOP*0.66))
		{
			BMSData.BMSstatus=WAKE_UP_CHG;				//Wake up in charge state. Avoid the pre-charge.
			Wake_up_CHG_done=1;
		}
		else
		{
			BMSData.BMSstatus=WAKE_UP;						//Wake up in normal state
#ifdef Use_PreChg
			SET_PRECHARGE;			 						//Enable Pre-Charge
#endif
			Wake_up_DSC_done=1;
		}
	}
			
	//Turn off the unused LEDS. Now the BMS its initialized.
	#ifndef High_P
	RSET_LD1;
	#endif
	RSET_LD2;
	
	#ifdef WDG_EN
	IWDG->KR=WDG_KEY_START;											//Activate the independent watchdog
	#endif
	
	
			// Default low heater and fan
		GPIOB->ODR &=~ GPIO_ODR_3; // PB3 pinini LOW yap
	  GPIOB->ODR &=~ GPIO_ODR_4; // PB4 pinini LOW yap
	
	  //GPIOB->ODR |=  GPIO_ODR_3; // PB3 pinini HIGH yap
		//GPIOB->ODR |=  GPIO_ODR_4; // PB4 pinini HIGH yap

	
	//MAIN LOOP****************************************************
	while(1)
	{
		
		
		// Raad heater and external ntc for 50 times and get avarage
		totalHeaterNtc = 0;
		totalExternalNtc = 0;
		for( ntcC=0; ntcC<50; ntcC++ ){
			heaterNtc = adc_pa7();
			totalHeaterNtc = totalHeaterNtc + heaterNtc;
			externalNtc = adc_pa5();
			totalExternalNtc = totalExternalNtc + externalNtc;
		}
		heaterNtc = totalHeaterNtc/50;
		externalNtc = totalExternalNtc/50;
		
		// Get celcius data for ntc's
		heaterNtcCelcius = ADC_to_Temperature(heaterNtc);
		externalNtcCelcius = ADC_to_Temperature(externalNtc);
		
		
		//Get card ntc values
		//avarageCardNtc=Measurements.TempVect[0]/100;
		avarageCardNtc=(Measurements.TempVect[0]+Measurements.TempVect[1]+Measurements.TempVect[2])/3/100;
		


		// if heater ntc > 65 heater low
		
		if(heaterNtcCelcius > 65){
			GPIOB->ODR &=~ GPIO_ODR_4; // PB4 pinini LOW yap
			heaterOn = 0;
	  }
		
		if(heaterNtcCelcius < -50){
			GPIOB->ODR &=~ GPIO_ODR_4; // PB4 pinini LOW yap
			heaterOn = 0;
	  }
		
		
		
		
		 // Works only at charge mode
		if(BMSData.BMSstatus==CHG){
			if(avarageCardNtc>=40 && fanOn == 0){
	      GPIOB->ODR |=  GPIO_ODR_3; // PB3 pinini HIGH yap
				fanOn = 1;
			}
			if(avarageCardNtc <= 38 && fanOn == 1 ){
					GPIOB->ODR &=~ GPIO_ODR_3; // PB3 pinini LOW yap
					fanOn = 0;
			}
			if(avarageCardNtc <= 0 && avarageCardNtc > -50 && heaterOn == 0){
					GPIOB->ODR |=  GPIO_ODR_4; // PB4 pinini HIGH yap
					heaterOn = 1;
				
			}
			if(avarageCardNtc >= 5 && heaterOn == 1){
				GPIOB->ODR &=~ GPIO_ODR_4; // PB4 pinini LOW yap
				heaterOn = 0;
		
			}
		}else{
			
			if(fanOn==1){
			GPIOB->ODR &=~ GPIO_ODR_3; // PB3 pinini LOW yap
				fanOn = 0;
			}
			
			if(heaterOn==1){
			GPIOB->ODR &=~ GPIO_ODR_4; // PB4 pinini LOW yap
				heaterOn = 0;
			}
				
		}
		
		 if (GPIOA->IDR & 0x1) { // PA0 pinini kontrol edin
            
			} else {
					// Butona basilmamissa LED'i sondurun
					StandByFlag =0; // PD12 pinini LOW yapin
					
			}

		
		
		
		
		//Initial Redundancy Checks for possible miss-handled ALARMS
		if ((*((__packed uint32_t *) &BMSData.BMSAlarms))&0x0003FFFF)
		{
			BMSData.BMSstatus=ALARM;
#ifdef Direct_Mode
			DISABLE_DCH;
			DISABLE_CHG;
#else
			DISABLE_FETs;
#endif
#ifdef Use_PreChg
			RSET_PRECHARGE;
#endif
			SET_LD4;
			Reset_Alarms_done=0;
		}
		//Check FAULT Pin Status:
		if (RD_FAULT)
		{
			Reset_Alarms_done=0;
			Fault=1;
		}
		//Check xALERT Pin Status:
		if (!RD_xALERT)
		{
			xALARM_Pending=1;
		}
		
		//Redundant LEDs 3 and 4 Control:
		if (BMSData.BMSstatus==AUTO_RECOVERY) SET_LD3;
		else {
			DOC_AutoRec=0;
			DSC_AutoRec=0;
			COC_AutoRec=0;
			CSC_AutoRec=0;
			RSET_LD3;
		}

		if (BMSData.BMSstatus==ALARM) SET_LD4;
		else RSET_LD4;

		//----------------------------------------------------------------------------------------------------
		//---------------------------------------- BMS STATE MACHINE: ----------------------------------------
		//----------------------------------------------------------------------------------------------------

		//-------------------------- WAKE_UP State --------------------------------------
		//-------------------------------------------------------------------------------
		//State WAKE_UP from SAFE mode
		if (BMSData.BMSstatus==SAFE && (!SafeFlag))
		{
			if (BMSData.PACKV>(BMSData.VTOP*0.66))
			{
				BMSData.BMSstatus=WAKE_UP_CHG;				//Wake up in charge state. Avoid the pre-charge.
				Wake_up_CHG_done=1;
			}
			else
			{
				BMSData.BMSstatus=WAKE_UP;						//Wake up in normal state
#ifdef Use_PreChg
				SET_PRECHARGE;			 					//Enable Pre-Charge
#endif
				Wake_up_DSC_done=1;
			}
		}

		//State WAKE_UP from STAND BY mode
		if (Sby_Exit && (!(StandByFlag)))
		{
			SET_FG_EN;														//Enable the MPF4279x wake up pin
			FirstSample=0;
			StandByFlag=0;
			NVIC_EnableIRQ(RTC_IRQn);							//Re-enable interrupt for RTC
			#ifdef ENABLE_CC
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x02, 0x4E, USE_CRC, 1);			//Run CC Again
			#endif
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_STB_CONFIG, (0x08 | ((Config.Sby_I_Thr&0x03)<<1)), 0x00, USE_CRC, 1); //0,0
			//Re-enable all interrupts
			NVIC_EnableIRQ(EXTI4_15_IRQn);
			NVIC_EnableIRQ(TIM6_IRQn);
			NVIC_EnableIRQ(TIM3_IRQn);
			
			FirstSample=0;
			StandByFlag=0;
			
			//Reconfigure WDT to normal operation:
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_WDT_STATUS, &WDT_STS[0], &WDT_STS[1], USE_CRC, 1);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);
			

			Tim3first=0;
			TIM3->ARR=START_TIME;											//Configure the TIM3 for the start time
			TIM3->EGR|=1;
			TIM3->CNT=0;
			TIM3->CR1=1;
			start=0;	
			//Set All LEDs
			SET_LD0;
			SET_LD1;
			SET_LD2;
			SET_LD3;
			SET_LD4;
			
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			while(!start) Nop();																				//Wait for the start interval
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			
			//Reset all remaining LEDs:
			#ifndef High_P
			RSET_LD1;
			#endif
			RSET_LD2;
			
			TIM3->CNT=0;
			TIM3->CR1=0;
			
			//Wait until inital data is measured
			while (!FirstSample)	//Wait until inital measurements are obtained
			{
				//While waiting verify that xALERT pin remains high, if not schedule xALERT Handler
				SysTick_Delay_us(1000);
				if (!RD_xALERT) xALARM_Pending=1;
			}
			
			//Select Wake Up mode:
			if ((BMSData.BMSstatus!=ALARM)&&(!RD_FAULT))
			{
				RSET_LD4;
				if (Sby_Exit&0x02) BMSData.BMSstatus=DCHG;
				else
				{
					if (BMSData.PACKV>(BMSData.VTOP*0.66))
					{
						BMSData.BMSstatus=WAKE_UP_CHG;				//Wake up in charge state. Avoid the pre-charge.
						Wake_up_CHG_done=1;
					}
					else
					{
						BMSData.BMSstatus=WAKE_UP;						//Wake up in normal state
#ifdef Use_PreChg
						SET_PRECHARGE;			 					//Enable Pre-Charge
#endif
						Wake_up_DSC_done=1;
					}
				}
			}
			else
			{
				Reset_Alarms_done=0;
				Fault=1;
			}
			Sby_Exit=0;
			//End start interval.
		}
		
		//-------------------------- DISCHARGE State --------------------------------------
		//-------------------------------------------------------------------------------
		if ((BMSData.BMSstatus==WAKE_UP) && Wake_up_DSC_done)	//WAKE_UP to DISCHARGE transition:
		{
#ifdef Direct_Mode
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(500000);
			ENABLE_CHG;
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(2000000);
			ENABLE_DCH;
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(1000000);
#else
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(500000);
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			if ((BMSData.BMSstatus==WAKE_UP)&&!RD_FAULT) 
			{	
				ENABLE_FETs;																								//Close the protections
				SysTick_Delay_us(500000);
				#ifdef WDG_EN
				IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
				#endif
				SysTick_Delay_us(500000);
			}
#endif
			if ((BMSData.BMSstatus==WAKE_UP)&&!RD_FAULT) BMSData.BMSstatus=DCHG;	//After protections are closed, transition to Discharge State
#ifdef Use_PreChg
			RSET_PRECHARGE;			 					//Disable Pre-Charge
#endif
			Wake_up_DSC_done=0;
			DisableSby=1;
		}
		else if ((BMSData.BMSstatus==CHG) && (Measurements.Current[0]<=-100))	//CHARGE to DISCHARGE transition:
		{
			BMSData.BMSstatus=DCHG;
		}
		
		//-------------------------- CHARGE State --------------------------------------
		//-------------------------------------------------------------------------------
		if ((BMSData.BMSstatus==WAKE_UP_CHG) && Wake_up_CHG_done)	//WAKE_UP_CHG to CHARGE transition:
		{
#ifdef Direct_Mode
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(1000);
			//Set the FET configuration
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_STB_CONFIG, (0x48 | ((Config.Sby_I_Thr&0x03)<<1)), 0x00, USE_CRC, 1);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_MODE, MP279xConfig.FET_MODE&0x00FF, ((MP279xConfig.FET_MODE&0xFF00)>>8)| 0x10, USE_CRC, 1);		//Use configurable FET Mode Config.
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ACT_CONFIG, 0x03, 0x02, USE_CRC, 1); //Use direct FET Control insted of Simple Control
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x01, 0, USE_CRC, 1);
			SysTick_Delay_us(1000000);
			SET_GPIO1;																									//Close DSG FET first
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(500000);
			SET_GPIO2;																									//Close CHG FET to start charging
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(500000);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x00, 0, USE_CRC, 1);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ACT_CONFIG, 0x01, 0x02, USE_CRC, 1); // Configure MP279x to use Simple Control (It manages itself the FET Turning on and off sequence
#else
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			SysTick_Delay_us(500000);
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			if ((BMSData.BMSstatus==WAKE_UP_CHG)&&!RD_FAULT)
			{
				ENABLE_FETs;
				SysTick_Delay_us(500000);
			}
#endif
			if ((BMSData.BMSstatus==WAKE_UP_CHG)&&!RD_FAULT) BMSData.BMSstatus=CHG; // Change to CHG state
			Wake_up_CHG_done=0;
			DisableSby=1;
		}
		else if ((BMSData.BMSstatus==DCHG) && (Measurements.Current[0]>=100))	//DISCHARGE to CHARGE transition:
		{
			BMSData.BMSstatus=CHG;
		}

		//-------------------------- SAFE State --------------------------------------
		//-------------------------------------------------------------------------------
		//Enter Safe State:
		if ((((BMSData.BMSstatus==DCHG)||(BMSData.BMSstatus==CHG)) && SafeFlag) || (Reset_Alarms_done == 1))
		{
#ifdef Direct_Mode
			if (BMSData.BMSstatus==DCHG)
			{
				DISABLE_DCH;
				SysTick_Delay_us(10000);
				DISABLE_CHG;
			}
			if (BMSData.BMSstatus==CHG)
			{
				DISABLE_CHG;
				SysTick_Delay_us(10000);
				DISABLE_DCH;
			}
#else
			DISABLE_FETs;																//Open the protections using the Simple Mode (IC decides the correct sequence)
#endif
#ifdef Use_PreChg
			RSET_PRECHARGE;
#endif
			Reset_Alarms_done=0;
			SafeFlag=1;
			BMSData.BMSstatus=SAFE;											//Transition to SAFE state
			// Clear cell dead flag correctly. Used to prevent wrong flag clearance
			if (CellDead_CPending==1) {
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG & 0xFE, 0x00, USE_CRC, 1);
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG, 0x00, USE_CRC, 1);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, (MP279xConfig.CELLFT_CTRL&0x00FF) | 0x01, ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x04, 0x00, USE_CRC, 1);
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, (MP279xConfig.CELLFT_CTRL&0x00FF), ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACKFT_CTRL, MP279xConfig.PACKFT_CTRL&0x00FF, ((MP279xConfig.PACKFT_CTRL&0xFF00)>>8) | 0x80, USE_CRC, 1);
				CellDead_CPending=0;
			}
		}

		//-------------------------- STAND_BY State --------------------------------------
		//-------------------------------------------------------------------------------
		if ((((BMSData.BMSstatus==DCHG)||(BMSData.BMSstatus==CHG)||(BMSData.BMSstatus==SAFE)) && StandByFlag) || (Reset_Alarms_done == 2)) //Enter STAND BY state
		{			
			// Disable Interrupts
			NVIC_DisableIRQ(RTC_IRQn);
			NVIC_DisableIRQ(EXTI4_15_IRQn);
			NVIC_DisableIRQ(TIM3_IRQn);
			
#ifdef Direct_Mode
#ifdef PFET_Enable
			SysTick_Delay_us(10000);
			if ((!Reset_Alarms_done)&&(Config.Features_Enable&0x10)) I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x01, 0, USE_CRC, 0);
#endif
			SysTick_Delay_us(10000);
			DISABLE_DCH;
			SysTick_Delay_us(30000);
			DISABLE_CHG;
#else
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_STB_CONFIG, (0x49 | ((Config.Sby_I_Thr&0x03)<<1)), 0x00, USE_CRC, 0); //Reconfigure Stand-by Current threshold
			SysTick_Delay_us(10000);
#ifdef PFET_Enable
			if (Reset_Alarms_done||(!(Config.Features_Enable&0x10)))
			{
				DISABLE_FETs;
				PFET_On=0;
			}
			else PFET_On=1;
#else
			DISABLE_FETs;
#endif
#endif
			//Clear and re-configure the interrupts
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x40, USE_CRC, 0);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_EN, 0x00, 0xDD, USE_CRC, 1);	//0x30, 0x6D; 0x00, 0x4D
			
			if (Config.Features_Enable&0x06)
			{
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, 0x01, 0x83, USE_CRC, 1);		//Clear Configuration
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, (Config.Features_Enable&0x06) | 0x20, 0x83, USE_CRC, 1); //Charger and load detection activated.
			}
			
			//Reset all LEDs:
			RSET_LD0;
			RSET_LD1;
			RSET_LD2;
			RSET_LD3;
			RSET_LD4;
			
			BMSData.BMSstatus=STAND_BY; //Puts in stand by mode.
			Reset_Alarms_done=0;
			SafeFlag=0;
			RSET_FG_EN;
			
			//Re-configure the WDG to match the new CLK speed
			#ifdef WDG_EN
			IWDG->KR=0x5555;							//This key give write acces to Prescaler (PR) and reload value (RLR) registers
			IWDG->PR=0x06;								//IWDG LSI 40kHz CLK prescaler (0x06 -> /256)
			IWDG->RLR=0x061B;							//IWDG Reload Value (WDG_PR=0x06 & WDG_RLR=0x61B -> if not reloaded reset in 10s)
			IWDG->KR=WDG_KEY_RELOAD;			//This key is to reload the RLR value to the counter and prevent write acces to Prescaler (PR) and reload value (RLR) registers
			#endif
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x02, 0x4E, USE_CRC, 1); //CC 0x02
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_CONFIG, 0xDD, 0xEF, USE_CRC, 1);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);
#ifdef Direct_Mode
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_STB_CONFIG, (0x49 | ((Config.Sby_I_Thr&0x03)<<1)), 0x00, USE_CRC, 1); //0,0
#endif
			
			//Change to HSI clock to lower current consumption:
			frequency=8;
			while(!(RCC->CR&RCC_CR_HSIRDY)) RCC->CR|=RCC_CR_HSION;
			while((RCC->CFGR&RCC_CFGR_SWS)!=0) RCC->CFGR &=~(0x03);
			RCC->CR&=!RCC_CR_PLLON;
			RCC->CR&=!RCC_CR_HSEON;
			//Configure the I2C for the new CLK
			I2C_Config_MP279x_8MHz(I2C2);
			
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);
			
			//Enter   mode and wait for StandByFlag=1;
			while(StandByFlag)
			{
				
				uint16_t DataTemp;
				I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_PWR_STATUS, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1);
				if (!(DataTemp&0x0100))
				{
					Sby_Exit|=0x02;
					StandByFlag=0;
				}
				while (!RD_xALERT)
				{
					SysTick_Delay_us(1000);
					xALARM_Handle();
				}
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 0);
				__WFI();
				
				
				// Stand By escape with pa0 button
				if (GPIOA->IDR & 0x1) { // PA0 pinini kontrol edin
							
				} else {
						// Butona basilmamissa LED'i sondurun
						StandByFlag =0; // PD12 pinini LOW yapin
					
				}
			}
			
			//Go back to PLL:
			while(!(RCC->CR&RCC_CR_HSERDY)) RCC->CR|=RCC_CR_HSEON;
			while(!(RCC->CR&RCC_CR_PLLRDY)) RCC->CR|=RCC_CR_PLLON;
			while((RCC->CFGR&RCC_CFGR_SWS)!=0x08) RCC->CFGR |= 0x02;
			frequency=48;
			
			//Configure the I2C for the new CLK
			I2C_Config_MP279x(I2C2);
			
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 0);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_EN, 0x00, 0xBD, USE_CRC, 1);					//Reconfigure Interrupts
			
			DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 3
			DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address
			DMA1_Channel3 -> CNDTR = 0x5C;														//The buffer has a 92 byte size
			RXcont=0;
			
			
			//Clear the reception buffer
			for (unsigned char i=0; i<23; i++) *(RxAddress+i)=0;
			
			USART1 -> RDR;
			
			#ifdef High_P
			SET_RS485_TXE_RXNE;
			SysTick_Delay_us(100);
			#endif
		
			DMA1_Channel3 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 3
			
			DMA1_Channel2 -> CCR &= ~DMA_CCR_EN;											//Enable DMA Channel 2
			DMA1 -> IFCR  |= DMA_IFCR_CTCIF2;													//Clear the transmision complete flag of the DMA
			USART1 -> ISR &= ~(USART_ISR_TC);														//Clear the TC bit in the UART_SR register
			DMA1_Channel2 -> CMAR = (uint32_t) (Tx_NACK); 							//DMA reads the values in this buffer to the above address
			DMA1_Channel2 -> CNDTR = 10;																	//The transmit buffer has a byte size equals to the defined as BT_LENGTH_TX
			DMA1_Channel2 -> CCR |= DMA_CCR_EN;													//Enable DMA Channel 2
			USART1 -> ISR &= ~(USART_ISR_TXE);
			
			Sby_Exit|=1;
			SbyCounter=0;

			New_Meas = 0;

			#ifdef WDG_EN
			InitIWDG();
			#endif
		}
		
		//-------------------------- ALARM/RESET_ALARM State ----------------------------
		//-------------------------------------------------------------------------------
		if (BMSData.BMSstatus==ALARM)
		{
			if (StandByFlag||SafeFlag) ResetAlarms();
			else if ((!((*((__packed uint32_t *) &BMSData.BMSAlarms))&0x0003FFFF)) && (!((*(__packed uint32_t *)(&BMSData.RT_Alarms))&0x0003FFFF)) && (!RD_FAULT)) //Redundancy check
			{
				BMSData.BMSstatus=RESET_ALARM;
				Reset_Alarms_done=1;
			}
		}
		if ((BMSData.BMSstatus==RESET_ALARM)&&(!Reset_Alarms_done)) ResetAlarms();

		//-------------------------- AUTO_RECOVERY State --------------------------------
		//-------------------------------------------------------------------------------
		if ((BMSData.BMSstatus==AUTO_RECOVERY)&&(!RD_FAULT))
		{
			//Select what state the IC is going after an AUTO-RECOVERY situation
			if (FETs_PIN_Status)
			{
				if (BMSData.AFE_Status&0x80) BMSData.BMSstatus=CHG;
				else BMSData.BMSstatus=DCHG;
			}
			else if (SafeFlag) BMSData.BMSstatus=SAFE;
			else
			{
				if (BMSData.PACKV>(BMSData.VTOP*0.66))
				{
					BMSData.BMSstatus=WAKE_UP_CHG;				//Wake up in charge state. Avoid the pre-charge.
					Wake_up_CHG_done=1;
				}
				else
				{
					BMSData.BMSstatus=WAKE_UP;						//Wake up in normal state
#ifdef Use_PreChg
					SET_PRECHARGE;			 					//Enable Pre-Charge
#endif
					Wake_up_DSC_done=1;
				}
			}
			RSET_LD3;
		}

		//Prevent UART's DMA from getting stucked
		if ((((USART1->ISR) & (USART_ISR_TC)) && ((DMA1->ISR)&(DMA_ISR_TCIF2))) || ((DMA1->ISR)&(DMA_ISR_TEIF2)))
		{
			DMA1_Channel2 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 2
			DMA1 -> IFCR  |= (DMA_IFCR_CTCIF2|DMA_IFCR_CTEIF2);													//Clear the transmision complete flag of the DMA
			USART1 -> ISR &= ~(USART_ISR_TC);														//Clear the TC bit in the UART_SR register
			#ifdef High_P
			SysTick_Delay_us(50);
			RSET_RS485_TXE_RXNE;
			#endif
		}
		
		#ifdef WDG_EN
		IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
		#endif
	}
}

//FAULT Pin Interrupt Handler
void EXTI0_1_IRQHandler(void)
{
	if (MP279x_I2C_Busy) Fault=1;		//When the I2C is not busy anymore, the IC will run the Fault_Handle function 
	else
	{
		Fault_Handle();
		Fault=0;
	}
	GPIO3IT++;
	EXTI->PR|=1; //Clear the Interrupt flag
}

//xALERT pin Handler
void EXTI2_3_IRQHandler(void)
{
	if (MP279x_I2C_Busy) xALARM_Pending=1; //When the I2C is not busy anymore, the IC will run the xALARM_Handle function 
	else
	{
		xALARM_Handle();
		xALARM_Pending=0;
	}
	xALARMIT++;
	EXTI->PR|=(1<<2); //Clear the interrupt flag
}

//FG_IT pin Handler
void EXTI4_15_IRQHandler(void)
{
	//Schedule a FG Results read, after a FG iteration has finished:
	if (!VirtualFG.VFG_En)
	{
		if (FirstSoC) FGReady=1;
	}
	else if (VirtualFG.VFG_IT_Running && !VirtualFG.VFG_IT_Done && !VirtualFG.VFG_Reading)
	{
		ReadRsltsVFG();
	}
	EXTI->PR|=(1<<8);	//Clear the interrupt flag
}

//TIM3. Defines the startime.
void TIM3_IRQHandler(void)
{
	TIM3->SR=~TIM_SR_UIF; //Clear TIM2 interrupt flag
	if (!Tim3first) Tim3first=1;
	else start=1;
}

//TIM6. Defines the UART Timeout
void TIM6_IRQHandler(void)
{
	TIM6->SR=~TIM_SR_UIF; //Clear TIM6 interrupt flag
	TIM6->CR1&=~1;
	TIM6->CNT=0;
	DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 3
	DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address
	DMA1_Channel3 -> CNDTR = 0x5C;														//The buffer has a 4 byte size
	if (RXcont)
	{
		DMA1_Channel2 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 4
		DMA1 -> IFCR  |= DMA_IFCR_CTCIF2;													//Clear the transmision complete flag of the DMA
		USART1 -> ISR &= ~(USART_ISR_TC);													//Clear the TC bit in the UART_SR register
		DMA1_Channel2 -> CNDTR = 10;															//The transmit buffer has a byte size equals to the defined fo ACK and NACK messages
		#ifdef High_P
		SET_RS485_TXE_RXNE;
		SysTick_Delay_us(10);
		#endif		
		DMA1_Channel2 -> CMAR = (uint32_t) (Tx_NACK); 						//DMA reads the values in this buffer to the above address
		DMA1_Channel2 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 4
		USART1 -> ISR &= ~(USART_ISR_TXE);													//Clear the UART TX buffer Empty register to send the UART message using DMA
		RXcont=0;
	}
	for (unsigned char i=0; i<23; i++) *(RxAddress+i)=0;
	USART1 -> RDR;
	DMA1_Channel3 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 5
}

//UART reception Interrupt
void USART1_IRQHandler(void)
{
	#ifdef High_P
	USART1->ICR|=(USART_ICR_IDLECF 	//Clear UART Interrupt flag
							| USART_ICR_CMCF);	//Clear Character Match flag
	#else
	USART1->ICR|=USART_ICR_IDLECF; 	//Clear UART Interrupt flag
	#endif
	USART1->RDR;
	if (!StandByFlag) CheckUARTRx(); // When the MCU is not in StnadBy state, checked the UART Data
	else StandByFlag=0;
	//	UART_IT_Cnt++;
}


//Check the status of received data from the UART
void CheckUARTRx(void)
{
	if ((USART1 -> ISR) & USART_ISR_TXE)						//Checked that a UART transmiting action is not on-going
	{
		if (RxBuff[0]==0x7E)													//The header of the message has to be 0x7E
		{	
			if (RxBuff[3]&1) {													//Extract if the message is a read/write action
				UART_Mode=RxMODE;
			}
			else {
				UART_Mode=TxMODE;
			}
			if (((((RxBuff[2]<<8) | RxBuff[1])<=82) || (VirtualFG.VFG_En && (UART_Mode==TxMODE) && (((((RxBuff[3]>>1) | (RxBuff[4]<<7))==0x0000) && (((RxBuff[2]<<8) | RxBuff[1])<=Measurements_size)) || 
				((((RxBuff[3]>>1) | (RxBuff[4]<<7))==0x0400) && (((RxBuff[2]<<8) | RxBuff[1])<=fg_out_size))))) && 
				(((RxBuff[RxBuff[1]+9]==0x0A)&&(UART_Mode==RxMODE))||((RxBuff[9]==0x0A)&&(UART_Mode==TxMODE))))		//Check the message has the correct Start and End bytes
			{
				DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 3
				DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address
				DMA1_Channel3 -> CNDTR = 0x5C;														//The buffer has a 92 byte size
				TIM6->CNT=0;
				TIM6->CR1&=~1;																						//Stop Timeout Counter
				UART_DataLength=((RxBuff[2]<<8) | RxBuff[1]);
				Read_UART();																							//Check & process the received message
				
				//Clear the reception buffer
				if (!UART_Mode)
				{
					unsigned char Lim=(UART_DataLength+10)/4;
					if ((UART_DataLength+10)%4) Lim++;
					for (unsigned char i=0; i<Lim; i++) *(RxAddress+i)=0;
				}
				else
				{
					*(RxAddress)=*(RxAddress+1)=*(RxAddress+2)=0;
				}
				
				RXcont=0;
				USART1 -> RDR;
				DMA1_Channel3 -> CCR |= DMA_CCR_EN;											//Re-enable DMA Channel 3
			}
			#ifdef LV_Test_EN
			else if ((((RxBuff[2]<<8) | RxBuff[1])==0xFFFF) && (((RxBuff[4]<<8) | RxBuff[3])==0xAAAA) && (((RxBuff[8]<<24) | (RxBuff[7]<<16) | (RxBuff[6]<<8) | RxBuff[5])==0xEB849C6F) && ((RxBuff[9]==0x0A)&&(UART_Mode==TxMODE)))
			{
				DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 3
				DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address
				DMA1_Channel3 -> CNDTR = 0x5C;														//The buffer has a 92 byte size
				TIM6->CNT=0;
				TIM6->CR1&=~1;																						//Stop Timeout Counter
				UART_DataLength=((RxBuff[2]<<8) | RxBuff[1]);
				SendUART();
				*(RxAddress)=*(RxAddress+1)=*(RxAddress+2)=0;
				RXcont=0;
				USART1 -> RDR;
				DMA1_Channel3 -> CCR |= DMA_CCR_EN;		
			}
			#endif
			else if (((RxBuff[RxBuff[1]+9]!=0x00)&&(UART_Mode==RxMODE))||((RxBuff[9]!=0x00)&&(UART_Mode==TxMODE)))		//if a message with incorrect Start and End bytes is received
			{
				DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 3
				DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address
				DMA1_Channel3 -> CNDTR = 0x5C;														//The buffer has a 92 byte size
				
				#ifdef High_P
				SET_RS485_TXE_RXNE;
				SysTick_Delay_us(100);
				#endif
				
				DMA1_Channel2 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 2
				DMA1 -> ISR  &= ~(DMA_ISR_TCIF2);													//Clear the transmision complete flag of the DMA
				USART1 -> ISR &= ~(USART_ISR_TC);														//Clear the TC bit in the UART_SR register
				DMA1_Channel2 -> CNDTR = 10;															//The transmit buffer has a byte size equals to the defined fo ACK and NACK messages
				DMA1_Channel2 -> CMAR = (uint32_t) (Tx_NACK); 						//DMA reads the values in this buffer to the above address
				DMA1_Channel2 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 2
				USART1 -> ISR &= ~(USART_ISR_TXE);													//Clear the UART TX buffer Empty register to send the UART message using DMA

				TIM6->CNT=0;
				TIM6->CR1&=~1;			//Stop Timeout Counter
				RXcont=0;
				
				//Clear the reception buffer
				for (unsigned char i=0; i<23; i++) *(RxAddress+i)=0;
				
				USART1 -> RDR;
				DMA1_Channel3 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 5
			}
			else if (!RXcont)
			{
				RXcont=1;
				TIM6->CNT=0;		//Start timeout counter
				TIM6->CR1|=1;
			}
		}
		else if ((RxBuff[0]!=0x00)||(DMA1_Channel3 -> CNDTR != 0x5C))	//Restart the DMA when the header is not matching
		{
			DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 3
			DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address
			DMA1_Channel3 -> CNDTR = 0x5C;														//The buffer has a 92 byte size
			USART1 -> RDR;
			DMA1_Channel3 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 3
			DMA1_Channel3 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 3
			DMA1_Channel3 -> CMAR = (uint32_t) (RxBuff);							//DMA writes the value of the above address in this address
			DMA1_Channel3 -> CNDTR = 0x5C;														//The buffer has a 92 byte size
			
			#ifdef High_P
			SET_RS485_TXE_RXNE;
			SysTick_Delay_us(100);
			#endif

			DMA1_Channel2 -> CCR &= ~DMA_CCR_EN;											//Disable DMA Channel 2
			DMA1 -> IFCR  |= DMA_IFCR_CTCIF2;													//Clear the transmision complete flag of the DMA
			USART1 -> ISR &= ~(USART_ISR_TC);														//Clear the TC bit in the UART_SR register
			DMA1_Channel2 -> CNDTR = 10;															//The transmit buffer has a byte size equals to the defined fo ACK and NACK messages
			DMA1_Channel2 -> CMAR = (uint32_t) (Tx_NACK); 						//DMA reads the values in this buffer to the above address
			DMA1_Channel2 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 2
			USART1 -> ISR &= ~(USART_ISR_TXE);													//Clear the UART TX buffer Empty register to send the UART message using DMA

			TIM6->CNT=0;
			TIM6->CR1&=~1;			//Stop Timeout Counter
			RXcont=0;
			
			//Clear the reception buffer
			for (unsigned char i=0; i<23; i++) *(RxAddress+i)=0;
			USART1 -> RDR;
			DMA1_Channel3 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 5
		}
		else if (USART1->ISR&USART_ISR_ORE) USART1->ICR|=USART_ICR_ORECF;
	}
	else if (USART1->ISR&USART_ISR_ORE) USART1->ICR|=USART_ICR_ORECF;
}

//Handles UART successful reception and classifies the data
void Read_UART(void)
{
	uint16_t Reg_Addr;
	uint8_t Succ=0;
	
	DMA1_Channel2 -> CCR &= ~DMA_CCR_EN;											//Enable DMA Channel 4
	DMA1 -> IFCR  |= DMA_IFCR_CTCIF2;													//Clear the transmision complete flag of the DMA
	USART1 -> ISR &= ~(USART_ISR_TC);													//Clear the TC bit in the UART_SR register
	TxBuff[0]=0x7E;																						//Set the TX header	
	Reg_Addr=(RxBuff[3]>>1) | (RxBuff[4]<<7);							//Extract the Register address from the received message
	//Calculate CRC
	CRC->CR|=CRC_CR_RESET;
	SysTick_Delay_us(1);
	CRC->DR=((RxBuff[4]<<24)|(RxBuff[3]<<16)|(RxBuff[2]<<8)|(RxBuff[1]));
	if (!UART_Mode)
	{
		CRCInputUART=0;
		for(unsigned char i=5; i<(UART_DataLength+5); i++)
		{
			CRCInputUART|=(RxBuff[i]<<((i-5)%4)*8);
			if ((((i-5)%4)==3)||(i==(UART_DataLength+4)))
			{
				CRC->DR=CRCInputUART;
				CRCInputUART=0;
			}
		}
		Succ = (CRC->DR==((RxBuff[UART_DataLength+8]<<24)|(RxBuff[UART_DataLength+7]<<16)|(RxBuff[UART_DataLength+6]<<8)|RxBuff[UART_DataLength+5]));
	}
	else
	{
		Succ = (CRC->DR==((RxBuff[8]<<24)|(RxBuff[7]<<16)|(RxBuff[6]<<8)|RxBuff[5]));
	}
	
	if (Succ)
	{
		Succ=0;
		for (uint8_t i=0; i<Reg_Struct_Count; i++)
		{
			if ((Reg_Addr>=Reg_Map_info[i].Address) && ((Reg_Addr+UART_DataLength)<=(Reg_Map_info[i].Address+Reg_Map_info[i].size)))
			{
				if (((UART_Mode && Reg_Map_info[i].Read) || ((!UART_Mode) && Reg_Map_info[i].Write)) && ((!Reg_Map_info[i].Command) || RxBuff[5]==1))
				{
					if (!Reg_Map_info[i].Special)
					{
						if (Reg_Map_info[i].FG)
						{
							LL_I2C_Enable(I2C1);		//ENABLE the I2C channel
							if (!UART_Mode)
							{
								if(I2C_MPF4279x_Write(I2C1, MPF4279x_Address, Reg_Addr, &(RxBuff[5]) , UART_DataLength, 1, MPF4279x_CRC_EN))
								{
									I2Cerror=1;
								}
								else Succ=1;
							}
							else
							{	
								//resend this to the MPF4279x
								if(I2C_MPF4279x_Read(I2C1, MPF4279x_Address, Reg_Addr, &(TxBuff[5]), UART_DataLength, 1, MPF4279x_CRC_EN))
								{
									I2Cerror=1;
								}
								else Succ=1;
							}
							LL_I2C_Disable(I2C1);		//DISABLE the I2C channel
						}
						else
						{
							MemMapAddr=(Reg_Map_info[i].Mem_Address + (Reg_Addr - Reg_Map_info[i].Address));
							if(!UART_Mode)
							{
								for(uint8_t l=0; l<UART_DataLength; l++)
								{
									*MemMapAddr = RxBuff[l+5];
									MemMapAddr++;
								}
								if (Reg_Map_info[i].Address==0x7F80)
								{
									//If it's new BMS Config, update it:
									#ifndef MP2791
									if (Config.NumofSeries<7) Config.NumofSeries=7;
									else if (Config.NumofSeries>16) Config.NumofSeries=16;
									#else
									if (Config.NumofSeries<7) Config.NumofSeries=7;
									else if (Config.NumofSeries>14) Config.NumofSeries=14;
									#endif
									#ifdef INT_PROTECTED
									Config.Features_Enable_Internal=FEATURES_EN_INT;
									#endif

									MP279xCConfigCalc();
									Reconfig=1;
									SaveConfigtoFLASH();
								}
							}
							else
							{
								for(uint8_t l=0; l<UART_DataLength; l++)
								{
									TxBuff[l+5] = *MemMapAddr;
									MemMapAddr++;
								}
//								if (!CAN_Comm)
//								{
//									UARTReadResponse();
//								}
							}
							Succ=1;
						}
					}
					else
					{
						uint16_t DataTemp;
						LL_I2C_Enable(I2C1);		//ENABLE the I2C channel
						Succ=1;
						switch (Reg_Map_info[i].Address)
						{
							case 0x0000:
								if (!UART_Mode)
								{
									if (VirtualFG.VFG_En)
									{
										MemMapAddr=(((uint8_t*)&MeasurementsVFG) + (Reg_Addr - Reg_Map_info[i].Address));
										for(uint8_t l=0; l<UART_DataLength; l++)
										{
											*MemMapAddr = RxBuff[l+5];
											MemMapAddr++;
										}
									}
									#ifdef FG_NewGen
									else if (Reg_Addr>=0x0078)
									#else
									else if (Reg_Addr>=0x0076)
									#endif
									{
										if(I2C_MPF4279x_Write(I2C1, MPF4279x_Address, Reg_Addr, &(RxBuff[5]) , UART_DataLength, 1, MPF4279x_CRC_EN))
										{
											I2Cerror=1;
											Succ=0;
										}
									}
									else Succ=0;
								}
								else
								{
									if (VirtualFG.VFG_En) MemMapAddr=(((uint8_t*)&MeasurementsVFG) + (Reg_Addr - Reg_Map_info[i].Address));
									else MemMapAddr=(Reg_Map_info[i].Mem_Address + (Reg_Addr - Reg_Map_info[i].Address));
									for(uint16_t l=0; l<UART_DataLength; l++)
									{
										TxBuff[l+5] = *MemMapAddr;
										MemMapAddr++;
									}
								}
								break;
#ifdef FG_NewGen
							case 0x0400:
								if (!UART_Mode)
								{
									if ((Reg_Addr + UART_DataLength - Reg_Map_info[i].Address) <= fgset_intr_size)
									{
										if(I2C_MPF4279x_Write(I2C1, MPF4279x_Address, Reg_Addr, &(RxBuff[5]) , UART_DataLength, 1, MPF4279x_CRC_EN))
										{
											I2Cerror=1;
											Succ=0;
										}
									}
								}
								else
								{
									MemMapAddr=(Reg_Map_info[i].Mem_Address + (Reg_Addr - Reg_Map_info[i].Address));
									for(uint16_t l=0; l<UART_DataLength; l++)
									{
										TxBuff[l+5] = *MemMapAddr;
										MemMapAddr++;
									}
									VirtualFG.VFG_IT_Done=0;
								}
								break;
#endif
							case 0x7FE2:
								if (UART_Mode)
								{
									MemMapAddr=(Reg_Map_info[i].Mem_Address + (Reg_Addr - Reg_Map_info[i].Address));
									for(uint8_t l=0; l<UART_DataLength; l++)
									{
										TxBuff[l+5] = *MemMapAddr;
										MemMapAddr++;
									}
								}
								else if(UART_DataLength==1)
								{
									if (RxBuff[5]==1)
									{
										VirtualFG.VFG_First=1;
										VirtualFG.VFG_En=1;
										VirtualFG.VFG_IT_Running=0;
										VirtualFG.VFG_IT_Done=0;
										VirtualFG.VFG_Reading=0;
									}
									else if (RxBuff[5]==0)
									{
										VirtualFG.VFG_En=0;
										VirtualFG.VFG_IT_Running=0;
										VirtualFG.VFG_IT_Done=0;
										VirtualFG.VFG_Reading=0;
										VirtualFG.VFG_First=0;
									}
								}
								break;
						case 0x7FEE:
							if (UART_Mode)
							{
								TxBuff[5]=IsenseR;
							}
							else
							{
								if (RxBuff[5]==0)
								{
									IsenseR=1;
								}
								else
								{
									IsenseR=RxBuff[5];
								}

								MP279xCConfigCalc();
								Reconfig=1;
								SaveConfigtoFLASH();
							}
							break;
						case 0x7FFE:
							//Send Trigger FG command to MPF4279x
							if (VirtualFG.VFG_En)
							{
								if ((!VirtualFG.VFG_IT_Running) && (!VirtualFG.VFG_IT_Done))
								{
									VirtualFG.VFG_IT_Running=1;
									SendMeasVFG();
								}
							}
							else
							{
								if(I2C_MPF4279x_Write(I2C1, MPF4279x_Address, Reg_Addr, &(RxBuff[5]) , UART_DataLength, 1, MPF4279x_CRC_EN))
								{
									I2Cerror=1;
									Succ=0;
								}
							}
							break;
						//BMS Commands 
						case 0x7FF0:
							I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_PWR_STATUS, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1);
							if (!(DataTemp&0x0080)) StandByFlag=1;
							break;
						case 0x7FF1:
							if ((BMSData.BMSstatus==DCHG)||(BMSData.BMSstatus==CHG)||(BMSData.BMSstatus==ALARM)) SafeFlag=1;
							else SafeFlag=0;
							break;
						case 0x7FF2:
							DefaultConfig=1;
							InitConfiguration();
							Reconfig=1;
							break;
						case 0x7FF3:
							SysTick_Delay_us(100);
							DMA1_Channel2 -> CNDTR = 10;													//The transmit buffer has a byte size equals to the defined as BT_LENGTH_TX
							DMA1_Channel2 -> CMAR = (uint32_t) (Tx_ACK);
							DMA1_Channel2 -> CCR |= DMA_CCR_EN;											//Enable DMA Channel 4
							USART1 -> ISR &= ~(USART_ISR_TXE);												//Clear the UART TX buffer Empty register to send the UART message using DMA
							SysTick_Delay_us(200);
							while(!(USART1->ISR&USART_ISR_TC)) Nop();
							NVIC_SystemReset();
							break;
						case 0x7FF4:
							BMSData.CC_Status.CC_Running=(!BMSData.CC_Status.CC_Running);
							break;
						case 0x7FF5:
							BMSData.CC_Status.CC_OverFlow=0;
							CC_BMS_Accum=0;
							CC_BMS_Cntr=0;
							break;
						case 0x7FF6:
							Config.I_Offset=ITOP_Temp;
							SaveConfigtoFLASH();
							break;
						}
						LL_I2C_Disable(I2C1);		//DISABLE the I2C channel
					}
				}
				break;
			}
		}
	}
	
	if(Succ)
	{
		if (!UART_Mode)
		{
			DMA1_Channel2 -> CNDTR = 10;																	//The transmit buffer has a byte size equals to the defined for ACK and NACK messages
			DMA1_Channel2 -> CMAR = (uint32_t) (Tx_ACK);
		}
		else
		{
			UARTReadResponse();
		}
	}
	else
	{
		DMA1_Channel2 -> CNDTR = 10;																	//The transmit buffer has a byte size equals to the defined fo ACK and NACK messages
		DMA1_Channel2 -> CMAR = (uint32_t) (Tx_NACK);
	}
	DMA1_Channel2 -> CCR |= DMA_CCR_EN;													//Enable DMA Channel 2
	USART1 -> ISR &= ~(USART_ISR_TXE);												//Clear the UART TX buffer Empty register to send the UART message using DMA
	if (StandByFlag)
	{
			SysTick_Delay_us(100);
			while(!(USART1->ISR&USART_ISR_TC)) Nop();
	}
}

//Answer with the Data after a UART read request
void UARTReadResponse(void)
{
	CRC->CR|=CRC_CR_RESET;
	CRCInputUART=0;
	TxBuff[1]=RxBuff[1];
	TxBuff[2]=RxBuff[2];
	TxBuff[3]=RxBuff[3]|0x01;
	TxBuff[4]=RxBuff[4];
	TxBuff[UART_DataLength+9]=0x0A;
	CRC->DR=((TxBuff[4]<<24)|(TxBuff[3]<<16)|(TxBuff[2]<<8)|(TxBuff[1]));
	for(unsigned short i=5; i<(UART_DataLength+5); i++)
	{
		CRCInputUART|=(TxBuff[i]<<((i-5)%4)*8);
		if ((((i-5)%4)==3)|(i==(UART_DataLength+4)))
		{
			CRC->DR=CRCInputUART;
			CRCInputUART=0;
		}
	}
	TxBuff[UART_DataLength+5]=(uint8_t)(CRC->DR&0xFF);
	TxBuff[UART_DataLength+6]=(uint8_t)((CRC->DR&0xFF00)>>8);
	TxBuff[UART_DataLength+7]=(uint8_t)((CRC->DR&0x00FF0000)>>16);
	TxBuff[UART_DataLength+8]=(uint8_t)((CRC->DR&0xFF000000)>>24);
	DMA1_Channel2 -> CNDTR = UART_DataLength+10;
	DMA1_Channel2 -> CMAR = (uint32_t) (TxBuff);
}

//RTC Interrupt Handler
void RTC_IRQHandler(void)
{
	uint16_t Dummy;
	uint16_t DataTemp;
	if ((RTC->ISR & RTC_ISR_ALRAF)) //If is caused by RTC Alarm A
	{
		RTC->ISR&=~RTC_ISR_ALRAF;				//Reset the RTC RTC Alarm A flag
		EXTI->PR|=(1<<17);
		MP279x_I2C_Busy=1;							//SET the MP279x I2C to busy
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_BAL_STS, (uint8_t *)&DataTemp, (uint8_t *)&Dummy, USE_CRC, 1); //Read the Balancing status 
		if (DataTemp&0x01)							//Check if a balancing session is ongoing
		{
			BalCounter++;
		}
		if(!(RTC_IT%4)) I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1); //Reset the WDT

		if ((RTC_IT%5)==1)
		{
			if (ITCount==1)
			{
				ITCount=0;
				#ifdef High_P
				//Control the displayed LEDs
				if (BMSData.BMSstatus==SAFE)
				{
					if (LED1_ON) RSET_LD1;
					else SET_LD1;
					RSET_LD0;
				}
				else
				{
					if (LED0_ON) RSET_LD0;
					else SET_LD0;
					RSET_LD1;
				}
				#else
				if (LED0_ON)
				{
					RSET_LD0;
					RSET_LD2;
				}
				else
				{
					SET_LD0;
					if (BMSData.BMSstatus==SAFE)
					{
						if (Config.Features_Enable&0x06)
						{
							I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, 0x01, 0x83, USE_CRC, 1);		//Clear Configuration
							SysTick_Delay_us(10);
							I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, Config.Features_Enable&0x06, 0x83, USE_CRC, 1); //Charger and load detection activated.
						}
						SET_LD2;
					}
					else
					{
						RSET_LD2;
					}
				}
				#endif
			}
			else ITCount++;
		}
		else if (!(RTC_IT%5))
		{
			if ((!((BMSData.BMSstatus==SAFE)||(BMSData.BMSstatus==ALARM)))||RTC_IT2)
			{
				if (!RTC_IT2)
				{
					#ifdef ENABLE_CC
					I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, (uint8_t *)&DataTemp, ((uint8_t *)&DataTemp + 1), USE_CRC, 1);
					if (DataTemp&0x0030)
					{
						I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x02, 0x4E, USE_CRC, 1);			//Disable Coloumb Counter to get rid of CC error/Complete flag.
						SysTick_Delay_us(100);
						I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x03, 0x4E, USE_CRC, 1);			//Enable Coloumb Counter
					}
					else if ((DataTemp&0x7F2B)!=0x4E03)
					{
						I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x03, 0x4E, USE_CRC, 1);			//Enable Coloumb Counter
					}
					#endif
					OWD=0;
				}
				
				//ADC Watchdog:
				if (ADC_Req)
				{
					if (ADC_WDG<200) ADC_WDG+=20;
					if (ADC_WDG>=100) 
					{
						BMSAlarmHandler(CHIP_ERROR);
					}
				}
				else if (ADC_WDG>1) ADC_WDG--;
				
				//Trigger ADC scan:
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_CLR, 0x00, 0x02, USE_CRC, 1); //Clear the scheduler error interrupt
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CTRL, 0x00, 0x00, USE_CRC, 1);	//Stop balancing before the MP279x reads the ADC measurements
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SFT_GO, 0x00, 0x00, USE_CRC, 1);		//Stops any OPW process
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, 0x00, 0x00, USE_CRC, 1);	//Clear all REG_ADC_CTRL bits before sending the ADC_Go command
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, 0x01, 0x00, USE_CRC, 1);	//Send the ADC_SCAN_GO command to the MP279x to read the ADC measurements
				ADCNew=1;
				ADC_Req=1;
				RTC_IT2=0;
			}
			else
			{
				if (OWD>=9)
				{
					if (Config.Features_Enable&0x01)
					{
						I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, (uint8_t *)&DataTemp, (uint8_t *)&Dummy, USE_CRC, 0);
						I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_ADC_STS, (uint8_t *)&Dummy, ((uint8_t *)&DataTemp)+1, USE_CRC, 1);
						I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_INT1, (uint8_t *)&Dummy, ((uint8_t *)&Dummy)+1, USE_CRC, 1);
						uint16_t DataTemp2;
						I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, (uint8_t *)&DataTemp2, ((uint8_t *)&DataTemp2)+1, USE_CRC, 1); //Read the CC status
						if ((!(DataTemp&0x05))&&(!(Dummy&0200))&&(!(DataTemp2&0x01)))// Check that the OPW and the CC is not working at the same time and ADC is working properly
						{
							SysTick_Delay_us(10);
							I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, 0x00, 0x00, USE_CRC, 1);	//Clear all REG_ADC_CTRL bits
							I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CTRL, 0x00, 0x00, USE_CRC, 1); //Stop balancing for reading voltages
							I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SFT_GO, 0x00, 0x00, USE_CRC, 1);		//Clear all REG_SFT_GO bits before the OPW detection
							I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SFT_GO, 0x00, 0x01, USE_CRC, 1);		//Run an OPW detection 
							OWD=0;
						}
						else
						{
							I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x02, 0x4E, USE_CRC, 1); // Stop the CC
							Measurements.I_CC=0;
							CC_Accum=CCCount=0;
						}
					}
				}
				else
				{
					if (OWD>=5)
					{
						I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x02, 0x4E, USE_CRC, 1); // Stop the CC
						Measurements.I_CC=0;
						CC_Accum=CCCount=0;
					}
					OWD++;
				}
				RTC_IT2=1;
			}
		}
		if (RTC_IT>=19) RTC_IT=0;
		else RTC_IT++;

		if (xALARM_Pending) //Check if the xALARM pending flag is reported due to an I2C Busy flag
		{
			xALARM_Handle();	//Run the Interrupt Alarm check function
			xALARM_Pending=0;
		}
		if (Fault) 					//Check if the Fault flag is reported due to an I2C Busy flag
		{
			Fault_Handle();		//Run the Fault check function		
			Fault=0;
		}
		//Read and Write registers while debugging 
		if (Read)
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,Register, &DataRx[0], &DataRx[1], USE_CRC, 1);
			Read=0;
		}
		if (Write)
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,Register, DataTx[0], DataTx[1], USE_CRC, 1);
			Write=0;
		}
#ifdef Use_PreChg
		if (PreChg_State==2)
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x08, 0x00, USE_CRC, 1);
			PreChg_State=0;
		}
		else if (PreChg_State==1)
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x00, 0x00, USE_CRC, 1);
			PreChg_State=0;
		}
#endif
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_ADC_STS, (uint8_t *)&Dummy, ((uint8_t *)&DataTemp)+1, USE_CRC, 1);
		MP279x_I2C_Busy=0;					//Release the I2C Busy flag

		//FG Read and Send command:
		if (!VirtualFG.VFG_En){			//VFG disabled
			if ((FGReady || (FG_Cntr>=(FG_Ts-2))) && FirstSoC)
			{
				LL_I2C_Enable(I2C1);			//ENABLE the I2C channel
				ReadFGResultsMPF4279x();		//Read MPF4279x FG Results
				LL_I2C_Disable(I2C1);			//DISABLE the I2C channel
			}
			if (((FG_IT>=(FG_Ts-1)) || FG_In_Send_Pend) && New_Meas)
			{
				LL_I2C_Enable(I2C1);			//ENABLE the I2C channel
				if (FG_IT>=(FG_Ts-1)) 
				{
					FG_In_Send_Pend = FG_Cntr = FG_IT = 0;
				}
				if (FG_In_Send_Pend)
				{
					FG_IT++;
					FG_Cntr++;
				}
				SendFGDataMPF4279x();			//Send measurements to the MPF4279x FG
				LL_I2C_Disable(I2C1);			//DISABLE the I2C channel
			}
			else
			{
				FG_IT++;
				FG_Cntr++;
			}
		}
		//Virtual FG handle:
		else if (VirtualFG.VFG_IT_Running && !VirtualFG.VFG_IT_Done && !VirtualFG.VFG_Reading)
		{
			ReadRsltsVFG();
		}

		//StandBy Flag handle:
		if ((Config.Features_Enable&0x08)&&(BMSData.AFE_Status&0x40))
		{
			if (SbyCounter>=(Config.Sby_Time*10)) StandByFlag=1;
			else SbyCounter++;
		}
		else SbyCounter=0;
		if (DisableSby) DisableSby=0;
	}
}

//Handler of MP279x Faults
void Fault_Handle(void)
{
	uint16_t DataTemp;
	uint8_t FT_Read_Error=1, FT_Read_Cntr=0;
	//Read the Fault registers that can trigger the fault interrupt (GPIO3)
	do {
		FT_Read_Error=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_FT_STS1, (uint8_t *) (&Faults), ((uint8_t *) &Faults) + 1, USE_CRC, 1);
		FT_Read_Error|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_FT_STS2, ((uint8_t *) (&Faults)) + 2, ((uint8_t *) &Faults) + 3, USE_CRC, 1);
		FT_Read_Cntr++;
	} while (FT_Read_Error&&(FT_Read_Cntr<5));
	if (Faults&0x0001)				//CELL UNDER VOLTAGE FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x01))		//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{
			SET_LD3;
			BMSData.BMSstatus=AUTO_RECOVERY;																		//Set the Auto Recovery state
		}
		else BMSAlarmHandler(CELL_UV);																				//Run the BMSAlarmHandler
	}
	if (Faults&0x0020)			//PACK UNDER VOLTAGE FAULT	
	{
		BMSAlarmHandler(PACK_UV);																							//Run the BMSAlarmHandler
	}
	if (Faults&0x0004)			//CELL DEAD FAULT
	{
		BMSAlarmHandler(CELL_DEAD);																						//Run the BMSAlarmHandler
	}
	if (Faults&0x0002)			//CELL OVER VOLTAGE FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x02))		//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{
			SET_LD3;
			BMSData.BMSstatus=AUTO_RECOVERY;																		//Set the Auto Recovery state
		}
		else BMSAlarmHandler(CELL_OV);																				//Run the BMSAlarmHandler
	}
	if (Faults&0x0040)			//PACK OVER VOLTAGE FAULT
	{
		BMSAlarmHandler(PACK_OV);																							//Run the BMSAlarmHandler
	}
	if (Faults&0x0400)			//DISCHARGE OVER CURRENT 1 FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x04)&&(!(Faults&(1<<28))))	//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{
			SET_LD3;
			BMSData.BMSstatus=AUTO_RECOVERY;																		//Set the Auto Recovery state
			DOC_AutoRec=1;
			BMSData.RT_Alarms.DOC=1;																						//Set the Real time Alarm
		}
		else
		{
			BMSAlarmHandler(DOC);																								//Run the BMSAlarmHandler
			DOC_AutoRec=0;
		}
	}
	if (Faults&0x0800)			//DISCHARGE OVER CURRENT 2 FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x08)&&(!(Faults&(1<<28))))	//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{
			SET_LD3;
			BMSData.BMSstatus=AUTO_RECOVERY;																		//Set the Auto Recovery state
			DOC_AutoRec=1;
			BMSData.RT_Alarms.DOC=1;																						//Set the Real time Alarm
		}
		else
		{
			BMSAlarmHandler(DOC);																								//Run the BMSAlarmHandler			
			DOC_AutoRec=0;
		}
	}
	if (Faults&0x2000)			//DISCHARGE SHORT CIRCUIT FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x20)&&(!(Faults&(1<<26)))) //Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{
			SET_LD3;
			BMSData.BMSstatus=AUTO_RECOVERY;																	//Set the Auto Recovery state
			DSC_AutoRec=1;                                                    
			BMSData.RT_Alarms.DSC=1;                                          //Set the Real time Alarm
		}                                                                   
		else                                                                
		{                                                                   
			BMSAlarmHandler(DSC);                                             //Run the BMSAlarmHandler			
			DSC_AutoRec=0;
		}
	}
	if (Faults&0x1000)			//CHARGE OVER CURRENT FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x10)&&(!(Faults&(1<<27))))	//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{
			SET_LD3;
			BMSData.BMSstatus=AUTO_RECOVERY;																	//Set the Auto Recovery state
			COC_AutoRec=1;                                                    
			BMSData.RT_Alarms.COC=1;                                          //Set the Real time Alarm
		}                                                                   
		else                                                                
		{                                                                   
			BMSAlarmHandler(COC);                                             //Run the BMSAlarmHandler			
			COC_AutoRec=0;
		}
	}
	if (Faults&0x4000)			//CHARGE SHORT CIRCUIT FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x40)&&(!(Faults&(1<<25))))	//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{
			SET_LD3;
			BMSData.BMSstatus=AUTO_RECOVERY;																	//Set the Auto Recovery state
			CSC_AutoRec=1;                                                    
			BMSData.RT_Alarms.CSC=1;                                          //Set the Real time Alarm
		}                                                                   
		else                                                                
		{                                                                   
			BMSAlarmHandler(CSC);                                             //Run the BMSAlarmHandler			
			CSC_AutoRec=0;
		}
	}
	if (Faults&0x0100)			//BAT OVER TEMPERATURE FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x80))	//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{                                                                   
			SET_LD3;                                                          
			BMSData.BMSstatus=AUTO_RECOVERY;                                  //Set the Auto Recovery state
		}                                                                   
		else BMSAlarmHandler(BAT_OT);                                       //Run the BMSAlarmHandler
	}
	if (Faults&0x0200)			//BAT UNDER TEMPERATURE FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x80))	//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{                                                                   
			SET_LD3;                                                          
			BMSData.BMSstatus=AUTO_RECOVERY;                                  //Set the Auto Recovery state
		}                                                                   
		else BMSAlarmHandler(BAT_UT);                                       //Run the BMSAlarmHandler
	}
	if (Faults&0x0080)			//PCB MONITOR NTC FAULT
	{
		if ((BMSData.BMSstatus!=ALARM)&&(Config.Faults_AutoRecovery&0x0200))//Check if the fault autorecovery is activated and the BMS is not already in ALARM state
		{                                                                   
			SET_LD3;                                                          
			BMSData.BMSstatus=AUTO_RECOVERY;                                  //Set the Auto Recovery state
		}                                                                   
		else BMSAlarmHandler(PCB_TEMP);                                     //Run the BMSAlarmHandler
	}
	if (Faults&0x8000)			//MP279X CHIP TEMPERATURE FAULT
	{
		BMSAlarmHandler(CHIP_TEMP);																					//Run the BMSAlarmHandler
	}
	if (Faults&0x0010)			//OPEN WIRE FAULT			
		{
			BMSAlarmHandler(OPEN_W);																					//Run the BMSAlarmHandler
	}
	if (Faults&0x0008)			//CELL MISMATCH FAULT
	{
		BMSAlarmHandler(CELL_MSMT);																					//Run the BMSAlarmHandler
	}
	FT_Read_Error|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_FET_STATUS, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1);	//Check FET Status
	if ((DataTemp&0x0008)||(Faults&0x00080000))			//FET DRIVER FAULT
	{
		BMSAlarmHandler(FET_DRIVER);
	}
	if ((Faults&0x00070000)||FT_Read_Error) BMSAlarmHandler(CHIP_ERROR);		//CHIP FAULT
	if (StandByFlag && ((BMSData.BMSstatus==ALARM) || (BMSData.BMSstatus==AUTO_RECOVERY))) StandByFlag=0;
}

//Handler of MP279x interrupts
void xALARM_Handle(void)
{
	uint16_t DataTemp;
	I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_WDT_STATUS, &WDT_STS[0], &WDT_STS[1], USE_CRC, 1);
	//Read the Interrupt Status
	I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_INT0, ((uint8_t *)&Interrupts), ((uint8_t *)&Interrupts)+1, USE_CRC, 1);
	I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_INT1, ((uint8_t *)&Interrupts)+2, ((uint8_t *)&Interrupts)+3, USE_CRC, 1);
	
	//Handling OC & SC with interrupt (discontinued)
	if (Interrupts&0x0020)				//SHORT CIRCUIT INTERRUPT	
	{
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_SC_STATUS, (uint8_t *)&DataTemp, ((uint8_t *)&DataTemp)+1, USE_CRC, 1); //Read the SC register state
		if (DataTemp&0x01) BMSAlarmHandler(DSC);	//Run Alarm handler for Discharge SC
		if (DataTemp&0x02) BMSAlarmHandler(CSC);	//Run Alarm handler for Charge SC
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x20, 0x00, USE_CRC, 1); //Clear the interrupt
	}
	if (Interrupts&0x0010)				//OVER CURRENT INTERRUPT
	{
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_OC_STATUS, (uint8_t *)&DataTemp, ((uint8_t *)&DataTemp)+1, USE_CRC, 1);	//Read the OC register state
		if (DataTemp&0x03) BMSAlarmHandler(DOC);	//Run Alarm handler for Discharge OC
		if (DataTemp&0x04) BMSAlarmHandler(COC);  //Run Alarm handler for Charge OC
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x10, 0x00, USE_CRC, 1);	//Clear the interrupt
	}

	if (Interrupts&0x0100)				//WDT BARK/BITE
	{
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x01, USE_CRC, 1); //Clear the Interrupt
		if (StandByFlag)	//Check the StandByFlag
		{
			#ifdef WDG_EN
			IWDG->KR=WDG_KEY_RELOAD;																		//Reload IWDG timer counter
			#endif
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_RST, 0x01, 0, USE_CRC, 1);		//Reset the WDT
		}
		BMSData.WDT_Int_Counter++;
	}
	if (Interrupts&0x0800)				//COMPLETED ADC_SCAN
	{
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x08, USE_CRC, 1);	//Clear the Interrupt
		if (!(Interrupts&0x00100000))	//Check that the OPW interrupt has not been triggered 
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, (uint8_t *)&DataTemp, ((uint8_t *)&DataTemp)+1, USE_CRC, 1);
			if (DataTemp&0x02)	//Check that the adc scan has finished
			{
				if (BMSData.BMSstatus!=ALARM) {
#ifdef MP279x_AutoBalance
					I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CTRL, ((Config.Features_Enable & 0x80)>>7) | 0x02 , 0x00, USE_CRC, 1); //Active Balancing if the IC is out of ALARM
#else
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CTRL, 0x00, 0x00, USE_CRC, 1);				
#endif
				}
				ReadADCResults();	//Run the Read ADC function to classify the data
					
			}
			BMSData.VScan_Int_Counter++;
		}
	}
	//COMPLETED COULOMB COUNTER 
	if (Interrupts&0x1000)
	{
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x10, USE_CRC, 1);	//Clear the Interrupt
		uint32_t CC_Bits;
		int32_t CC_Temp;
		//Read the CC registers
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CCIRQL, (uint8_t *)&CC_Bits, ((uint8_t *)&CC_Bits)+1, USE_CRC, 1);
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CCIRQH, ((uint8_t *)&CC_Bits)+2, ((uint8_t *)&CC_Bits)+3, USE_CRC, 1);
		//Calculate the CC value
		if (CC_Bits&(1<<25))
		{
			CC_Temp=((CC_Bits&0x03FFFFFF)|0xFC000000);
		}
		else
		{
			CC_Temp=(CC_Bits&0x03FFFFFF);
		}
		CC_Temp=((CC_Temp*100000.0/32768.0)/I_SENSE_R)+(Config.I_Offset*240);
		CC_Accum+=CC_Temp;
		CCCount++;
		if (BMSData.CC_Status.CC_Running&&(!BMSData.CC_Status.CC_OverFlow))
		{
			if ((CC_Temp>1500) || (CC_Temp<(-1500))) CC_BMS_Accum+=CC_Temp;
			CC_BMS_Cntr++;
			int64_t CC_BMS_Prov=CC_BMS_Accum/(500*360);
			uint32_t CC_BMS_Cntr_Prov=(CC_BMS_Cntr*(480.0/1000));
			if (CC_BMS_Prov>0x7FFFFFFF)
			{
				BMSData.CC=0x7FFFFFFF;
				BMSData.CC_Status.CC_OverFlow=1;
			}
			else if ((-CC_BMS_Prov)>0x8000000)
			{
				BMSData.CC=0x80000000;
				BMSData.CC_Status.CC_OverFlow=1;
			}
			else BMSData.CC=CC_BMS_Prov;
			if (CC_BMS_Cntr_Prov>0x3FFFFFFF)
			{
				BMSData.CC_Status.CC_Cntr=0x3FFFFFFF;
				BMSData.CC_Status.CC_OverFlow=1;
			}
			else BMSData.CC_Status.CC_Cntr=CC_BMS_Cntr_Prov;
		}
	}
	if (Interrupts&0x0400) 				//CHANGE IN AFE STATE
	{
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x04, USE_CRC, 1);	//Clear the Interrupt
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_PWR_STATUS, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1); //Read the MP279x state
		BMSData.AFE_Status = (DataTemp&0x001F) | ((DataTemp&0x0380) >> 2);	//Save the MP279x state
		if ((DataTemp&0x08)&&(!((BMSData.BMSstatus==ALARM) || (BMSData.BMSstatus==AUTO_RECOVERY)))) Fault_Handle();	//Check the fault function if the MP279x is in Fault State
		if (StandByFlag && ((PFET_On&&(!(DataTemp&0x02)))||((!PFET_On)&&(!(DataTemp&0x01)))))
		{
			Sby_Exit|=0x01;
			StandByFlag=0;
		}
		BMSData.AFE_Mode_Int_Counter++;
	}
	if (Interrupts&0x4000) 				//CHANGED PACK CURRENT STATUS
	{
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x40, USE_CRC, 1);	//Clear the Interrupt
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_PWR_STATUS, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1);	//Check the MP279x Status
		if (StandByFlag && (!(DataTemp&0x0100)))	//MP279x in Recovery state and StandByFlag
		{
#ifdef Direct_Mode
			ENABLE_CHG;
			ENABLE_DCH;
#else
			ENABLE_FETs;		//Enable the FETs
#endif
			Sby_Exit|=0x02;
			StandByFlag=0;
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_EN, 0x00, 0xBD, USE_CRC, 1);
		}
		BMSData.Pack_I_Int_Counter++;
	}
	if (Interrupts&0x2000) 				//DETECTED CHARGER OR/AND LOAD
	{
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0x00, 0x20, USE_CRC, 1);	//Clear the Interrupt
		if ((BMSData.BMSstatus==SAFE)	|| StandByFlag)
		{
			SysTick_Delay_us(10);
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_STATUS, (uint8_t *) &BMSData.LC_Status, ((uint8_t *) &BMSData.LC_Status)+1, USE_CRC, 1);
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_STB_STATUS, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1); // it returns if VPACK>VTOP... in 10:8
			BMSData.LC_Status&=~(0x0700); // clear the bits in LC_Status (the important bits are only 12:15)
			BMSData.LC_Status|=DataTemp&0x0700; 
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1);
			BMSData.LC_Status|=(((~DataTemp)&0x0008)<<4);
			if (BMSData.LC_Status&0xC000){ //Check if there are any load o charger detected.
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x08, 0x00, USE_CRC, 1);
				if (!StandByFlag)
				{
					if (BMSData.LC_Status&0x4000) {		//Load detected
						BMSData.BMSstatus=WAKE_UP;						//Wake up in normal state
#ifdef Use_PreChg
						I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x00, 0x00, USE_CRC, 0);
#endif
						Wake_up_DSC_done=1;
						SafeFlag=0;
					}
					else if (BMSData.LC_Status&0x8000) {   //Charger detected
						BMSData.BMSstatus=WAKE_UP_CHG;				//Wake up in charge state. Avoid the pre-charge.
						Wake_up_CHG_done=1;
						SafeFlag=0;
					}
				}
				else
				{
					Sby_Exit|=0x01;
					StandByFlag=0;
				}
				BMSData.Conn_Int_Counter++;
			}
			else if (BMSData.LC_Status&0x3000) {		//Fault detected
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, 0x01, 0x83, USE_CRC, 1);		//Clear Configuration
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, (Config.Features_Enable&0x06) | (StandByFlag ? 0x20 : 0), 0x83, USE_CRC, 1); //Charger and load detection activated.
			}
		}
		else {
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, 0x01, 0x83, USE_CRC, 1); //Clear Configuration
//			SysTick_Delay_us(10);
//			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, Config.Features_Enable&0x06, 0x83, USE_CRC, 1); //Charger and load detection activated.
		}
	}
	if (Interrupts&0x04000000)		//CHIP SENSE INTERRUPT (SELF TEST)
	{
		BMSAlarmHandler(CHIP_SENSE_ERROR);	
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_CLR, 0x00, 0x04, USE_CRC, 1);
	}
	if (Interrupts&0x00100000)		//OPEN WIRE INTERRUPT
	{
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_SFT_GO, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp)+1, USE_CRC, 1);
		if (DataTemp&0x0200)
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_OPENH, (uint8_t *) BMSData.OpenWireDet, ((uint8_t *) BMSData.OpenWireDet) + 1, USE_CRC, 1);
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_OPENL, ((uint8_t *) BMSData.OpenWireDet) + 2, (uint8_t *) &DataTemp, USE_CRC, 1);
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SFT_GO, 0x00, 0x00, USE_CRC, 1);
			BMSData.OpenWireDet[2]|=0x80;
			if (BMSData.OpenWireDet[0]||BMSData.OpenWireDet[1]||(BMSData.OpenWireDet[2]&0x01)) BMSData.RT_Alarms.OW=1;
			else	BMSData.RT_Alarms.OW=0;
		}
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_CLR, 0x10, 0x00, USE_CRC, 1);
		OWD=0;
	}
	if (Interrupts&0x20000000)		//FET DRIVER INTERRUPT
	{
		BMSAlarmHandler(FET_DRIVER);		//Run the BMSAlarmHandler
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_CLR, 0x00, 0x20, USE_CRC, 1); //Clear the Interrupt
	}
	if (Reconfig==1)							//MP279x RECONFIGURATION
	{
		MP279xCReConfig();
		if ((BMSData.BMSstatus==ALARM) || (BMSData.BMSstatus==AUTO_RECOVERY)) Reconfig=2;
		else Reconfig=0;
	}
	else if (Reconfig==2)
	{
		ReadADCResults(); //Needed for going to safe state after a reconfiguration in Alarm state
		ResetAlarms();
		Reconfig=0;
	}
}

//Initializes and configures the MP279x
void MP279xCInit(void)
{
	//Enable CRC and configure I2C Address; wait until everything is correct:
	do{
		I2C_MP279xC_Write16B(I2C2,1,REG_COMM_CONFIG, 0x04, ADDRESS_MP279x, USE_CRC, 1);
	}while (I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_COMM_CONFIG, &DataRx[0], &DataRx[1], USE_CRC, 1));

  //Configure the MP279x:
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_CONFIG, (Config.NumofSeries-1)&0x0F, 0, USE_CRC, 1);			//Configure Cells Series in use
#ifdef Direct_Mode
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ACT_CONFIG, 0x03, 0x02, USE_CRC, 1);				// Use GPIO to control FETs; Direct Mode; "All FETs Off"; Level Control
#else
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ACT_CONFIG, 0x01, 0x02, USE_CRC, 1);				// Use GPIO to control FETs; Simple Mode; "All FETs Off"; Level Control
#endif
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_STB_CONFIG, (0x08 | ((Config.Sby_I_Thr&0x03)<<1)), 0x00, USE_CRC, 1); //Sby Disabled; Configurable Sby I thres; Disabled Hyst (bit negated R1 issue); 1 Slot A each 4 Slot Bs
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_REG_CONFIG, 0x05, 0x00, USE_CRC, 1);		//0x01 only for External BJT available; 0x05 to also have 3.3V available in SHDN
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, 0x01, 0x83, USE_CRC, 1);	//Clear sts bit; Det not enabled; Dets. not started; Chg Det not enabled to exit Sby??????????????????????????????????; Config packp prechg.= 1.6 s; Enable packp comparator
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, 0x06, 0x83, USE_CRC, 1); //Charger and load detection activated.
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_CONFIG, 0x11, 0x0C, USE_CRC, 1);	//GPIO1 and 2 digital input, no pull-up; GPIO3 Output, with pull-up, FAULT Pin
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PINS_CONFIG, MP279xConfig.PINS_CONFIG, 0x00, USE_CRC, 1);	//xALERT Active Low; nSHDWN Pin Ctrl., Active Low; Configurable reaction to WDT Bite (MCU Rst or not3); 3.3V pull-up; GPIOHV is output
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x08, 0x00, USE_CRC, 1);	//GPIOHV set High to disale Precharge
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_WDT_CONFIG, 0x5C, 0x2E, USE_CRC, 1);	//Configure Watchdog: Not enabled yet, 1175 ms Bark, 1175 ms Bite
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x00, 0x00, USE_CRC, 1);		//PFET_Enable Disabled
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_MODE, MP279xConfig.FET_MODE&0x00FF, ((MP279xConfig.FET_MODE&0xFF00)>>8), USE_CRC, 1);		//Use configurable FET Mode Config.
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CONFIG, 0xF0, 0x7E, USE_CRC, 1);	//0xC0, 0x3E
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT_TYPE0, 0x50, 0x00, USE_CRC, 1);	//Use level as interrupt type except for NTC Hot& Cold->Rising Edge
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT_TYPE1, 0x00, 0x00, USE_CRC, 1);	//Use level as interrupt type 
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT_TYPE2, 0x01, 0x00, USE_CRC, 1);	//Use level as interrupt type except for Cell Dead->Rising Edge
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_MASK_INT0, 0x00, 0x00, USE_CRC, 1);	//Don't mask interrupts (added)
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_MASK_INT1, 0x00, 0x00, USE_CRC, 1);	//Don't mask interrupts
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DSGOC_LIM, MP279xConfig.DSGOC1_LIM, MP279xConfig.DSGOC2_LIM, USE_CRC, 1);	//Config DOC Limits
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DSGOC_DEG, MP279xConfig.DSGOC1_DEG, MP279xConfig.DSGOC2_DEG, USE_CRC, 1);	//Config DOC Deglitch Times
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CHGOC_DEG, MP279xConfig.CHGOC_LIM, MP279xConfig.CHGOC_DEG, USE_CRC, 1);	//Config COC Limit and Deglitch time
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DSGSC_CONFIG, MP279xConfig.DSGSC_LIM, MP279xConfig.DSGSC_DEG, USE_CRC, 1);		//Config DSC Limit and Deglitch time
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CHGSC_CONFIG, MP279xConfig.CHGSC_LIM, MP279xConfig.CHGSC_DEG, USE_CRC, 1);		//Config CSC Limit and Deglitch time
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACK_UV_OV, 0x00, 0x00, USE_CRC, 1);	//Don't apply Hysteresis yet
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_UV, MP279xConfig.CELL_UV, ((MP279xConfig.CELL_UV&0xFF00)>>8), USE_CRC, 1);	//Config Cell UV thres. and deglitch
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_OV, MP279xConfig.CELL_OV, ((MP279xConfig.CELL_OV&0xFF00)>>8), USE_CRC, 1);	//Config Cell OV thres. and deglitch
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACK_UV, MP279xConfig.PACK_UV&0x00FF, ((MP279xConfig.PACK_UV&0xFF00)>>8), USE_CRC, 1);	//Config Pack UV thres. and deglitch
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACK_OV, MP279xConfig.PACK_OV&0x00FF, ((MP279xConfig.PACK_OV&0xFF00)>>8), USE_CRC, 1);	//Config Pack OV thres. and deglitch
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_DEAD_THR,  MP279xConfig.CELL_DEAD_THR&0x00FF, ((MP279xConfig.CELL_DEAD_THR&0xFF00)>>8), USE_CRC, 1);	//Config Cell Dead thres. and deglitch
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_MSMT, MP279xConfig.CELL_MSMT&0x00FF, ((MP279xConfig.CELL_MSMT&0xFF00)>>8), USE_CRC, 1);	//Config Cell MSMT thres. and deglitch
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_NTC_CLR, 0x04, 0x00, USE_CRC, 1);	//Show RT Status for pcb_mntr_sts
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DIE_CONFIG, 0x0A, 0x00, USE_CRC, 1);        //Enable die_temp-dig and die_temp_dig fault
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_OTHR_DSG, MP279xConfig.OTHR_DSG&0x00FF, ((MP279xConfig.OTHR_DSG&0xFF00)>>8), USE_CRC, 1);	//Configure Cell DSG OT
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_UTHR_DSG, MP279xConfig.UTHR_DSG&0x00FF, ((MP279xConfig.UTHR_DSG&0xFF00)>>8), USE_CRC, 1);	//Configure Cell DSG UT
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_OTHR_CHG, MP279xConfig.OTHR_CHG&0x00FF, ((MP279xConfig.OTHR_CHG&0xFF00)>>8), USE_CRC, 1);	//Configure Cell CHG OT
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_UTHR_CHG, MP279xConfig.UTHR_CHG&0x00FF, ((MP279xConfig.UTHR_CHG&0xFF00)>>8), USE_CRC, 1);	//Configure Cell CHG UT and Cell OT/UT Hysteresis
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_NTCM_OTHR, MP279xConfig.NTCM_OTHR&0x00FF, ((MP279xConfig.NTCM_OTHR&0xFF00)>>8), USE_CRC, 1);	//Configure Monitor OT and Monitor OT Hysteresis
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DIE_OT, 0xE3, 0x82, USE_CRC, 1);		//Configure Die OT (0x2E3 = 88�C) and Die OT Hysteresis  (0x10 = 8.4�C)
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_OPEN_CFG, 0x01, 0x0B, USE_CRC, 1);	//SET opw_check_len=2ms (0xXX01) and opw_check_th=120mV (0x02XX) (approx.)
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_REGIN_UV, 0x55, 0x01, USE_CRC, 1);	//Configure REGIN_UV
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_V3P3_UV, 0xE8, 0x00, USE_CRC, 1);	//Configure V3P3_UV
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_VDD_UV, 0x7C, 0x00, USE_CRC, 1);	//Configure VDD_UV
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SELF_THR, 0x55, 0x65, USE_CRC, 1);	//Configure SELF_UV and SELF_OV
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_REC, MP279xConfig.FT_REC&0x00FF, ((MP279xConfig.FT_REC&0xFF00)>>8), USE_CRC, 1);	//Configurable Enables for Fault Recovery
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT0_CONFIG, MP279xConfig.FT0_CFG&0x00FF, ((MP279xConfig.FT0_CFG&0xFF00)>>8), USE_CRC, 1);	//Use less restrective Cell OT and UT limits in Sby; open both FETs for all faults; Configure Fault recovery options and configure Enables for Cell UV and Cell OV Autorecovery
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT1_CONFIG, 0x30, 0x40, USE_CRC, 1);	//R1 with autorecovery:  0x30, 0x40		R1:0x00, 0x20 	R0: 0x00, 0x28
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT2_CONFIG, 0xBB, 0x0B, USE_CRC, 1);	//R1 with autorecovery:  0xBB, 0x0B		R1:0x00, 0x00 	R0: 0x66, 0x06
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CC_CONFIG, 0x02, 0x4E, USE_CRC, 1);	//CC not enabled yet; CC Enabled in Safe; Don't use power saving mode; Accumulate for 480 ms in back-to-back mode
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_TRIMG_IPCB, MP279xConfig.TRIMG_IPCB&0x00FF, ((MP279xConfig.TRIMG_IPCB&0xFF00)>>8), USE_CRC, 1);	//Configure I_ADC_Trim (Gain)
	//Activate all channels except for GPIOs:
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_HRSCAN0, 0x77, 0x03, USE_CRC, 1);	//And don't use 1k res compensation
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_HRSCAN1, 0xFF, 0xFF, USE_CRC, 1);
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_HRSCAN2, 0xE0, 0x01, USE_CRC, 1);
	
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_LIST, 0x00, 0x00, USE_CRC, 1);	//Don't balance any cell
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CTRL, 0x00, 0x00, USE_CRC, 1);	//Disable Balancing
#ifdef MP279x_AutoBalance
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CONFIG, (Config.Features_Enable&0x80) ? 0xFE	: 0x00, 0x00, USE_CRC, 1);	////Auto-Balance with autobal_always (and bal_repetition set to 32 rep.)
#else
    I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CONFIG, 0xF8, 0x00, USE_CRC, 1);		//Control Balancing with register; Use manual balancing; 32 balancing cycles
#endif
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_THR, MP279xConfig.BAL_THR&0x00FF, ((MP279xConfig.BAL_THR&0xFF00)>>8), USE_CRC, 1);	//Configure balancing

	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, 0x00, 0x00, USE_CRC, 1);	//Don't start ADC High Res Scan

	//Clear->Configure&Enable->Clear Faults&Interrupts
	for (char i=0;i<2;i++)
	{
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_CLR, 0xFF, 0xFF, USE_CRC, 1);
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_CLR, 0xFF, 0xFF, USE_CRC, 1);
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0xFF, 0xFF, USE_CRC, 1);
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_OCFT_CTRL, MP279xConfig.OCFT_CTRL&0x00FF, ((MP279xConfig.OCFT_CTRL&0xFF00)>>8), USE_CRC, 1);		// To put MP279x in fault mode:  0xFF, 0x01
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SCFT_CTRL, MP279xConfig.SCFT_CTRL&0x00FF, ((MP279xConfig.SCFT_CTRL&0xFF00)>>8), USE_CRC, 1);		// To put MP279x in fault mode:  0x3F, 0x00
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, MP279xConfig.CELLFT_CTRL&0x00FF, ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);	//0xB6, 0x1A; 0xB6, 0x02,
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACKFT_CTRL, MP279xConfig.PACKFT_CTRL&0x00FF, ((MP279xConfig.PACKFT_CTRL&0xFF00)>>8), USE_CRC, 1);	//0x77, 0x03	//Think what to do about Cell MSMT
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_NTC_CONFIG, MP279xConfig.NTC_CONFIG&0x00FF, ((MP279xConfig.NTC_CONFIG&0xFF00)>>8), USE_CRC, 1);	//0xD5, 0xE0;, 0xD5, 0xC0
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SELF_CFG, MP279xConfig.SELF_CFG, 0xC3, USE_CRC, 1); //0x4F, 0xC0
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT0_EN, 0x00, 0xBD, USE_CRC, 1);	//Enabled following interrupts: WDT, AFE MODE, VSCAN DONE, CC_ACC, CONN_DET, INT_ALERT
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_EN, 0x10, 0x24, USE_CRC, 1);	//Enabled following interrupts: OPEN WIRE INT, SELF TEST, FET DRIVER 
		if (i==0) SysTick_Delay_us(1000000);
	}
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG, 0x00, USE_CRC, 1);	//Enable Slot A in Safe, configure if SC&OC will be checked in Safe
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_HYST, MP279xConfig.CELL_HYST, ((MP279xConfig.CELL_HYST&0xFF00)>>8), USE_CRC, 1);	//Configure Here Cell Hysteresis
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACK_UV_OV, MP279xConfig.PACK_HYST, ((MP279xConfig.PACK_HYST&0xFF00)>>8), USE_CRC, 1);	//Configure Here Pack Hysteresis
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, 0x01, 0x00, USE_CRC, 1);	//Trigger the First High Res. Scan
}

//Reconfiguration of the MP279x:
void MP279xCReConfig(void)
{
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_CONFIG, (Config.NumofSeries-1)&0x0F, 0, USE_CRC, 1);			//Configure Cells Series in use
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_STB_CONFIG, (0x08 | ((Config.Sby_I_Thr&0x03)<<1)), 0x00, USE_CRC, 1); //Configure the Standby Current
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_CONFIG, ((BMSData.BMSstatus==SAFE) ? (Config.Features_Enable&0x06) : 0x01), 0x83, USE_CRC, 1); //Configurable load and charger detection
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG, 0x00, USE_CRC, 1);	// 0x05, 0x00
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PINS_CONFIG, MP279xConfig.PINS_CONFIG, 0x00, USE_CRC, 1);	//Was: MP279xConfig.PINS_CONFIG, 0x04 -> 0x04 was used because of the inverted logic on R0		//Think about the use of the WDT pin	0x00, 0x04, to not toggle WDT 0x02, 0x04 to toggle WDT
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_MODE, MP279xConfig.FET_MODE&0x00FF, ((MP279xConfig.FET_MODE&0xFF00)>>8), USE_CRC, 1);		//0x03, 0x01
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DSGOC_LIM, MP279xConfig.DSGOC1_LIM, MP279xConfig.DSGOC2_LIM, USE_CRC, 1);	//0x0F, 0x0F	//Fet configurable
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DSGOC_DEG, MP279xConfig.DSGOC1_DEG, MP279xConfig.DSGOC2_DEG, USE_CRC, 1);
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CHGOC_DEG, MP279xConfig.CHGOC_LIM, MP279xConfig.CHGOC_DEG, USE_CRC, 1);	//0x0B, 0x4C,	//Fet Configurable
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CHGSC_CONFIG, MP279xConfig.CHGSC_LIM, MP279xConfig.CHGSC_DEG, USE_CRC, 1);		// 0x0F, 0x64	//Fet configurable
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_DSGSC_CONFIG, MP279xConfig.DSGSC_LIM, MP279xConfig.DSGSC_DEG, USE_CRC, 1);		// 0x0A, 0x64	//Fet configurable
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_HYST, MP279xConfig.CELL_HYST, ((MP279xConfig.CELL_HYST&0xFF00)>>8), USE_CRC, 1); //Configure Cell Hysteresis
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACK_UV_OV, MP279xConfig.PACK_HYST, ((MP279xConfig.PACK_HYST&0xFF00)>>8), USE_CRC, 1); //Configure Pack Hysteresis
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_UV, MP279xConfig.CELL_UV, ((MP279xConfig.CELL_UV&0xFF00)>>8), USE_CRC, 1);	//, 0x9A, 0x01	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_OV, MP279xConfig.CELL_OV, ((MP279xConfig.CELL_OV&0xFF00)>>8), USE_CRC, 1);	//, 0xD7, 0x01	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACK_UV, MP279xConfig.PACK_UV&0x00FF, ((MP279xConfig.PACK_UV&0xFF00)>>8), USE_CRC, 1);	//, 0xC7, 0x19	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACK_OV, MP279xConfig.PACK_OV&0x00FF, ((MP279xConfig.PACK_OV&0xFF00)>>8), USE_CRC, 1);	//, 0x61, 0x1D	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_MSMT, MP279xConfig.CELL_MSMT&0x00FF, ((MP279xConfig.CELL_MSMT&0xFF00)>>8), USE_CRC, 1);
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELL_DEAD_THR, MP279xConfig.CELL_DEAD_THR, 0x00, USE_CRC, 1);	//, 0xFB, 0x00	//Fet configurable
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_OTHR_DSG, MP279xConfig.OTHR_DSG&0x00FF, ((MP279xConfig.OTHR_DSG&0xFF00)>>8), USE_CRC, 1);	//0x2E, 0x01	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_UTHR_DSG, MP279xConfig.UTHR_DSG&0x00FF, ((MP279xConfig.UTHR_DSG&0xFF00)>>8), USE_CRC, 1);	//0x94, 0x02	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_OTHR_CHG, MP279xConfig.OTHR_CHG&0x00FF, ((MP279xConfig.OTHR_CHG&0xFF00)>>8), USE_CRC, 1);	//0x2E, 0x01	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_UTHR_CHG, MP279xConfig.UTHR_CHG&0x00FF, ((MP279xConfig.UTHR_CHG&0xFF00)>>8), USE_CRC, 1);	//0x94, 0x8A	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_NTCM_OTHR, MP279xConfig.NTCM_OTHR&0x00FF, ((MP279xConfig.NTCM_OTHR&0xFF00)>>8), USE_CRC, 1);	//0xEB, 0x80	//Fet configurable!
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_REC, MP279xConfig.FT_REC&0x00FF, ((MP279xConfig.FT_REC&0xFF00)>>8), USE_CRC, 1);
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT0_CONFIG, MP279xConfig.FT0_CFG&0x00FF, ((MP279xConfig.FT0_CFG&0xFF00)>>8), USE_CRC, 1);	//R1: 0x63, 0x36	r0: 0x3C, 0x03		//0x7C, 0x37; 0x7F, 0x37,;0x7B, 0x37,
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_TRIMG_IPCB, MP279xConfig.TRIMG_IPCB&0x00FF, ((MP279xConfig.TRIMG_IPCB&0xFF00)>>8), USE_CRC, 1);	//0x00, 0x00
#ifdef MP279x_AutoBalance
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CONFIG, (Config.Features_Enable&0x80) ? 0xFE	: 0x00, 0x00, USE_CRC, 1);	////Auto-Balance with autobal_always (and bal_repetition set to 32 rep.)
#endif
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_THR, MP279xConfig.BAL_THR&0x00FF, ((MP279xConfig.BAL_THR&0xFF00)>>8), USE_CRC, 1);	//Configure Balancing threshold
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_OCFT_CTRL, MP279xConfig.OCFT_CTRL&0x00FF, ((MP279xConfig.OCFT_CTRL&0xFF00)>>8), USE_CRC, 1);		// To put MP279x in fault mode:  0xFF, 0x01
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SCFT_CTRL, MP279xConfig.SCFT_CTRL&0x00FF, ((MP279xConfig.SCFT_CTRL&0xFF00)>>8), USE_CRC, 1);		// To put MP279x in fault mode:  0x3F, 0x00
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, MP279xConfig.CELLFT_CTRL&0x00FF, ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);	//0xB6, 0x1A; 0xB6, 0x02,
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACKFT_CTRL, MP279xConfig.PACKFT_CTRL&0x00FF, ((MP279xConfig.PACKFT_CTRL&0xFF00)>>8), USE_CRC, 1);	//0x77, 0x03	//Think what to do about Cell MSMT
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_NTC_CONFIG, MP279xConfig.NTC_CONFIG&0x00FF, ((MP279xConfig.NTC_CONFIG&0xFF00)>>8), USE_CRC, 1);	//0xD5, 0xE0;, 0xD5, 0xC0
	I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SELF_CFG, MP279xConfig.SELF_CFG, 0xC3, USE_CRC, 1); //0x4F, 0xC0
}

//Calculate the configuration registers to be sent to the MP279x using the Datasheet LSB
void MP279xCConfigCalc(void)
{
	double R_NTC;

	MP279xConfig.SAFE_CONFIG = 0x09 | ((Config.Features_Enable&0x0100) >> 7);
#ifdef Direct_Mode
	MP279xConfig.FET_MODE = 0x17F9;
#else
	MP279xConfig.FET_MODE = 0x07F9;
#endif
	//CELL Thresholds Calculation
	MP279xConfig.CELL_UV = (uint16_t) (ceil(Config.Cell_UV/195.3125))  | (Config.CellUV_Dgl << 8);
	MP279xConfig.CELL_OV = (uint16_t) (Config.Cell_OV/195.3125)  | (Config.CellOV_Dgl << 8);
	MP279xConfig.CELL_HYST = (uint16_t) ((Config.CellUV_Hyst << 4) | (Config.CellOV_Hyst << 8));
	MP279xConfig.CELL_DEAD_THR = (uint16_t) (ceil(Config.Cell_Dead/195.3125))  | (Config.CellDead_Dgl << 7);
	MP279xConfig.CELL_MSMT = (uint16_t)  (Config.Cell_MSMT&0x1F)  | (Config.CellMSMT_Dgl << 5);
	MP279xConfig.CELLFT_CTRL = (uint16_t) 0x1A92 | ((Config.Faults_Enable&0x01)<<2) | ((Config.Faults_Enable&0x02)<<4);//R0:0x1A92 | ((Config.Faults_Enable&0x01)<<2) | ((Config.Faults_Enable&0x02)<<4);//0xB6, 0x1A
	//PACK Threshold Calculation
	MP279xConfig.PACK_UV = (((uint16_t)ceil(Config.Pack_UV/9.765625)) | (Config.PackUV_Dgl<<12));
	MP279xConfig.PACK_OV = (((uint16_t)(Config.Pack_OV/9.765625)) | (Config.PackOV_Dgl<<12));
	MP279xConfig.PACK_HYST = (uint16_t) ((Config.PackUV_Hyst << 2) | (Config.PackOV_Hyst << 10));	//Mind the shift error for vtop_uv_hyst; when solved use: "(Config.PackUV_Hyst << 2)"
	MP279xConfig.PACKFT_CTRL = 0x0D11 | ((Config.Faults_Enable&0x08)>>2) | ((Config.Faults_Enable&0x10)<<1) | ((Config.Faults_Enable&0x04)<<7) | ((Config.Faults_Enable&0x2000)>>1);	//0x0555 | ((Config.Faults_Enable&0x08)>>2) | ((Config.Faults_Enable&0x10)<<1) | ((Config.Faults_Enable&0x04)<<7);//0x77, 0x03;
	
	//Short Circtuit and Over Current Threshold Calculation
	MP279xConfig.DSGOC1_LIM = Calc_OC_SC_Lim(Config.Pack_DOC1, 80000, 2500/10, 7500/10);
	if (Config.PackDOC1_Dgl<320) MP279xConfig.DSGOC1_DEG = (uint8_t)(Config.PackDOC1_Dgl/5);
	else MP279xConfig.DSGOC1_DEG = ((uint8_t)(Config.PackDOC1_Dgl/40))|(1<<6);
	MP279xConfig.DSGOC2_LIM = Calc_OC_SC_Lim(Config.Pack_DOC2, 80000, 2500/10, 7500/10);
	if (Config.PackDOC2_Dgl<320) MP279xConfig.DSGOC2_DEG = (uint8_t)(Config.PackDOC2_Dgl/5);
	else MP279xConfig.DSGOC2_DEG = ((uint8_t)(Config.PackDOC2_Dgl/40))|(1<<6);
	
	MP279xConfig.DSGSC_LIM = Calc_OC_SC_Lim(Config.Pack_DSC, 176000, 5500/10, 16500/10);
	MP279xConfig.DSGSC_DEG = Config.PackDSC_Dgl;

	MP279xConfig.CHGOC_LIM = Calc_OC_SC_Lim(Config.Pack_COC, 51200, 1600/10, 4800/10);
	if (Config.PackCOC_Dgl<320) MP279xConfig.CHGOC_DEG = (uint8_t)(Config.PackCOC_Dgl/5);
	else MP279xConfig.CHGOC_DEG = ((uint8_t)(Config.PackCOC_Dgl/40))|(1<<6);

	MP279xConfig.CHGSC_LIM = Calc_OC_SC_Lim(Config.Pack_CSC, 80000, 2500/10, 7500/10);
	MP279xConfig.CHGSC_DEG = Config.PackCSC_Dgl;
	
	if (MP279xConfig.DSGOC1_LIM==0xFF) MP279xConfig.DSGOC1_LIM = 0;
	if (MP279xConfig.DSGOC2_LIM==0xFF) MP279xConfig.DSGOC2_LIM = 0;
	if (MP279xConfig.DSGSC_LIM==0xFF) MP279xConfig.DSGSC_LIM = 0;
	if (MP279xConfig.CHGOC_LIM==0xFF) MP279xConfig.CHGOC_LIM = 0;
	if (MP279xConfig.CHGSC_LIM==0xFF) MP279xConfig.CHGSC_LIM = 0;

	MP279xConfig.OCFT_CTRL = 0x0007 | ((Config.Faults_Enable&0xE0)<<1);
	MP279xConfig.SCFT_CTRL = 0x0003 | ((Config.Faults_Enable&0x0300)>>4);

	MP279xConfig.FT_REC=0x0008 | ((Config.Faults_AutoRecovery&0x0080)? 0x06 : 0x00) | ((Config.Faults_AutoRecovery&0x0200)>>5) | ((Config.Faults_AutoRecovery&0x007C)<<4);
	MP279xConfig.FT0_CFG = 0x3463 | ((Config.Faults_AutoRecovery&0x01)<<7) | ((Config.Faults_AutoRecovery&0x02)<<10); //0x3663
	
	//NTCs Threshold Calculation
	R_NTC=(R_TEMP_MEAS_PCB * exp(BETA_NTC_PCB*((1.0/((Config.PCB_OT/100.0)+273.15))-(1/298.15))));
	MP279xConfig.NTCM_OTHR = ((uint16_t)((1023.0*R_NTC)/(R_NTC + R_TEMP_MEAS_PCB)))|(Config.PCBOT_Hyst<<11);

	R_NTC=(R_TEMP_MEAS_BAT * exp(Config.BetaNTCBat*((1.0/((Config.Bat_DCH_OT/100.0)+273.15))-(1/298.15))));	//Config.Bat_DCH_OT	
	MP279xConfig.OTHR_DSG = (1023.0*R_NTC)/(R_NTC + R_TEMP_MEAS_BAT);

	R_NTC=(R_TEMP_MEAS_BAT * exp(Config.BetaNTCBat*((1.0/((Config.Bat_CHG_OT/100.0)+273.15))-(1/298.15))));	//Config.Bat_CHG_OT	
	MP279xConfig.OTHR_CHG = (1023.0*R_NTC)/(R_NTC + R_TEMP_MEAS_BAT);

	R_NTC=(R_TEMP_MEAS_BAT * exp(Config.BetaNTCBat*((1.0/((Config.Bat_CHG_UT/100.0)+273.15))-(1/298.15))));	//Config.Bat_CHG_UT	
	MP279xConfig.UTHR_CHG = ((uint16_t)((1023.0*R_NTC)/(R_NTC + R_TEMP_MEAS_BAT)))|((Config.Cells_NTC_Config&0x1F)<<11);

	R_NTC=(R_TEMP_MEAS_BAT * exp(Config.BetaNTCBat*((1.0/((Config.Bat_DCH_UT/100.0)+273.15))-(1/298.15))));	//Config.Bat_DCH_UT	
	MP279xConfig.UTHR_DSG = ((1023.0*R_NTC)/(R_NTC + R_TEMP_MEAS_BAT));
	
	uint16_t ntc_cnfg_base;
	if ((Config.Cells_NTC_Config&0x60)==0x20) ntc_cnfg_base=0x04C1;
	else if ((Config.Cells_NTC_Config&0x60)==0x40) ntc_cnfg_base=0x04C5;
	else ntc_cnfg_base=0x04D5;
	MP279xConfig.NTC_CONFIG = ntc_cnfg_base | ((Config.Faults_Enable&0x1C00)<<3);	//0x04D5 | ((Config.Faults_Enable&0x1C00)<<3);	// 0x20D5;
	
	MP279xConfig.SELF_CFG = 0x47 | ((Config.Features_Enable&0x40)>>3);
	MP279xConfig.PINS_CONFIG = (Config.Features_Enable&0x20) ? 0x60 : 0;		//0x60 added so MP279x will reset itself after a WDT event to avoid the issue of MCU unable to be programmed due to WDT pin (connected to MCU's nRST) toggling 

	switch (Config.NumofSeries)
	{
		case 7:
			MP279xConfig.CELLS_MASK=CELLS_MASK_7;
			break;
		case 8:
			MP279xConfig.CELLS_MASK=CELLS_MASK_8;
			break;
		case 9:
			MP279xConfig.CELLS_MASK=CELLS_MASK_9;
			break;
		case 10:
			MP279xConfig.CELLS_MASK=CELLS_MASK_10;
			break;
		case 11:
			MP279xConfig.CELLS_MASK=CELLS_MASK_11;
			break;
		case 12:
			MP279xConfig.CELLS_MASK=CELLS_MASK_12;
			break;
		case 13:
			MP279xConfig.CELLS_MASK=CELLS_MASK_13;
			break;
		case 14:
			MP279xConfig.CELLS_MASK=CELLS_MASK_14;
			break;
		#ifndef MP2791
		case 15:
			MP279xConfig.CELLS_MASK=CELLS_MASK_15;
			break;
		case 16:
			MP279xConfig.CELLS_MASK=CELLS_MASK_16;
			break;
		default:
			MP279xConfig.CELLS_MASK=CELLS_MASK_16;
			break;
		#else
		default:
			MP279xConfig.CELLS_MASK=CELLS_MASK_14;
			break;
		#endif
	}

	if (Config.I_Gain)
	{
		MP279xConfig.TRIMG_IPCB = ((Config.I_Gain*1024)/65534)-512;
		MP279xConfig.TRIMG_IPCB &= 0x03FF;
	}
	else
	{
		MP279xConfig.TRIMG_IPCB=511;
	}

	if ((Config.MinBalV>25000)&&(Config.MinBalV<50000)) MP279xConfig.BAL_THR=(uint16_t)((Config.MinBalV-25000)/390.625);
	else if (Config.MinBalV<=25000) MP279xConfig.BAL_THR=0x0000;
	else MP279xConfig.BAL_THR=0x003F;

	MP279xConfig.BAL_THR |= (Config.BalConfig<<6);
}

//Over Current and Short Circuit threshold calculation
uint8_t Calc_OC_SC_Lim (uint16_t conf_lim, uint32_t res_chng, uint16_t res_basic_red, uint16_t res_ext_red)
{
	uint8_t prov_val;
	uint16_t prov_val_10;
	if ((conf_lim*I_SENSE_R*2)<=res_chng)
	{
		prov_val_10 = (uint16_t)((conf_lim*I_SENSE_R*2)/res_basic_red);
		prov_val = prov_val_10/10;
		if ((prov_val_10<(prov_val*10+5))) prov_val-=1;
	}
	else 
	{
		prov_val_10 = (uint16_t)((conf_lim*I_SENSE_R*2)/res_ext_red);
		prov_val = prov_val_10/10;
		if ((prov_val_10<(prov_val*10+5))) prov_val-=1;
		prov_val |= (1<<5);
	}
	return prov_val;
}


//Perform the readings from the MP279x
void ReadADCResults(void)
{
	double R_NTC;
	uint16_t DataTempU;
	int16_t DataTempS;

	CommError=I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_ADC_CTRL, 0x00, 0x00, USE_CRC, 0);		//Clear ADC_CTRL register
	CommError|=I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_CLR, 0x00, 0x20, USE_CRC, 1);	//Clear 
	//Cell Voltage and Current Calculation:
	for (unsigned char i=0; i<16; i++)
	{
		if (i<Config.NumofSeries)
		{
			int16_t CellsI=0;
			//Read Cell Volatge measurement
			CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_VCELL1+2*i, (uint8_t *) (Measurements.Cells + i), ((uint8_t *) (Measurements.Cells + i)) + 1, USE_CRC, 1);
			Measurements.Cells[i]=((Measurements.Cells[i]*50000)/32767); //Classify cell voltage measurement wiht the correct LSB
			CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_ICELL1+2*i, (uint8_t *)&CellsI, (uint8_t *)&CellsI + 1, USE_CRC, 1);
			Measurements.Current[i]=((CellsI*100000.0/32768.0)/I_SENSE_R)+Config.I_Offset;
		}
		else Measurements.Cells[i]=Measurements.Current[i]=0;
		if (i)
		{
			if (i<Config.NumofSeries)
			{
				//Calculate the max and min cell voltage
				if (Measurements.Cells[i]>MaxVoltage)
				{
					MaxVoltage=Measurements.Cells[i];
					MaxVID=i;
				}
				else if (Measurements.Cells[i]<MinVoltage)
				{
					MinVoltage=Measurements.Cells[i];
					MinVID=i;
				}
			}
		}
		else
		{
			MaxVoltage=Measurements.Cells[0];
			MaxVID=0;
			MinVoltage=Measurements.Cells[0];
			MinVID=0;
		}
	}
	//Cell temperature Calculation
	for (unsigned char i=0; i<4; i++)
	{
		uint16_t VTemp=0;
		CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_VNTC1-i, (uint8_t *)&VTemp, (uint8_t *)&VTemp + 1, USE_CRC, 1); //Read Cell temperature
		if (i<3)
		{
			//Calculate the correct temperature value for the external NTCs
			if ((VTemp>0)&&(VTemp<32767))
			{
				R_NTC=(((float)(VTemp*R_TEMP_MEAS_BAT))/((float)(32767-VTemp)));
				float Temperature = (float)( ( 100* ((Config.BetaNTCBat) * 298.15) /
						( ( (Config.BetaNTCBat) + ( 298.15 * log( (double)((double)R_NTC/R_TEMP_MEAS_BAT) ) ) ) ) )
						- 27315	);
						
				if (Temperature>32767) Measurements.TempVect[i]=32767;
				else if (Temperature<(-27315)) Measurements.TempVect[i]=-27315;
				else Measurements.TempVect[i]=Temperature;
			}
			else if (VTemp>=32767) Measurements.TempVect[i]=-27315;
			else Measurements.TempVect[i]=32767;
		}
		else
		{
			//Calculate the correct tempreature value for the protections NTC
			if ((VTemp>0)&&(VTemp<32767))
			{
				R_NTC=(((float)(VTemp*R_TEMP_MEAS_PCB))/((float)(32767-VTemp)));
				float Temperature = (float)( ( 100* ((BETA_NTC_PCB) * 298.15) /
						( ( (BETA_NTC_PCB) + ( 298.15 * log( (double)((double)R_NTC/R_TEMP_MEAS_PCB) ) ) ) ) )
						- 27315	);
						
				if (Temperature>32767) BMSData.TempPCB=32767;
				else if (Temperature<(-27315)) BMSData.TempPCB=-27315;
				else BMSData.TempPCB=Temperature;
			}
			else if (VTemp==32767) BMSData.TempPCB=-27315;
			else BMSData.TempPCB=32767;
		}
	}
	//Read the MP279x AFE Status
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_PWR_STATUS, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.AFE_Status = (DataTempU&0x001F) | ((DataTempU&0x0380) >> 2);
#ifndef MP279x_AutoBalance
	//This is to handle MCU autobalancing without the MP279x autobalancing feature. See BMS_MP279x.h
	MP279xConfig.BAL_LIST = 0;
	if ((BMSData.BMSstatus != ALARM) && (BMSData.BMSstatus != AUTO_RECOVERY) && (Config.Features_Enable&0x80) && (((BMSData.AFE_Status&0x40)&&(Config.BalConfig&0x02))||((BMSData.AFE_Status&0x80)&&(Config.BalConfig&0x01))))
	{
		if (MaxVoltage>Config.MinBalV)
		{
			for (unsigned char i=0; i<Config.NumofSeries; i++)
			{
				//Check the cells are within the thresholds, otherwise they will be balanced 
				if ((Measurements.Cells[i] > Config.MinBalV) && (Measurements.Cells[i] > (MinVoltage + 195.3125 + ((Config.BalConfig&0x70)>>4)*97.65625)))	//+ 195.3125 + ((Config.BalConfig&0x70)>>4)*97.65625)))	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				{
					MP279xConfig.BAL_LIST |= (1<<i); //Add the cell i to the balance list
				}
			}
		}
	}
	CommError|=I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_LIST, (MP279xConfig.BAL_LIST & 0x00FF), ((MP279xConfig.BAL_LIST & 0xFF00)>>8), USE_CRC, 1); //Send the balance list
	CommError|=I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_BAL_CTRL, ((Config.Features_Enable & 0x80)>>7), 0x00, USE_CRC, 1); //Start a new balance seasson (if configured)
#endif

	//Read Real Time Under Voltage Fault
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_OV, (uint8_t *) &BMSData.CellsOVReg, ((uint8_t *) &BMSData.CellsOVReg) + 1, USE_CRC, 1);
	if (BMSData.CellsOVReg) CellMask_Fail_OV=Config.Faults_Enable & 0x02;
	else CellMask_Fail_OV=0;
	BMSData.CellsOVReg&=~(MP279xConfig.CELLS_MASK);
	if (BMSData.CellsOVReg) BMSData.RT_Alarms.OVC=1;
	else BMSData.RT_Alarms.OVC=0;
	//Read Real Time Under Voltage Fault
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_UV, (uint8_t *) &BMSData.CellsUVReg, ((uint8_t *) &BMSData.CellsUVReg) + 1, USE_CRC, 1);
	if (BMSData.CellsUVReg) CellMask_Fail_UV=Config.Faults_Enable & 0x01;
	else CellMask_Fail_UV=0;
	BMSData.CellsUVReg&=~(MP279xConfig.CELLS_MASK);
	if (BMSData.CellsUVReg) BMSData.RT_Alarms.UVC=1;
	else BMSData.RT_Alarms.UVC=0;
	//Read Real Time Cell Dead Fault
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_DEAD, (uint8_t *) &BMSData.CellsDeadReg, ((uint8_t *) &BMSData.CellsDeadReg) + 1, USE_CRC, 1);
	if (BMSData.CellsDeadReg) CellMask_Fail_DeadC=Config.Faults_Enable & 0x04;
	else {
		if (BMSData.BMSstatus==ALARM && BMSData.BMSAlarms.DeadC) CellMask_Fail_DeadC=0;
	}
	BMSData.CellsDeadReg&=~(MP279xConfig.CELLS_MASK);
	if (BMSData.CellsDeadReg) BMSData.RT_Alarms.DeadC=1;
	else BMSData.RT_Alarms.DeadC=0;
	//Read Real Time Cell Mismatch Fault
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_MSMT, (uint8_t *) &BMSData.CellsMSMTReg, ((uint8_t *) &BMSData.CellsMSMTReg) + 1, USE_CRC, 1);
	if (BMSData.CellsMSMTReg) CellMask_Fail_MSMT=Config.Faults_Enable & 0x2000;
	else CellMask_Fail_MSMT=0;
	BMSData.CellsMSMTReg&=~(MP279xConfig.CELLS_MASK);
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_CELL_MSMT_STS, (uint8_t *) &BMSData.CellsMSMT_Status, ((uint8_t *) &BMSData.CellsMSMT_Status) + 1, USE_CRC, 1);
	BMSData.RT_Alarms.MSMT=(BMSData.CellsMSMT_Status&0x4000)>>14;
	//Reset Cell Real Time Alarms
	if (((BMSData.RT_Alarms.UVC|BMSData.RT_Alarms.OVC<<1|BMSData.RT_Alarms.DeadC<<2|BMSData.RT_Alarms.MSMT<<13)&Config.Faults_Enable)==0 && (CellMask_Fail_OV || CellMask_Fail_UV || CellMask_Fail_DeadC || CellMask_Fail_MSMT)){
		if (BMSData.BMSstatus==ALARM && SafeFlag){
			ResetAlarms();
			CellMask_Fail_DeadC=0;
		}
		else if (BMSData.BMSstatus==SAFE){
			ResetAlarms();
			CellMask_Fail_DeadC=0;
		}
	}
	//Read Real Time Pack Over/Under Voltage
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_PACK_UV_OV, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.RT_Alarms.OVP=(DataTempU&0x0100) >> 8;
	BMSData.RT_Alarms.UVP=DataTempU&0x0001;
	//Read Real Time Over Current Faul
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_OC_STATUS, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.RT_Alarms.COC=((DataTempU&0x0400) >> 10) | COC_AutoRec;
	if (DataTempU&0x0300)	BMSData.RT_Alarms.DOC=1;
	else BMSData.RT_Alarms.DOC=0;
	BMSData.RT_Alarms.DOC |= DOC_AutoRec;
	//Read Real Time Under Current Fault
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_SC_STATUS, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.RT_Alarms.CSC=((DataTempU&0x0200) >> 9) | CSC_AutoRec;
	BMSData.RT_Alarms.DSC=((DataTempU&0x0100) >> 8) | DSC_AutoRec;
	//Read Real Time NTC Status
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_NTC_DIE, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.NTC_Status = ((DataTempU&0x0800) >> 5) | (((DataTempU&0x2000) | ((DataTempU&0x4000)>>1)|((DataTempU&0x8000) >> 2)) >> 6);

	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_VANTC3, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.NTC_Status |= ((DataTempU&0x7000) >> 12);
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_VANTC4, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.NTC_Status |= ((DataTempU&0x7000) >> 9);
	//Classify Real Time NTC Faults
	if (BMSData.NTC_Status&0x07)	BMSData.RT_Alarms.BatOT=1;
	else	BMSData.RT_Alarms.BatOT=0;
	if (BMSData.NTC_Status&0x38)	BMSData.RT_Alarms.BatUT=1;
	else	BMSData.RT_Alarms.BatUT=0;
	BMSData.RT_Alarms.TPCB=(BMSData.NTC_Status&0x40) >> 6;
	BMSData.RT_Alarms.Tchip=(BMSData.NTC_Status&0x80) >> 7;
	
	if ((Config.Features_Enable&0x01) && ((BMSData.BMSstatus==ALARM) || (BMSData.BMSstatus==SAFE))) BMSData.OpenWireDet[2]|=0x80;
	else BMSData.OpenWireDet[2]&=(~0x80);

	//Check the load or charger status
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_LOAD_CHGR_STATUS, (uint8_t *) &BMSData.LC_Status, ((uint8_t *) &BMSData.LC_Status)+1, USE_CRC, 1);
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_STB_STATUS, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU)+1, USE_CRC, 1);
	BMSData.LC_Status&=~(0x0700);
	BMSData.LC_Status|=DataTempU&0x0700;
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU)+1, USE_CRC, 1);
	BMSData.LC_Status|=(((~DataTempU)&0x0008)<<4);
	
	//Check the FET status
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_FET_STATUS, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU)+1, USE_CRC, 1);
	BMSData.FETs_Status = (DataTempU&0x000F) | ((DataTempU&0x0F00)>>4);
	
	//Check the ADC voltages
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_RD_V3P3, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	V3P3=((DataTempU*66000)/32767);
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_RD_V1P8, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	V1P8=((DataTempU*33000)/32767);
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_RD_V5, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	V5=((DataTempU*66000)/32767);
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_RD_VTOP, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.VTOP=((DataTempU*40000)/32767);
	//Pack Current Reading
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_RD_ITOP, (uint8_t *) &DataTempS, ((uint8_t *) &DataTempS) + 1, USE_CRC, 1);
	ITOP_Temp=(-(DataTempS*100000.0/32768.0)/I_SENSE_R);
	Measurements.PackCurrent=BMSData.ITOP=-ITOP_Temp+Config.I_Offset;	(Config.I_Gain/32767.0)+20000-Config.I_Offset;
	//Pack Voltage Reading 
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_RD_VPACKP, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	Measurements.VPack=BMSData.PACKV=((DataTempU*40000)/32767);
	//MP279x Chip Internal Temperature Reading
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_RD_TDIE, (uint8_t *) &DataTempU, ((uint8_t *) &DataTempU) + 1, USE_CRC, 1);
	BMSData.TempIC=((((DataTempU*33000)/32767)-18308)/0.68);        //((((DataTempU*33000)/32767)-20000)/0.6)+2500;
	//Check the Balance List and the Balancing status
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_BAL_STS, (uint8_t *) &BMSData.Bal_Status, ((uint8_t *) &BMSData.Bal_Status) + 1, USE_CRC, 1);
	#ifdef MP279x_AutoBalance
	if (!CommError)
	{
		if ((Config.Features_Enable & 0x80) && (!(BMSData.Bal_Status&0x0780)) && (!(BMSData.AFE_Status&0x20)) && (!((!(MP279xConfig.BAL_THR&0x40))&&(BMSData.AFE_Status&0x80))) && (!((!(MP279xConfig.BAL_THR&0x80))&&(BMSData.AFE_Status&0x40))))
		{
			CommError=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_BAL_LIST, (uint8_t *) &Measurements.CellsBalancing, ((uint8_t *) &Measurements.CellsBalancing) + 1, USE_CRC, 1);
		}
		else Measurements.CellsBalancing=0;
	}
	#else
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x, REG_BAL_LIST, (uint8_t *) &Measurements.CellsBalancing, ((uint8_t *) &Measurements.CellsBalancing) + 1, USE_CRC, 1);
	#endif
	//Check the MP279x self test result
	CommError|=I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_SELF_STS, (uint8_t *) (&BMSData.SelfStatus), (uint8_t *) &DataTempU, USE_CRC, 1);
	if (BMSData.SelfStatus&0x03) BMSData.RT_Alarms.ChipSenseError=1;
	else BMSData.RT_Alarms.ChipSenseError=0;

	//Check if any communication error has happened
	if (CommError)
	{
		ErrorCount++;
		SET_LD4;
		BMSAlarmHandler(CHIP_ERROR);
		BMSData.RT_Alarms.ChipSystemError=1;
	}
	else
	{
		if (BMSData.SelfStatus&0x48) BMSData.RT_Alarms.ChipSystemError=1;
		else	BMSData.RT_Alarms.ChipSystemError=0;
		
		//ADC Watchdog:
		ADC_Req=0;
	}
	if ((!FirstSample)&&(!CommError)) FirstSample=1;
	New_Meas = 1;
}

//Initialize the configuration registers
void InitConfiguration(void)
{
	//Charge the Default configuration when asked or when the Flash is empty:
	if ((*Cell_UV_FLASH==0xFFFF)||DefaultConfig)
	{
		Config.NumofSeries=CellsSeries;
		Config.Cell_UV = CellUV_Thres;
		Config.CellUV_Hyst = 1;
		Config.CellUV_Dgl = 1;
		Config.Cell_OV = CellOV_Thres;
		Config.CellOV_Hyst = 1;
		Config.CellOV_Dgl = 1;
		Config.Cell_Dead = CellDead_Thres;
		Config.CellDead_Dgl = 1;
		Config.Cell_MSMT = CellMSMT_Thres;
		Config.CellMSMT_Dgl = 1;
		Config.Pack_UV = PackUV_Thres/2;
		Config.PackUV_Hyst = 3;
		Config.PackUV_Dgl = 1;
		Config.Pack_OV = PackOV_Thres/2;
		Config.PackOV_Hyst = 3;
		Config.PackOV_Dgl = 1;
		Config.Pack_DOC1 = OC_THRES_1/2;
		Config.PackDOC1_Dgl = OC_THRES_1_Dgl;
		Config.Pack_DOC2 = OC_THRES_2/2;
		Config.PackDOC2_Dgl = OC_THRES_2_Dgl;
		Config.Pack_COC = COC_THRES/2;
		Config.PackCOC_Dgl = COC_THRES_Dgl;
		Config.Pack_DSC = DSC_THRES/2;;
		Config.PackDSC_Dgl = DSC_THRES_Dgl;
		Config.Pack_CSC = CSC_THRES/2;
		Config.PackCSC_Dgl = CSC_THRES_Dgl;
		Config.Bat_DCH_OT = BAT_DOT_THRES;
		Config.Bat_DCH_UT = BAT_DUT_THRES;
		Config.Bat_CHG_OT = BAT_COT_THRES;
		Config.Bat_CHG_UT = BAT_CUT_THRES;
		Config.Cells_NTC_Config = 0x62;
		Config.PCB_OT = PCB_OT_THRES;
		Config.PCBOT_Hyst = 0x02;
		Config.I_Gain=Current_Gain;
		if ((*((uint16_t *)CurrOffset))==0xFFFF) Config.I_Offset=Current_Offset;
		Config.BetaNTCBat=BETA_NTC_BAT;
		Config.Sby_Time=SBY_TIME;
		Config.Sby_I_Thr=SBY_I_THR;
		Config.MinBalV=Min_Bal_V;
		Config.BalConfig=Bal_Config;
		Config.Faults_Enable=FAULTS_EN;//0x1FFF;
		Config.Faults_AutoRecovery=FAULTS_AUTORECOVERY;//0x0283;
		Config.Features_Enable=FEATURES_EN;//0x0175;
		IsenseR=I_SENSE_R_DEF*20;
		SaveConfigtoFLASH();
		DefaultConfig=0;
	}
	//Charge the configuration saved in the Flash:
	Config.Cell_UV=*Cell_UV_FLASH;
	Config.CellUV_Hyst = *Cell_UV_Hyst_FLASH;
	Config.CellUV_Dgl = *Cell_UV_Dgl_FLASH;
	Config.Cell_OV=*Cell_OV_FLASH;
	Config.CellOV_Hyst = *Cell_OV_Hyst_FLASH;
	Config.CellOV_Dgl = *Cell_OV_Dgl_FLASH;
	Config.Cell_Dead = *Cell_Dead_FLASH;
	Config.CellDead_Dgl = *Cell_Dead_Dgl_FLASH;
	Config.Cell_MSMT = *Cell_MSMT_FLASH;
	Config.CellMSMT_Dgl = *Cell_MSMT_Dgl_FLASH;
	Config.Pack_UV=*Pack_UV_FLASH;
	Config.PackUV_Hyst = *Pack_UV_Hyst_FLASH;
	Config.PackUV_Dgl = *Pack_UV_Dgl_FLASH;
	Config.Pack_OV=*Pack_OV_FLASH;
	Config.PackOV_Hyst = *Pack_OV_Hyst_FLASH;
	Config.PackOV_Dgl = *Pack_OV_Dgl_FLASH;
	Config.Pack_DOC1=*Pack_DOC1_FLASH;
	Config.PackDOC1_Dgl = *Pack_DOC1_Dgl_FLASH;
	Config.Pack_DOC2=*Pack_DOC2_FLASH;
	Config.PackDOC2_Dgl = *Pack_DOC2_Dgl_FLASH;
	Config.Pack_DSC=*Pack_DSC_FLASH;
	Config.PackDSC_Dgl = *Pack_DSC_Dgl_FLASH;
	Config.Pack_COC=*Pack_COC_FLASH;
	Config.PackCOC_Dgl = *Pack_COC_Dgl_FLASH;
	Config.Pack_CSC=*Pack_CSC_FLASH;
	Config.PackCSC_Dgl = *Pack_CSC_Dgl_FLASH;
	Config.Bat_DCH_OT=*Bat_DCH_OT_FLASH;
	Config.Bat_DCH_UT=*Bat_DCH_UT_FLASH;
	Config.Bat_CHG_OT=*Bat_CHG_OT_FLASH;
	Config.Bat_CHG_UT=*Bat_CHG_UT_FLASH;
	Config.Cells_NTC_Config=*Cells_NTC_Config_FLASH;
	Config.PCB_OT=*PCB_OT_FLASH;
	Config.PCBOT_Hyst=*PCB_OT_Hyst_FLASH;
	Config.I_Gain=*I_Gain_FLASH;
	Config.I_Offset=*CurrOffset;
	Config.BetaNTCBat=*BetaNTCBatFLASH;
	Config.NumofSeries=*NumofSeriesFLASH;
	Config.Sby_Time=*Sby_Time_FLASH;
	Config.Sby_I_Thr=*Sby_I_Thr_FLASH;
	Config.MinBalV=*MinBalV_FLASH;
	Config.BalConfig=*BalConfig_FLASH;
	Config.Faults_Enable=*Faults_Enable_FLASH;
	Config.Faults_AutoRecovery=*Faults_AutoRecovery_FLASH;
	Config.Features_Enable=*Features_Enable_FLASH;
	IsenseR=*I_Sense_R_FLASH;
	MP279xCConfigCalc();
}

//Save the Config registers in Flash
void SaveConfigtoFLASH(void)
{
	if (!((FLASH->SR)&FLASH_SR_BSY))
	{
		FLASH->KEYR=0x45670123;
		FLASH->KEYR=0xCDEF89AB;
		FLASH->CR|=FLASH_CR_PER;
		FLASH->AR=(uint32_t)FL_Config_ADDRESS;
		FLASH->CR|=FLASH_CR_STRT;
		while((FLASH->SR)&FLASH_SR_BSY){Nop();}
		FLASH->CR&=!FLASH_CR_PER;
		FLASH->CR|=FLASH_CR_PG;
		*Cell_UV_FLASH=Config.Cell_UV;
		*Cell_UV_Hyst_FLASH=Config.CellUV_Hyst;
		*Cell_UV_Dgl_FLASH=Config.CellUV_Dgl;
		*Cell_OV_FLASH=Config.Cell_OV;
		*Cell_OV_Hyst_FLASH=Config.CellOV_Hyst;
		*Cell_OV_Dgl_FLASH=Config.CellOV_Dgl;
		*Cell_Dead_FLASH=Config.Cell_Dead;
		*Cell_Dead_Dgl_FLASH=Config.CellDead_Dgl;
		*Cell_MSMT_FLASH=Config.Cell_MSMT;
		*Cell_MSMT_Dgl_FLASH=Config.CellMSMT_Dgl;
		*Pack_UV_FLASH=Config.Pack_UV;
		*Pack_UV_Hyst_FLASH=Config.PackUV_Hyst;
		*Pack_UV_Dgl_FLASH=Config.PackUV_Dgl;
		*Pack_OV_FLASH=Config.Pack_OV;
		*Pack_OV_Hyst_FLASH=Config.PackOV_Hyst;
		*Pack_OV_Dgl_FLASH=Config.PackOV_Dgl;
		*Pack_DOC1_FLASH=Config.Pack_DOC1;
		*Pack_DOC1_Dgl_FLASH = Config.PackDOC1_Dgl;
		*Pack_DOC2_FLASH=Config.Pack_DOC2;
		*Pack_DOC2_Dgl_FLASH = Config.PackDOC2_Dgl;
		*Pack_DSC_FLASH=Config.Pack_DSC;
		*Pack_DSC_Dgl_FLASH = Config.PackDSC_Dgl;
		*Pack_COC_FLASH=Config.Pack_COC;
		*Pack_COC_Dgl_FLASH = Config.PackCOC_Dgl;
		*Pack_CSC_FLASH=Config.Pack_CSC;
		*Pack_CSC_Dgl_FLASH = Config.PackCSC_Dgl;
		*Bat_DCH_OT_FLASH=Config.Bat_DCH_OT;
		*Bat_DCH_UT_FLASH=Config.Bat_DCH_UT;
		*Bat_CHG_OT_FLASH=Config.Bat_CHG_OT;
		*Bat_CHG_UT_FLASH=Config.Bat_CHG_UT;
		*Cells_NTC_Config_FLASH=Config.Cells_NTC_Config;
		*PCB_OT_FLASH=Config.PCB_OT;
		*PCB_OT_Hyst_FLASH=Config.PCBOT_Hyst;
		*I_Gain_FLASH=Config.I_Gain;
		*CurrOffset= Config.I_Offset;
		*NumofSeriesFLASH=Config.NumofSeries;
		*BetaNTCBatFLASH=Config.BetaNTCBat;
		*Sby_Time_FLASH=Config.Sby_Time;
		*Sby_I_Thr_FLASH=Config.Sby_I_Thr;
		*MinBalV_FLASH=Config.MinBalV;
		*BalConfig_FLASH=Config.BalConfig;
		*Faults_Enable_FLASH=Config.Faults_Enable;
		*Faults_AutoRecovery_FLASH=Config.Faults_AutoRecovery;
		*Features_Enable_FLASH=Config.Features_Enable;
		*I_Sense_R_FLASH=IsenseR;
		while((FLASH->SR)&FLASH_SR_BSY){Nop();}
		FLASH->CR&=!FLASH_CR_PG;
		FLASH->CR|=FLASH_CR_LOCK;
	}
}

//Reads SoC results from the MPF4279x FG
void ReadFGResultsMPF4279x(void)
{
	uint8_t FG_flags;
	if (!MPF4279x_to_Active_Mode(I2C1, MPF4279x_Address, 0, 0, ADD_STOP))		//The MPF4279x I2C communication has to be activated
	{
		SysTick_Delay_us(30);
		I2Cerror=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0F00, (uint8_t *) &FG_flags, 1, 0, MPF4279x_CRC_EN);	//Check the MPF4279x Flags
		if ((!I2Cerror)&&(!(FG_flags&0x05)))	//Make sure the MPF4279x is not running a Iteration and is not in Configuration mode
		{
			SysTick_Delay_us(50);
			//Read relevant FG Input data from FG:
			#ifdef FG_NewGen
			I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x006A, (uint8_t *) &Measurements.Tamb, 2, 0, MPF4279x_CRC_EN);
			SysTick_Delay_us(50);
			I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0074, (uint8_t *) &Measurements.TChip, 2, 0, MPF4279x_CRC_EN);
			SysTick_Delay_us(50);
			I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0078, (uint8_t *) &Measurements.rst, 4, 0, MPF4279x_CRC_EN);
			#else
			SysTick_Delay_us(50);
			I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0072, (uint8_t *) &Measurements.TChip, 2, 0, MPF4279x_CRC_EN);
			SysTick_Delay_us(50);
			I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0076, (uint8_t *) &Measurements.rst, 3, 0, MPF4279x_CRC_EN);
			#endif
			//Read FG Output registers:
			uint16_t rem_length = fg_out_size;
			uint16_t Reg_Addr=0x0400;
			while (rem_length>0)
			{
				SysTick_Delay_us(50);
				uint8_t read_length = rem_length >= 82 ? 82 : rem_length;
				I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, Reg_Addr, ((uint8_t *) &ResultsFG) + (Reg_Addr-0x400), read_length, 0, MPF4279x_CRC_EN);
				Reg_Addr+=read_length;
				rem_length-=read_length;
			}

			SysTick_Delay_us(50);
			//Check FG's configured Period of Execution
			I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x1000, (uint8_t *) &FG_Ts, 1, 0, MPF4279x_CRC_EN);
			
			//Ensure iteration is completed:
			if ((!I2Cerror)&&(ResultsFG.iteration!=Last_FG_IT))
			{
				FG_Cntr = FGReady = FirstSoC = 0;
				Last_FG_IT=ResultsFG.iteration;
			}
		}
	}
	else I2Cerror=1;
}

//Writes the FG relevant measurements to the MPF4279x FG
void SendFGDataMPF4279x(void)
{
	//Update the CC measurement
	#ifdef ENABLE_CC
	if(CCCount && (!FG_In_Send_Pend))
	{
		Measurements.I_CC=((2000.0)/(5.0*480.0))*CC_Accum*FG_Ts/CCCount;
		CC_Accum=CCCount=0;
		FG_CC_Cntr++;
	}
	#endif
	for (char errCnt=0;errCnt<3;errCnt++) //It tries to send the measurements 3 times if I2Cerror is triggered
	{
		I2Cerror=0;
		if (MPF4279x_to_Active_Mode(I2C1, MPF4279x_Address, 0, 0, ADD_STOP))	//The MPF4279x I2C line has to be activated
		{
			if (errCnt<2) SysTick_Delay_us(50);
			I2Cerror=1;
			continue;
		}
		if (I2C_MPF4279x_Write(I2C1, MPF4279x_Address, 0x0000, (uint8_t *)Measurements.Cells, 34, 0, MPF4279x_CRC_EN))		//Write voltage measurements
		{
			I2Cerror=1;
			continue;
		}
		#ifdef ENABLE_CC
		if (I2C_MPF4279x_Write(I2C1, MPF4279x_Address, 0x0022, (uint8_t *)Measurements.Current, 72, 0, MPF4279x_CRC_EN))		//Write current measurements
		{
			I2Cerror=1;
			continue;
		}
		#else
		if (I2C_MPF4279x_Write(I2C1, MPF4279x_Address, 0x0022, (uint8_t *)Measurements.Current, 68, 0, MPF4279x_CRC_EN))		//Write current measurements
		{
			I2Cerror=1;
			continue;
		}
		#endif
		
		#ifdef FG_NewGen
		if (I2C_MPF4279x_Write(I2C1, MPF4279x_Address, 0x006C, (uint8_t *)Measurements.TempVect, 8, 0, MPF4279x_CRC_EN))		//Write Temperature
		{
			I2Cerror=1;
			continue;
		}
		if (I2C_MPF4279x_Write(I2C1, MPF4279x_Address, 0x0076, (uint8_t *)&Measurements.CellsBalancing, 2, 0, MPF4279x_CRC_EN))		//Write Balancing
		{
			I2Cerror=1;
			continue;
		}
		#else
		if (I2C_MPF4279x_Write(I2C1, MPF4279x_Address, 0x006A, (uint8_t *)Measurements.TempVect, 8, 0, MPF4279x_CRC_EN))		//Write Temperature
		{
			I2Cerror=1;
			continue;
		}
		if (I2C_MPF4279x_Write(I2C1, MPF4279x_Address, 0x0074, (uint8_t *)&Measurements.CellsBalancing, 2, 0, MPF4279x_CRC_EN))		//Write Balancing
		{
			I2Cerror=1;
			continue;
		}
		#endif
		if(!(I2C_MPF4279x_Write(I2C1, MPF4279x_Address, FG_Trigger, (uint8_t *)&CmdMsg , 1, 0, MPF4279x_CRC_EN)))					//Send the Trigger message to run a FG Iteration
		{
			FirstSoC = 1;
			FG_In_Send_Pend = 0;
			I2Cerror=0;
			break;
		}
		I2Cerror=1;
	}
}

void SendMeasVFG(void)
{
	uint16_t rem_length = Measurements_size;
	uint16_t Reg_Addr=0x0000;
	uint8_t Check_Wrt[82];
	
	for (uint8_t ErrorCnt=0; ErrorCnt<3; ErrorCnt++)
	{
		if (MPF4279x_to_Active_Mode(I2C1, MPF4279x_Address, 0, 0, ADD_STOP))
		{
			continue;
		}
		else
		{
			Reg_Addr=0x0000;
			rem_length = Measurements_size;
			I2Cerror=0;
			uint8_t FG_flags;
			if (!I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0F00, (uint8_t *) &FG_flags, 1, 0, MPF4279x_CRC_EN))
			{
				if (!(FG_flags&0x01))
				{
					if (VirtualFG.VFG_First)
					{
						I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0400 + fg_out_size - 4, (uint8_t *) &VirtualFG.VFG_Last_IT, 4, 0, MPF4279x_CRC_EN);
						VirtualFG.VFG_First=0;
					}
					uint8_t verify_ok=0;
					while (!verify_ok)
					{
						I2Cerror=0;
						Reg_Addr=0x0000;
						rem_length = Measurements_size;			
						while (rem_length>0)
						{
							SysTick_Delay_us(50);
							uint8_t write_length = rem_length >= 82 ? 82 : rem_length;
							I2Cerror|=I2C_MPF4279x_Write(I2C1, MPF4279x_Address, Reg_Addr, ((uint8_t *) &MeasurementsVFG) + Reg_Addr, write_length, 1, MPF4279x_CRC_EN);
							if (!I2Cerror)
							{
								Reg_Addr+=write_length;
								rem_length-=write_length;
							}
							else break;
						}
						if (!I2Cerror)
						{
							Reg_Addr=0x0000;
							rem_length = Measurements_size;						
							while (rem_length>0)
							{
								SysTick_Delay_us(50);
								uint8_t write_length = rem_length >= 82 ? 82 : rem_length;
								I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, Reg_Addr, Check_Wrt, write_length, 1, MPF4279x_CRC_EN);
								uint8_t check_fail=0;
								if (!I2Cerror)
								{
									for (uint8_t i=0; i < write_length; i++)
									{
										if ((*(((uint8_t *) &MeasurementsVFG) + Reg_Addr + i)) != Check_Wrt[i])
										{
											check_fail=1;
											break;
										}
									}
									if (!check_fail)
									{
										Reg_Addr+=write_length;
										rem_length-=write_length;
									}
									else
									{
										continue;
									}
								}
								else
								{
									break;
								}
							}
							if (!I2Cerror) verify_ok=1;
						}
						else break;
						if(I2Cerror) break;
					}
					if (!I2Cerror) break;
				}
				else
				{
					return;
				}
			}
			else
			{
				continue;
			}
		}
	}
	if (!I2Cerror)
	{
		I2Cerror=I2C_MPF4279x_Write(I2C1, MPF4279x_Address, FG_Trigger, (uint8_t *)&CmdMsg , 1, 1, MPF4279x_CRC_EN);
	}
}

void ReadRsltsVFG(void)
{
	VirtualFG.VFG_Reading=1;
	if (VirtualFG.VFG_First)
	{
		SendMeasVFG();
	}
	else
	{
		uint8_t FG_flags;
		I2Cerror=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0F00, (uint8_t *) &FG_flags, 1, 1, MPF4279x_CRC_EN);
		if (!I2Cerror)
		{
			if (!(FG_flags&0x01))
			{
				uint32_t FG_IT;
				I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, 0x0400 + fg_out_size - 4, (uint8_t *) &FG_IT, 4, 0, MPF4279x_CRC_EN);
				if (!I2Cerror)
				{
					if (FG_IT!=VirtualFG.VFG_Last_IT)
					{
						uint16_t rem_length = Measurements_size - VFG_Meas_NoRead_Size;
						uint16_t Reg_Addr=0x0000;
						
						while (rem_length>0)
						{
							SysTick_Delay_us(50);
							uint8_t read_length = rem_length >= 82 ? 82 : rem_length;
							I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, Reg_Addr, ((uint8_t *) &MeasurementsVFG) + Reg_Addr, read_length, 0, MPF4279x_CRC_EN);
							Reg_Addr+=read_length;
							rem_length-=read_length;
						}
						
						rem_length = fg_out_size;
						Reg_Addr=0x0400;
						while (rem_length>0)
						{
							SysTick_Delay_us(50);
							uint8_t read_length = rem_length >= 82 ? 82 : rem_length;
							I2Cerror|=I2C_MPF4279x_Read(I2C1, MPF4279x_Address, Reg_Addr, ((uint8_t *) &ResultsFG) + (Reg_Addr-0x400), read_length, 0, MPF4279x_CRC_EN);
							Reg_Addr+=read_length;
							rem_length-=read_length;
						}
						if (!I2Cerror)
						{
							VirtualFG.VFG_Last_IT=ResultsFG.iteration;
							VirtualFG.VFG_IT_Done=1;
							VirtualFG.VFG_IT_Running=0;
						}
					}
					else
					{
						SendMeasVFG();
					}
				}
			}
		}
	}
	VirtualFG.VFG_Reading=0;
}

//ALARM Handler Routine
void BMSAlarmHandler(alarms alarm)
 {
	//Check the kind of fault triggered
	if (((alarm==COC) && (!BMSData.BMSAlarms.COC)) || ((alarm==CSC) && (!BMSData.BMSAlarms.CSC)) || ((alarm==PACK_OV) && (!BMSData.BMSAlarms.OVP)) || ((alarm==CELL_OV) && (!BMSData.BMSAlarms.OVC)))
	{
#ifdef Direct_Mode
		uint8_t FET_State=0xFF, Driver_State=0xFF;
		DISABLE_CHG;
		SysTick_Delay_us(1000);
		while ((FET_State&0x01)||(Driver_State&0x01))
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_FET_STATUS, (uint8_t *) &FET_State, (uint8_t *) &Driver_State, USE_CRC, 1);
		}
		DISABLE_DCH; 						//Needed for the charger to work properly
#else
		DISABLE_FETs;						//Open the protections
#endif
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x00, 0x00, USE_CRC, 1);		//Re-configure the FETs
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x08, 0x00, USE_CRC, 1);		//Re-configure the GPIOs
		if (alarm==CELL_OV)
		{
			BMSData.BMSAlarms.OVC=1;		//Update the BMSAlarm structure
		}
		else if (alarm==PACK_OV)
		{
			BMSData.BMSAlarms.OVP=1;		//Update the BMSAlarm structure
		}
		else if (alarm==COC) BMSData.BMSAlarms.COC=1;		//Update the BMSAlarm structure
		else BMSData.BMSAlarms.CSC=1;		//Update the BMSAlarm structure
		BMSData.BMSstatus=ALARM;		//Change the IC Status
	}
	if (((alarm==DOC) && (!BMSData.BMSAlarms.DOC)) || ((alarm==DSC) && (!BMSData.BMSAlarms.DSC)) || ((alarm==PACK_UV) && (!BMSData.BMSAlarms.UVP)) || ((alarm==CELL_UV) && (!BMSData.BMSAlarms.UVC)) || ((alarm==CELL_DEAD) && (!BMSData.BMSAlarms.DeadC)))
	{
#ifdef Direct_Mode
		uint8_t FET_State=0xFF, Driver_State=0xFF;
		DISABLE_DCH; 						//Needed for the charger to work properly
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x00, 0x00, USE_CRC, 1);
		SysTick_Delay_us(1000);
		while ((FET_State&0x02)||(Driver_State&0x02))
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_FET_STATUS, (uint8_t *) &FET_State, (uint8_t *) &Driver_State, USE_CRC, 1);
		}
		DISABLE_CHG;
#else
		DISABLE_FETs;			//Open the protections
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x00, 0x00, USE_CRC, 1);		//Re-configure the FETs
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x08, 0x00, USE_CRC, 1);   //Re-configure the GPIOs
#endif
		if (alarm==CELL_UV)
		{
			BMSData.BMSAlarms.UVC=1;		//Update the BMSAlarm structure
		}
		else if (alarm==PACK_UV)
		{
			BMSData.BMSAlarms.UVP=1;		//Update the BMSAlarm structure
		}
		else if (alarm==CELL_DEAD) BMSData.BMSAlarms.DeadC=1;		//Update the BMSAlarm structure
		else if (alarm==DOC) BMSData.BMSAlarms.DOC=1;		//Update the BMSAlarm structure
		else BMSData.BMSAlarms.DSC=1;		//Update the BMSAlarm structure
		BMSData.BMSstatus=ALARM;		//Change the IC Status
	}
	if ((alarm==CHIP_TEMP && !BMSData.BMSAlarms.Tchip) | (alarm==BAT_OT && !BMSData.BMSAlarms.BatOT) | (alarm==BAT_UT && !BMSData.BMSAlarms.BatUT) | (alarm==PCB_TEMP && !BMSData.BMSAlarms.TPCB) | (alarm==OPEN_W && !BMSData.BMSAlarms.OW) | (alarm==CELL_MSMT && !BMSData.BMSAlarms.MSMT) | (alarm==FET_DRIVER && !BMSData.BMSAlarms.FET_Drv) | (alarm==CHIP_ERROR && !BMSData.BMSAlarms.ChipSystemError) | (alarm==CHIP_SENSE_ERROR && !BMSData.BMSAlarms.ChipSenseError))
	{
#ifdef Direct_Mode
		DISABLE_CHG;
		DISABLE_DCH;
#else
		DISABLE_FETs;		//Open the protections
#endif
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FET_CTRL, 0x00, 0x00, USE_CRC, 1);		//Re-configure the FETs
		I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_GPIO_OUT, 0x08, 0x00, USE_CRC, 1);   //Re-configure the GPIOs
		
		if (alarm==CHIP_TEMP) BMSData.BMSAlarms.Tchip=1;											//Update the BMSAlarm structure
		if (alarm==BAT_OT) BMSData.BMSAlarms.BatOT=1;													//Update the BMSAlarm structure
		if (alarm==BAT_UT) BMSData.BMSAlarms.BatUT=1;													//Update the BMSAlarm structure
		if (alarm==PCB_TEMP) BMSData.BMSAlarms.TPCB=1;												//Update the BMSAlarm structure
		if (alarm==OPEN_W) BMSData.BMSAlarms.OW=1;														//Update the BMSAlarm structure
		if (alarm==CELL_MSMT) BMSData.BMSAlarms.MSMT=1;												//Update the BMSAlarm structure
		if (alarm==FET_DRIVER) BMSData.BMSAlarms.FET_Drv=1;										//Update the BMSAlarm structure
		if (alarm==CHIP_ERROR) BMSData.BMSAlarms.ChipSystemError=1;						//Update the BMSAlarm structure
		if (alarm==CHIP_SENSE_ERROR) BMSData.BMSAlarms.ChipSenseError=1;			//Update the BMSAlarm structure
		BMSData.BMSstatus=ALARM;		//Change the IC Status
	}
	//Update StandByFlag and SafeFlag
	if (StandByFlag) StandByFlag=0;
	if (SafeFlag) SafeFlag=0;
	Reset_Alarms_done = 0;
	
	SET_LD4;
	RSET_LD3;
}

//Reset Alarms Handler
void ResetAlarms(void)
{
	//Reset ALARMS if everything is OK
	uint16_t DataTemp=0;
	//Reset Cell Over Voltage Alarm
	if (BMSData.BMSAlarms.OVC || CellMask_Fail_OV)
	{
		if ((!BMSData.RT_Alarms.OVC)||(!(Config.Faults_Enable&0x02)))	//Check that the fault is enabled
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_OV, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp) + 1, USE_CRC, 1); 	//Read the Real Time Fault
			if (DataTemp&MP279xConfig.CELLS_MASK)
			{
				//Clear the Fault when it is in a non-used cell
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, (MP279xConfig.CELLFT_CTRL&0x00CF) | 0x01, ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x02, 0x00, USE_CRC, 1);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, (MP279xConfig.CELLFT_CTRL&0x00FF), ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);
			}
			else I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x02, 0x00, USE_CRC, 1);	 //Clear the fault if everything is OK
			BMSData.BMSAlarms.OVC=0;
		}
	}
	//Reset Pack Over Voltage Alarm
	if (BMSData.BMSAlarms.OVP)
	{
		if ((!BMSData.RT_Alarms.OVP)||(!(Config.Faults_Enable&0x10)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x40, 0x00, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.OVP=0;
		}
	}
	//Reset Cell Under Voltage Alarm
	if (BMSData.BMSAlarms.UVC || CellMask_Fail_UV)
	{
		if ((!BMSData.RT_Alarms.UVC)||(!(Config.Faults_Enable&0x01)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_UV, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp) + 1, USE_CRC, 1);		//Read the Real Time Fault
			if (DataTemp&MP279xConfig.CELLS_MASK)
			{
				//Clear the Fault when it is in a non-used cell
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG & 0xFE, 0x00, USE_CRC, 1);
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, (MP279xConfig.CELLFT_CTRL&0x00F9) | 0x01, ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);
				SysTick_Delay_us(100);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_CELLFT_CTRL, MP279xConfig.CELLFT_CTRL&0x00FF, ((MP279xConfig.CELLFT_CTRL&0xFF00)>>8), USE_CRC, 1);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG, 0x00, USE_CRC, 1);
			}
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x01, 0x00, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.UVC=0;
		}
	}
	//Reset Cell Dead Alarm
	if (BMSData.BMSAlarms.DeadC || CellMask_Fail_DeadC)
	{
		if ((!BMSData.RT_Alarms.DeadC)||(!(Config.Faults_Enable&0x04)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_DEAD, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp) + 1, USE_CRC, 1);	//Read the Real Time Fault
			if (DataTemp&MP279xConfig.CELLS_MASK)
			{
				//Clear the Fault when it is in a non-used cell
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG & 0xFE, 0x00, USE_CRC, 1);
				SysTick_Delay_us(10);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACKFT_CTRL, MP279xConfig.PACKFT_CTRL&0x00FF, ((MP279xConfig.PACKFT_CTRL&0xFC00)>>8), USE_CRC, 1);
				SysTick_Delay_us(100);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_SAFE_CONFIG, MP279xConfig.SAFE_CONFIG, 0x00, USE_CRC, 1);	
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACKFT_CTRL, MP279xConfig.PACKFT_CTRL&0x00FF, ((MP279xConfig.PACKFT_CTRL&0xFF00)>>8) | 0x80, USE_CRC, 1);				
			}
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x04, 0x00, USE_CRC, 1);	//Clear the fault if everything is OK
			BMSData.BMSAlarms.DeadC=0;
			CellDead_CPending=1;
		}
	}
	//Reset Pack Under Voltage Alarm
	if (BMSData.BMSAlarms.UVP)
	{	
		if ((!BMSData.RT_Alarms.UVP)||(!(Config.Faults_Enable&0x08)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x20, 0x00, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.UVP=0;
		}
	}
	//Reset Discharge Over Current Alarm
	if (BMSData.BMSAlarms.DOC)
	{
		I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_OC_STATUS, (uint8_t *) (&DataTemp), ((uint8_t *) &DataTemp) + 1, USE_CRC, 1);
		if (((!(DataTemp&0x0100))||(!(Config.Faults_Enable&0x20))) && ((!(DataTemp&0x0200))||(!(Config.Faults_Enable&0x40))))
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x00, 0x0C, USE_CRC, 1);
			BMSData.BMSAlarms.DOC=0;
		}
	}
	//Reset Discharge Short Circuit Alarm
	if (BMSData.BMSAlarms.DSC)
	{
		if ((!BMSData.RT_Alarms.DSC)||(!(Config.Faults_Enable&0x0100)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x00, 0x20, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.DSC=0;
		}
	}
	//Reset Charge Over Current Alarm
	if (BMSData.BMSAlarms.COC)
	{
		if ((!BMSData.RT_Alarms.COC)||(!(Config.Faults_Enable&0x80)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x00, 0x10, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.COC=0;
		}
	}
	//Reset Charge Short Circuit Alarm
	if (BMSData.BMSAlarms.CSC)
	{
		if ((!BMSData.RT_Alarms.CSC)||(!(Config.Faults_Enable&0x0200)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x00, 0x40, USE_CRC, 1);		//Clear the fault if everything is OK	
			BMSData.BMSAlarms.CSC=0;
		}
	}
	//Reset PCB Temperature Alarm
	if (BMSData.BMSAlarms.TPCB)
	{
		if ((!BMSData.RT_Alarms.TPCB)||(!(Config.Faults_Enable&0x1000)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x80, 0x00, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.TPCB=0;
		}
	}
	//Reset Battery Over Temperature Alarm
	if (BMSData.BMSAlarms.BatOT)
	{
		if ((!BMSData.RT_Alarms.BatOT)||(!(Config.Faults_Enable&0x0400)))		//Check that the fault is enabled
		{
			if (!BMSData.BMSAlarms.BatUT) I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x00, 0x01, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.BatOT=0;
		}
	}
	//Reset Battery Under Temperature Alarm
	if (BMSData.BMSAlarms.BatUT)
	{
		if ((!BMSData.RT_Alarms.BatUT)||(!(Config.Faults_Enable&0x0800)))		//Check that the fault is enabled
		{
			if (!BMSData.BMSAlarms.BatOT) I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x00, 0x02, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.BatUT=0;
		}
	}
	//Reset MP279x IC Temperature Alarm
	if (BMSData.BMSAlarms.Tchip)
	{
		if (!BMSData.RT_Alarms.Tchip)
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x00, 0x80, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.Tchip=0;
		}
	}
	//Reset Open Wire Alarm
	if (BMSData.BMSAlarms.OW)
	{
		if ((!BMSData.RT_Alarms.OW)||(!(Config.Features_Enable&0x01)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x10, 0x00, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.OW=0;
		}
	}
	//Reset Cell Mismatch Alarm
	if (BMSData.BMSAlarms.MSMT || CellMask_Fail_MSMT)
	{
		if ((!BMSData.RT_Alarms.MSMT)||(!(Config.Faults_Enable&0x2000)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Read16B(I2C2,ADDRESS_MP279x,REG_RD_CELL_MSMT, (uint8_t *) &DataTemp, ((uint8_t *) &DataTemp) + 1, USE_CRC, 1);		//Read the Real Time Fault
			if ((DataTemp&(~MP279xConfig.CELLS_MASK))==(~MP279xConfig.CELLS_MASK))
			{
				//Clear the Fault when it is in a non-used cell
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACKFT_CTRL, MP279xConfig.PACKFT_CTRL&0x00FF, ((MP279xConfig.PACKFT_CTRL&0xE700)>>8), USE_CRC, 1);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x08, 0x00, USE_CRC, 1);
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_PACKFT_CTRL, MP279xConfig.PACKFT_CTRL&0x00FF, ((MP279xConfig.PACKFT_CTRL&0xFF00)>>8), USE_CRC, 1);
			}
			else I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_CLR, 0x08, 0x00, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.MSMT=0;
		}
	}
	//Reset FET Driver Error
	if (BMSData.BMSAlarms.FET_Drv)
	{
		if (!BMSData.RT_Alarms.FET_Drv)
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_REC, MP279xConfig.FT_REC&0x00FF, 0x20 | ((MP279xConfig.FT_REC&0xFF00)>>8), USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.FET_Drv=0;
		}
	}
	//Reset Chip Sense Alarm
	if (BMSData.BMSAlarms.ChipSenseError)
	{
		if ((!BMSData.RT_Alarms.ChipSenseError)||(!(Config.Features_Enable&0x40)))		//Check that the fault is enabled
		{
			I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_INT1_CLR, 0x00, 0x04, USE_CRC, 1);		//Clear the fault if everything is OK
			BMSData.BMSAlarms.ChipSenseError=0;
		}
	}
	//Reset Chip System Alarm
	if ((!CommError)&&(ADC_WDG<100))
	{
		if (BMSData.BMSAlarms.ChipSystemError)
		{
			if (!BMSData.RT_Alarms.ChipSystemError)
			{
				I2C_MP279xC_Write16B(I2C2,ADDRESS_MP279x,REG_FT_REC, MP279xConfig.FT_REC&0x00FF, 0xC0 | ((MP279xConfig.FT_REC&0xFF00)>>8), USE_CRC, 1);		//Clear the fault if everything is OK
				BMSData.BMSAlarms.ChipSystemError=0;
			}
		}
		if (!((*((__packed uint32_t *) &BMSData.BMSAlarms))&0x0003FFFF))
		{
			if (BMSData.BMSstatus!=AUTO_RECOVERY)
			{
				BMSData.BMSstatus=RESET_ALARM;
				if (StandByFlag) Reset_Alarms_done=2;
				else Reset_Alarms_done=1;
			}
			RSET_LD4;
		}
		else
		{
			Reset_Alarms_done = 0;
			StandByFlag=0;
			SafeFlag=0;
		}
	}
	else NVIC_SystemReset();		//Run a System Reset
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
	while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) __NOP();
	SysTick_Delay_Done = 1;
}

//Check if the IC goes to HardFault Handler
void HardFault_Handler(void)
{
	while(1)
	{
		RSET_LD0;
		SET_LD1;
		SysTick_Delay_us(100000);
		SET_LD0;
		RSET_LD1;
		SysTick_Delay_us(100000);
	}
}

float ADC_to_Temperature(float adcValue) 
{
    // Direnc bolucu voltaji hesapla
    float Vdiv = adcValue * 3.3 / 4095.0;
    // NTC direncini hesapla (10k direnc ile)
    float Rntc = (R1 * Vdiv) / (3.3 - Vdiv);
    // Beta formulu kullanarak sicakligi hesapla
    float T = (1.0 / ((1.0 / 298.15) + (1.0 / BETA) * log(Rntc / 10000.0))) - 273.15;
    return T;
}

