/************************************************************************************
*         File: MPF4279x.h																													*
*					Description: Header for the MPF4279x_I2C.c file												 		*
*					CPU: STM32F03xx Medium density devices																		*
*					Project:	OPU, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Red Origin Technologies																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/
#include "stm32f0xx.h"                  // Device header
#include "BMS_MP279x.h"

#define bool _Bool
//MPF4279x Commands MAP
#define FG_Trigger          	   		0x7FFE

#define MPF4279x_Address						0x08
#define MPF4279x_CRC_EN							1
#define NO_STOP											1
#define ADD_STOP										0

#ifdef FG_NewGen
typedef enum{
	CHARGE, DISCHARGE, REST
}def_fg_status;

typedef enum{
	Cell, Pack, Chgr, Smrtchgr
}def_fg_lim;

typedef enum{
	Disabled, Paused, Ongoing
}def_fg_lrn;

typedef enum{
	Default, Updated, Updated2Max, Updated2Min
}def_fg_rslt;

typedef struct __attribute__((packed)){
	unsigned char id;
	unsigned chg_cc:			1;
	unsigned chg_end:			1;
	unsigned dis:				1;
}def_fg_OT;

typedef struct __attribute__((packed)){
	unsigned iteration:		1;
	unsigned status: 		1;
	unsigned emty_lim:		1;
	unsigned full_lim:		1;
	unsigned soc_pack_rslt: 1;
	unsigned OT_warn:		1;
	unsigned idis_avg:  	1;
	unsigned idis_end:  	1;
	unsigned ichg_cc:  		1;
	unsigned ichg_end:  	1;
	unsigned vchg_cv:  		1;
	unsigned soresr_cells:  1;
	unsigned rcxn_cells:  	1;
	unsigned hconv:  		1;
	unsigned soh_cells:  	1;
	unsigned rsvd:  	1;
	unsigned pdis_lim:  	1;
	unsigned pchg_lim:  	1;
}def_fg_intr;

typedef __packed struct __attribute__((packed)){

	def_fg_intr		intr;

	//	STATUS
	def_fg_status	status;
	
	unsigned char reserved0[288];

	// 	SOC CELLS
	unsigned char 	soc_abs_cells[SETMAX_NCELLS_SER];
	
	unsigned char 	reserved1[68];

	//	EMPTY
	unsigned char	emty_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	emty_id;
	unsigned short 	emty_rtime;
	signed short	emty_dT;
	def_fg_lim		emty_lim;
	
	unsigned char 	reserved2[4];
	
	//	FULL
	unsigned char	full_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	full_id;
	unsigned short 	full_rtime;
	unsigned short 	full_rtime_cc;
	signed short	full_dT;
	signed short	full_dT_cc;
	def_fg_lim		full_lim;

	// 	SOC PACK
	unsigned short	soc_pack;
	unsigned char 	reserved3[1];
	unsigned short 	soc_pack_unavbl;

	// -------------------
	// 		LEARNINGS
	// -------------------

	//	IDIS_AVG
	unsigned short	idis_avg;
	def_fg_lrn		idis_avg_lrn;
	def_fg_rslt		idis_avg_rslt;

	//	IDIS_END
	unsigned short	idis_end;
	def_fg_lrn		idis_end_lrn;
	def_fg_rslt		idis_end_rslt;

	//	ICHG_CC
	unsigned short	ichg_cc;
	def_fg_lrn		ichg_cc_lrn;
	def_fg_rslt		ichg_cc_rslt;

	//	ICHG_END
	unsigned short	ichg_end;
	def_fg_lrn		ichg_end_lrn;
	def_fg_rslt		ichg_end_rslt;

	//	VCHG_CV
	unsigned short	vchg_cv;
	def_fg_lrn		vchg_cv_lrn;
	def_fg_rslt		vchg_cv_rslt;

	//	ESR
	float		 	soresr_dis_cells[SETMAX_NCELLS_SER];
	float		 	soresr_chg_cells[SETMAX_NCELLS_SER];
	def_fg_lrn		soresr_lrn_cells[SETMAX_NCELLS_SER];
	def_fg_rslt		soresr_rslt_cells[SETMAX_NCELLS_SER];

	// RCXN_CELLS
	unsigned short 	rcxn_cells[SETMAX_NCELLS_SER];
	def_fg_lrn		rcxn_lrn_cells[SETMAX_NCELLS_SER];
	def_fg_rslt		rcxn_rslt_cells[SETMAX_NCELLS_SER];

	// HCONV
	unsigned short	hconv;
	def_fg_lrn		hconv_lrn;
	def_fg_rslt		hconv_rslt;

	// SOH
	unsigned long 	soh_pack;
	unsigned long 	soh_cells[SETMAX_NCELLS_SER];
	
	unsigned char 	reserved4[192];

	// -------------------
	// 		POWER
	// -------------------

	// POWER ESTIMATION
	unsigned long	prdg;
	unsigned long	pdis;
	unsigned char	pdis_id;
	def_fg_lim		pdis_lim;
	unsigned long	pchg;
	unsigned char	pchg_id;
	def_fg_lim		pchg_lim;

	// WARNINGS
	def_fg_OT		OT_warn;

	// OTHER
	unsigned long 	iteration;

}def_fg_out;
#define fg_out_size sizeof(def_fg_out)
	
typedef struct __attribute__((packed)){
	unsigned hard:			1;
	unsigned fg:				1;
	unsigned log:				1;
	unsigned wkup:			1;
	unsigned status:		1;
	unsigned soh:				1;
	unsigned RES00:			1;
	unsigned soc:				1;
	unsigned full:			1;
	unsigned emty:			1;
	unsigned idis_avg:	1;
	unsigned idis_end:	1;
	unsigned ichg_end:	1;
	unsigned ichg_cc:		1;
	unsigned vchg_cv:		1;
	unsigned soresr:		1;
	unsigned hconv:			1;
	unsigned RES01:				1;
}def_fg_rst;

typedef struct __attribute__((packed)){
	unsigned pchg:			1;
	unsigned pdis:			1;
}def_fg_shw;

typedef struct __attribute__((packed))
{
	uint16_t 		Cells[SETMAX_NCELLS_SER];
	uint16_t 		VPack;
	int32_t 		Current[SETMAX_NCELLS_SER];
	int32_t 		PackCurrent;
	int32_t 		I_CC;
	int16_t			Tamb;
	int16_t 		TempVect[4];
	int16_t 		TChip;
	uint16_t 		CellsBalancing;
	def_fg_rst 	rst;
	def_fg_shw	shw;
} Meas_REG;
#define Measurements_size sizeof(Meas_REG)

#define VFG_Meas_NoRead_Size				6
#define LTL_REG_size 								148
#define FG_Status_size							5
#define fgset_main_size							12
#define fgset_en_size								4
#define fgset_intr_size							3
#define fgset_vbat_size							12
#define fgset_ibat_size							6
#define fgset_rbat_size							38
#define fgset_Tbat_size							10
#define fgset_nbat_size							3
#define fgset_load_size							4
#define fgset_chgr_size							7
#define fgset_empty_size						6
#define fgset_full_size							7
#define fgset_ichg_cc_lrn_size			5
#define fgset_ichg_end_lrn_size			5
#define fgset_vchg_cv_lrn_size			5
#define fgset_idis_avg_lrn_size			3
#define fgset_idis_end_lrn_size			5
#define fgset_soh_lrn_size					9
#define fgset_resr_lrn_size					4
#define fgset_rcxn_cells_lrn_size		4
#define fgset_hconv_lrn_size				4
#define LEDs_REG_size								3
#ifdef MPF42791
#define I2Cset_size									1
#else
#define I2Cset_size									2
#endif
#define I2Csts_size									1
#define fgcust_pass_size						5
#else 

typedef enum{
	CHARGE, DISCHARGE, REST
}def_fg_status;

typedef enum{
	Cell, Pack
}def_fg_lim;

typedef struct __attribute__((packed)){

	unsigned char reserved0[356];

	//	EMPTY
	unsigned char	emty_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	emty_id;
	unsigned short 	emty_rtime;
	def_fg_lim		emty_lim;

	unsigned char reserved1[4];
	
	//	FULL
	unsigned char	full_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	full_id;
	unsigned short 	full_rtime;
	def_fg_lim		full_lim;

	//	IDIS_AVG 
	unsigned short	idis_avg;

	unsigned char reserved2[1];
	
	//	IDIS_END
	unsigned short	idis_end;

	unsigned char reserved3[2];


	//	ICHG_CC
	unsigned short	ichg_cc;

	unsigned char reserved4[2];


	//	ICHG_END
	unsigned short	ichg_end;

	unsigned char reserved5[2];


	//	VCHG_CV 
	unsigned short	vchg_cv;

	unsigned char reserved6[2];


	// 	SOC (CELL & PACK)
	unsigned short 	soc_pack_unavbl;
	unsigned short	soc_pack;

	unsigned char reserved7[1];

	unsigned char 	soc_abs_cells[SETMAX_NCELLS_SER];

	unsigned char reserved8[80];

	// SOH
	unsigned long 	soh_pack;
	unsigned long 	soh_cells[SETMAX_NCELLS_SER];

	unsigned char reserved9[192];

	// POWER ESTIMATION
	unsigned long	prdg;
	unsigned long	pdis;
	unsigned char	pdis_id;
	def_fg_lim		pdis_lim;
	unsigned long	pchg;
	unsigned char	pchg_id;
	def_fg_lim		pchg_lim;

	// OTHER (1 bytes)
	def_fg_status	status;
	unsigned long iteration;
}def_fg_out;
#define fg_out_size sizeof(def_fg_out)

typedef struct __attribute__((packed)){
	unsigned hard:			1;
	unsigned fg:			1;
	unsigned log:			1;
	unsigned wkup:			1;
	unsigned status:		1;
	unsigned soh:			1;
	unsigned RES00:			1;
	unsigned soc:			1;
	unsigned full:			1;
	unsigned emty:			1;
	unsigned idis_avg:		1;
	unsigned idis_end:		1;
	unsigned ichg_end:		1;
	unsigned ichg_cc:		1;
	unsigned vchg_cv:		1;
	unsigned RES01:			1;
}def_fg_rst;

typedef struct __attribute__((packed)){
	unsigned pchg:			1;
	unsigned pdis:			1;
	unsigned emty_rtime:	1;
	unsigned full_rtime:	1;
}def_fg_shw;

typedef struct __attribute__((packed))
{
	uint16_t Cells[SETMAX_NCELLS_SER];
	uint16_t VPack;
	int32_t Current[SETMAX_NCELLS_SER];
	int32_t PackCurrent;
	int32_t I_CC;
	int16_t TempVect[4];
	int16_t TChip;
	uint16_t CellsBalancing;
	def_fg_rst rst;
	def_fg_shw shw;
} Meas_REG;
#define Measurements_size sizeof(Meas_REG)
	
#define VFG_Meas_NoRead_Size			5
#define LTL_REG_size 							148
#define FG_Status_size						5
#define fgset_main_size						11
#define fgset_en_size							3
#define fgset_bat_size						63
#define fgset_vm_size							15
#define fgset_empty_size					7
#define fgset_full_size						11
#define fgset_chg_size						6
#define fgset_dis_size						5
#define fgset_soh_size						7
#define fgset_esr_size						2
#define LEDs_REG_size							3
#define I2Cset_size								1

#endif

//I2C MPF4279x Functions
uint8_t I2C_MPF4279x_Write(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t *Data, uint8_t len, uint8_t Activate, uint8_t CRC_En);
uint8_t I2C_MPF4279x_Read(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t *Data, uint8_t len, uint8_t Activate, uint8_t CRC_En);
uint8_t MPF4279x_to_Active_Mode(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t len, uint8_t no_stop);
