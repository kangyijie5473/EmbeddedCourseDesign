// Author:kangkang
// Time:2018.12.21

#include "config.h"
int judgeKeyBoard(const uint32 key);
void recoveryLight();
void clearAllLight();
void specialFlash();

const uint32 green_light = (1 << 17);
const uint32 red_light = (1 << 18);
const uint32 yellow_light = (1 << 19);
const uint32 blue_light = (1 << 20);
const uint32 on_special_key = 1 << 16;
const uint32 off_special_key = 1<< 15;
const uint32 N = 100;
//对应的数码管引脚
const uint32 Nixie_a = (1 << 6);	
const uint32 Nixie_b = (1 << 9);	
const uint32 Nixie_c = (1 << 10);
const uint32 Nixie_d = (1 << 4);
const uint32 Nixie_e = (1 << 12);
const uint32 Nixie_f = (1 << 13);
const uint32 Nixie_g = (1 << 5);
//到时标志
int timeout_flag;
//状态
#define START  0
#define EAST_WEST_GO 1
#define NORTH_SOUTH_GO 2
#define SPECIAL_FLASH 3
#define EAST_WEST_FLASH 4
#define NORTH_SOUTH_FLASH  5

int count = 91;

int state;
int flash_state;

int red_enable;
int green_enable;
int yellow_enable;
int blue_enable;
int LED_enable;


void displayLED(int num){
	IO0CLR =  Nixie_a | Nixie_b | Nixie_c | Nixie_d | Nixie_e | Nixie_f| Nixie_g;
	if(num == 0){
		IO0SET =  Nixie_a | Nixie_b | Nixie_c | Nixie_d | Nixie_e | Nixie_f;
	}
	if(num == 1){
		IO0SET =  Nixie_b | Nixie_c ;
	}
	if(num == 2){
		IO0SET =  Nixie_a | Nixie_b | Nixie_g | Nixie_e | Nixie_d;
	}
	if(num == 3){
		IO0SET =  Nixie_a | Nixie_b | Nixie_g | Nixie_c | Nixie_d;

	}
	if(num == 4){
		IO0SET =  Nixie_f | Nixie_g | Nixie_b | Nixie_c;
	}
	if(num == 5){
		IO0SET =  Nixie_a | Nixie_f | Nixie_g | Nixie_c | Nixie_d;
	}
	if(num == 6){
		IO0SET =  Nixie_a | Nixie_f | Nixie_g | Nixie_e | Nixie_c | Nixie_d;
	}
	if(num == 7){
		IO0SET =  Nixie_a | Nixie_b | Nixie_c;
	}
	if(num == 8){
		IO0SET =  Nixie_a | Nixie_b | Nixie_c | Nixie_d | Nixie_e | Nixie_f| Nixie_g;
	}
	if(num == 9){
		IO0SET =  Nixie_a | Nixie_b | Nixie_c | Nixie_d | Nixie_f | Nixie_g;
	}

}

void clearAllLight()
{
	IO0SET = yellow_light | red_light| green_light | blue_light;
	yellow_enable = 0;
	green_enable = 0;
	blue_enable = 0;
	red_enable = 0;
}
void enableYellow()
{

	IO0CLR = yellow_light;
	yellow_enable = 5;
}
void enableBlue()
{
	IO0CLR = blue_light;
	blue_enable = 6;
}
void enableRed()
{
	IO0CLR = red_light;
	red_enable = 7;
}
void enableGreen()
{
	IO0CLR = green_light;
	green_enable = 8;
}
void recoveryLight()
{
	int on_light = 0;
	if (green_enable)
		on_light |= green_light;
	if (blue_enable)
		on_light |= blue_light;
	if (red_enable)
		on_light |= red_light;
	if (yellow_enable)
		on_light |= yellow_light;
	IO0CLR = on_light;
}

void DelayNS (uint32 dly)
{
	uint32 i;
	
	for ( ; dly>0; dly--)
		for (i=0; i<50000; i++);
}
void TimerInit(void)
{
	T0TC = 0;
	T0PR = 0;
	T0MCR = 0x03;
	T0MR0 = Fpclk / 10 ; // 每次0.1s
	T0TCR = 0x01;
}

