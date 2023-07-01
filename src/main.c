#include "stm32l476xx.h"
#include "helper_functions.h"
#include "7seg.h"
#include "keypad.h"
#include "led_button.h"
#include "timer.h"
#include "lcd.h"
#include "gpio.h"
#include "delay.h"
#include "ds18b20.h"
#include "game.h"
#include <string.h>
// Define pins for 7seg
#define SEG_gpio GPIOC
#define DIN_pin 5
#define CS_pin 6
#define CLK_pin 7
// Define pins for keypad
// If need to change need to also change EXTI_Setup and IRQHandler
#define COL_gpio GPIOA
#define COL_pin 5       // 5 6 7 8
#define ROW_gpio GPIOC
#define ROW_pin 0       // 0 1 2 3
// Define pins for button (default use on-board button PC13)
#define BUTTON_gpio GPIOC
#define BUTTON_pin 13
// Define Counter timer
#define COUNTER_timer TIM2
// Remember to use correct "startup_stm32.s"
LCD1602A lcd;
GPIOPin dq;
int GameStart = 0;
int now_col = 3;
int Key_Cnt = 0, KeyValue = -1;
int AlarmState = 0, Alarm_Timer = 0;
int button_press_cycle_per_second = 10;
// Use to state how many cycles to check per button_press_cycle
int debounce_cycles = 100;
// Use to state the threshold when we consider a button press
int debounce_threshold = 70;
// Used to implement negative edge trigger 0=not-presses 1=pressed
int last_button_state=0;
// 0 is not setting step, 1 is Timer Counting, 2 is Time up.
int nowPos = 0, status = 0;
struct Game_Filed GameObj;
int gamecount = 0;

