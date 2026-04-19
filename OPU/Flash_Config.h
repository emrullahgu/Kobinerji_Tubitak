/************************************************************************************
*         File: Flash_Config.h																											*
*					Description: Contains the Flash configuration														  *
*					CPU: STM32F03xx Medium density devices																		*
*					Project:	OPU, Battery Management Unit																		*
*					Version:	1.0																															*
*					Author:		Red Origin Technologies																				  *
*					Date:			06/05/2024																											*
*************************************************************************************/

#ifndef FLASH_CONFIG_H_
#define FLASH_CONFIG_H_

__IO uint16_t *Cell_UV_FLASH = (__IO uint16_t*) FL_Config_ADDRESS;
__IO uint16_t *Cell_UV_Hyst_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+2);
__IO uint16_t *Cell_UV_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+4);
__IO uint16_t *Cell_OV_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+6);
__IO uint16_t *Cell_OV_Hyst_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+8);
__IO uint16_t *Cell_OV_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+10);
__IO uint16_t *Cell_Dead_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+12);
__IO uint16_t *Cell_Dead_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+14);
__IO uint16_t *Cell_MSMT_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+16);
__IO uint16_t *Cell_MSMT_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+18);
__IO uint16_t *Pack_UV_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+20);
__IO uint16_t *Pack_UV_Hyst_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+22);
__IO uint16_t *Pack_UV_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+24);
__IO uint16_t *Pack_OV_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+26);
__IO uint16_t *Pack_OV_Hyst_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+28);
__IO uint16_t *Pack_OV_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+30);
__IO uint16_t *Pack_DOC1_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+32);
__IO uint16_t *Pack_DOC1_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+34);
__IO uint16_t *Pack_DOC2_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+36);
__IO uint16_t *Pack_DOC2_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+38);
__IO uint16_t *Pack_DSC_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+40);
__IO uint16_t *Pack_DSC_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+42);
__IO uint16_t *Pack_COC_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+44);
__IO uint16_t *Pack_COC_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+46);
__IO uint16_t *Pack_CSC_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+48);
__IO uint16_t *Pack_CSC_Dgl_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+50);
__IO uint16_t *Bat_DCH_OT_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+52);
__IO uint16_t *Bat_DCH_UT_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+54);
__IO uint16_t *Bat_CHG_OT_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+56);
__IO uint16_t *Bat_CHG_UT_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+58);
__IO uint16_t *Cells_NTC_Config_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+60);
__IO uint16_t *PCB_OT_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+62);
__IO uint16_t *PCB_OT_Hyst_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+64);
__IO uint16_t *I_Gain_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+66);
__IO int16_t *CurrOffset = (__IO int16_t*) (FL_Config_ADDRESS+68);
__IO uint16_t *NumofSeriesFLASH = (__IO uint16_t*) (FL_Config_ADDRESS+70);
__IO uint16_t *BetaNTCBatFLASH = (__IO uint16_t*) (FL_Config_ADDRESS+72);
__IO uint16_t *Sby_Time_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+74);
__IO uint16_t *Sby_I_Thr_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+76);
__IO uint16_t *MinBalV_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+78);
__IO uint16_t *BalConfig_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+80);
__IO uint16_t *Faults_Enable_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+82);
__IO uint16_t *Faults_AutoRecovery_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+84);
__IO uint16_t *Features_Enable_FLASH = (__IO uint16_t*) (FL_Config_ADDRESS+86);
__IO uint16_t *I_Sense_R_FLASH = (__IO uint16_t *) (FL_Config_ADDRESS+88);

#endif /* FLASH_CONFIG_H_ */