void __irq IRQ_func(void)
{
	timeout_flag = 1;

	T0IR = 0x01;			
	VICVectAddr = 0x00;		
	return;
}
//主逻辑
void doWork(void)
{
	count--;
	if (count % 10 == 0)
		displayLED(count / 10);

	if (judgeKeyBoard(on_special_key)) {
		state = SPECIAL_FLASH;
		count = 31;
		return;
	}
	switch(state) {
		case START:
			clearAllLight();
			enableBlue();
			enableRed();
			state = NORTH_SOUTH_GO;
			count = 90;	
			break;
		case EAST_WEST_GO :

			if (count == 30) {
				state = EAST_WEST_FLASH;
			}
			break;
		case NORTH_SOUTH_GO :
			if (count == 30) {
				state = NORTH_SOUTH_FLASH;	
			}
			break;
		case EAST_WEST_FLASH:
			if (count % 5 == 0)
				clearAllLight();
			else {
				enableGreen();
				enableYellow();		
			} 
				
			if (count == 10) {
				clearAllLight();
				enableBlue();
				enableRed();
				state = NORTH_SOUTH_GO;
				count = 90;
			} 

			break;		
		case NORTH_SOUTH_FLASH:
			if (count % 5 == 0)
				clearAllLight();
			else {
				enableBlue();
				enableRed();
			}
			if (count == 10) {
				clearAllLight();
				enableGreen();
				enableYellow();
				state = EAST_WEST_GO;
				count = 90;
			}

			break;
		case SPECIAL_FLASH:
			if(count == 10){
				state = START;
				count = 90;
				break;
				}
			clearAllLight();
			switch(flash_state) {
				case 0:
				case 6:
					enableRed();
					flash_state = red_enable;
					break;
				case 7:
					enableGreen();
					flash_state = green_enable;
					break;
				case 8:
					enableYellow();
					flash_state = yellow_enable;
					break;
				case 5:
					enableBlue();
					flash_state = blue_enable;
					break;
			}
			
		
	}
	

	
	/*1S定时版
	if (judgeKeyBoard(on_special_key)) {
		while(judgeKeyBoard(off_special_key) == 0)
			specialFlash();
		recoveryLight();
	}


	if (count % 10 == 0)
		displayLED(count / 10);
	
	if (state == START || (state == EAST_WEST_GO && count == 1)) {
		clearAllLight();
		enableBlue();
		enableRed();
		state = NORTH_SOUTH_GO;
		count = 9;
	}else if (state == NORTH_SOUTH_GO && count == 1) {
		clearAllLight();
		enableGreen();
		enableYellow();
		state = EAST_WEST_GO;
		count = 9;
	}

	*/
}
void IRQInit(void)
{
	VICIntSelect = 0x00;	//set IRQ interupt			
	VICVectCntl0 = 0x20 | 0x04;		
	VICVectAddr0 = (uint32)IRQ_func;	//set interupt process address
	
	VICIntEnable = 1 << 0x04;
}
int judgeKeyBoard(const uint32 key)
{
	PINSEL0 = 0x00000000;
	if ((IO0PIN & key) == 0)
		return 1;
	else 
		return 0;
}
// 使用延时做的流水灯
// 适合前期测试
/*
void specialFlash()
{

	IO0CLR = 0x00;
	IO0SET = IO0DIR;
	DelayNS(10);
	IO0CLR = green_light;
	DelayNS(10);
	IO0SET = green_light;
	DelayNS(10);
	IO0CLR = yellow_light;
	DelayNS(10);
	IO0SET = yellow_light;
	DelayNS(10);
	IO0CLR = red_light;
	DelayNS(10);
	IO0SET = red_light;
	DelayNS(10);
	IO0CLR = blue_light;
	DelayNS(10);
	IO0SET = blue_light;
	
}
*/


int main (void)
{
	uint8 i;
	
	IO0DIR  = yellow_light | red_light| green_light | blue_light| Nixie_a | Nixie_b | Nixie_c | Nixie_d | Nixie_e | Nixie_f| Nixie_g;			
	TimerInit(); 
	IRQInit();
	IRQEnable();
	clearAllLight();
	while(1) {
		if (timeout_flag) {
			doWork();
			timeout_flag = 0;
		}
	} 
	/*	 
	while (1)
	{
		IO0CLR = 0x00;

		IO0SET = IO0DIR;
		if (judgeKeyBoard(on_special_key))
			while(judgeKeyBoard(off_special_key) == 0)
				specialFlash();

		for (i=1; i<=20 ; i++) // 20S green
		{	
			DelayNS(N);
			IO0CLR = green_light;
			DelayNS(N);
			IO0SET = green_light;
			if (judgeKeyBoard(on_special_key))
				while(judgeKeyBoard(off_special_key) == 0)
					specialFlash();
		}


		for (i=1; i<=3; i++)  // 3S yellow
		{	
			DelayNS(N);
			IO0CLR = yellow_light;
			DelayNS(N);
			IO0SET = yellow_light;
			if (judgeKeyBoard(on_special_key))
				while(judgeKeyBoard(off_special_key) == 0)
					specialFlash();
		}

		for (i=1; i<=20; i++) // 20 red
		{	
			DelayNS(N);
			IO0CLR = red_light;
			DelayNS(N);
			IO0SET = red_light;
			if (judgeKeyBoard(on_special_key))
				while(judgeKeyBoard(off_special_key) == 0)
					specialFlash();
		}
	
	}
	*/
	
    return 0;
}
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
