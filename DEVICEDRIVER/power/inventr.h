#ifndef __INVENTR_H
#define __INVENTR_H	 
#include "sys.h"


#define INVENTR_MAX_CURRENT_ADC_VAL	1058		//实测值1058  0x0422
#define INVENTR_MAX_VOLTAGE_ADC_VAL	90			//实测值90  0x005A

#define INVENTR_MAX_CURRENT_MA		1050		//最大输出电流1050ma
#define INVENTR_MAX_VOLTAGE_V		272.108844f		//最大输出电压272.108844v,VMAX =Power/(Iomax*70%)=200w/(1.05A*70%)= 204.1v(其中70%是固定值，常量)


#define RELAY_ON	GPIO_SetBits(GPIOC,GPIO_Pin_7);
#define RELAY_OFF	GPIO_ResetBits(GPIOC,GPIO_Pin_7);


extern u8 InventrBusy;
extern u8 InventrDisable;

extern float InventrInPutCurrent;
extern float InventrInPutVoltage;
extern float InventrOutPutCurrent;
extern float InventrOutPutVoltage;


void RELAY_Init(void);

u8 InventrSetMaxPowerCurrent(u8 percent);
u8 InventrSetLightLevel(u8 level);
float InventrGetOutPutCurrent(void);
float InventrGetOutPutVoltage(void);
u8 InventrGetDeviceInfo(void);

void SetLightLevel(u8 intfc_type, u8 level);





































#endif