char *ready = "Press BlueButton";
char *start = "Start Game";
char *over = "Game Over";
char *restart = "BlueButton Retry";
void EXTIKeyPadHandler(int r)
{
	int nowKey = keypad[r][(now_col+3)%4];
	if(nowKey == KeyValue)
	{
		Key_Cnt++;
	}
	else
	{
		Key_Cnt = 0;
	}
	KeyValue = nowKey;
	if(Key_Cnt >= 10)
	{
		Key_Cnt = 10;
		compare_input_element(&GameObj, KeyValue);
		int display = GameObj.level*10000000 + GameObj.goal;
		display_number(SEG_gpio, DIN_pin, CS_pin, CLK_pin, display, 8);
	}
}
void EXTI_Setup()
{
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0_Msk;
SYSCFG->EXTICR[0] |= (2 << SYSCFG_EXTICR1_EXTI0_Pos);
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1_Msk;
SYSCFG->EXTICR[0] |= (2 << SYSCFG_EXTICR1_EXTI1_Pos);
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2_Msk;
SYSCFG->EXTICR[0] |= (2 << SYSCFG_EXTICR1_EXTI2_Pos);
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3_Msk;
SYSCFG->EXTICR[0] |= (2 << SYSCFG_EXTICR1_EXTI3_Pos);


EXTI->IMR1 |= EXTI_IMR1_IM0;
EXTI->IMR1 |= EXTI_IMR1_IM1;
EXTI->IMR1 |= EXTI_IMR1_IM2;
EXTI->IMR1 |= EXTI_IMR1_IM3;

EXTI->FTSR1 |= EXTI_FTSR1_FT0;
EXTI->FTSR1 |= EXTI_FTSR1_FT1;
EXTI->FTSR1 |= EXTI_FTSR1_FT2;
EXTI->FTSR1 |= EXTI_FTSR1_FT3;

NVIC_EnableIRQ(EXTI0_IRQn);
NVIC_EnableIRQ(EXTI1_IRQn);
NVIC_EnableIRQ(EXTI2_IRQn);
NVIC_EnableIRQ(EXTI3_IRQn);


}
void EXTI0_IRQHandler()
{
	if(EXTI->PR1&EXTI_PR1_PIF0_Msk)
	{
		EXTIKeyPadHandler(0);
		EXTI->PR1 = EXTI_PR1_PIF0_Msk;
	}
}
void EXTI1_IRQHandler()
{
	if(EXTI->PR1&EXTI_PR1_PIF1_Msk)
	{
		EXTIKeyPadHandler(1);
		EXTI->PR1 = EXTI_PR1_PIF1_Msk;
	}
}
void EXTI2_IRQHandler()
{
	if(EXTI->PR1&EXTI_PR1_PIF2_Msk)
	{
		EXTIKeyPadHandler(2);
		EXTI->PR1 = EXTI_PR1_PIF2_Msk;
	}
}
void EXTI3_IRQHandler()
{
	if(EXTI->PR1&EXTI_PR1_PIF3_Msk)
	{
		EXTIKeyPadHandler(3);
		EXTI->PR1 = EXTI_PR1_PIF3_Msk;
	}
}
void LCDDisplay(){
	LCDClearScreen(&lcd);
	LCDPrintString(&lcd, GameObj.topstream, 0,  0);
	LCDPrintString(&lcd, GameObj.botstream, 1,  0);
}
void Keypad_update()
{
	reset_push(COL_gpio, now_col + COL_pin);
	now_col = (now_col+1)%4;
	set_push(COL_gpio, now_col + COL_pin);
}
void SysTick_Handler() {
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
	{
		/*
		 * 這邊執行固定速率畫面的更新包含7SEG及Keypad input
		 */
		Keypad_update();

	}
}
void LCDInit(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	for(int i=0;i<=10;i++){
		GPIOB->MODER &= ~(3 << (i * 2));
		GPIOB->MODER |= (1 << (i * 2));
	}
	GPIOPin gpio;
	for(int i=0;i<8;i++){
		GPIOInit(&gpio, GPIOB, i);
		LCDSetDATA(&lcd, &gpio, i);
	}
	GPIOInit(&gpio, GPIOB, 8);
	LCDSetRS(&lcd, &gpio);
	GPIOInit(&gpio, GPIOB, 9);
	LCDSetRW(&lcd, &gpio);
	GPIOInit(&gpio, GPIOB, 10);
	LCDSetEN(&lcd, &gpio);
}
int Init()
{
	// Cause we want to use floating points we need to init FPU
	FPU_init();
	if(init_button(BUTTON_gpio, BUTTON_pin) != 0){
		// Fail to init button
		return -1;
	}
	if(init_7seg_number(SEG_gpio, DIN_pin, CS_pin, CLK_pin) != 0)
	{
		return -1;
	}
	if(init_keypad(ROW_gpio, COL_gpio, ROW_pin, COL_pin) != 0)
	{
		return -1;
	}
	LCDInit();
	EXTI_Setup();
	return 0;

}
void SysClock(){
	int plln = 20;
	int pllm = 1;
	int pllr = 1;
	plln <<= RCC_PLLCFGR_PLLN_Pos;
	pllm <<= RCC_PLLCFGR_PLLM_Pos;
	pllr <<= RCC_PLLCFGR_PLLR_Pos;
	RCC->CFGR &= ~(RCC_CFGR_SW_Msk);
	RCC->CR &= ~(RCC_CR_PLLON);
	while((RCC->CR & RCC_CR_PLLRDY));
	//config
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN_Msk);
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk);
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLR_Msk);
	RCC->PLLCFGR |= plln;
	RCC->PLLCFGR |= pllm;
	RCC->PLLCFGR |= pllr;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;
	RCC->CR |= RCC_CR_PLLON;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}
void SysTickConfig(int tick){
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	SysTick->LOAD = tick & SysTick_LOAD_RELOAD_Msk;
	SysTick->VAL = 0;
	SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk);
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
int read_button(){
	int cnt = 0;
	for(int i=200;i;i--){
		cnt += (GPIOC->IDR >> 13) & 1;
	}
	return cnt < 100;
}
int main()
{
Init();
SysClock();
SysTickConfig(10000);
LCDInitSetting(&lcd);
LCDClearScreen(&lcd);
LCDPrintString(&lcd, ready, 0,  0);
LCDPrintString(&lcd, start, 1,  0);
init_game(&GameObj);
while(1){

	if(read_button())
	{
		init_game(&GameObj);
		GameStart = 1;
		int display = GameObj.level*10000000 + GameObj.goal;
		display_number(SEG_gpio, DIN_pin, CS_pin, CLK_pin, display, 8);
	}
	if(GameStart)
	{
		gamecount++;
		check_event_happend(&GameObj, gamecount);
		LCDDisplay();
	}
	if(is_game_over(&GameObj))
	{
		GameStart = 0;
		LCDClearScreen(&lcd);
		LCDPrintString(&lcd, over, 0,  0);
		LCDPrintString(&lcd, restart, 1,  0);
		GameObj.top_temp = ' ';
		GameObj.bot_temp = ' ';
	}
}

	return 0;
}
