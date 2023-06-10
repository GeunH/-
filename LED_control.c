#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_
#endif

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_
#include "def.h"
#include "stm32f7xx_hal.h"
static void SystemClock_Config(void);
#endif

void hwInit(void);
void MyApp();

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }

  /* activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    //Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    //Error_Handler();
  }
}

void TurnOffinLED();

void segment(int number)
{
  unsigned char Num[8] = {
      0b0110000, // 1
      0b1101101, // 2
      0b1111001, // 3
      0b0110011, // 4
      0b1011011, // 5
      0b0011111, // 6
      0b1110010, // 7
      0b1111110, // 0
  };
  unsigned char num = Num[number-1];
  for (int i=0; i< 7 ; i++)
  {
    *(volatile unsigned int*)(0x40020418U) &= ~(0x00000100U);
    *(volatile unsigned int*)(0x40020418U) |= 0x01000000U;
    if(!(num & 0b00000001))
    {
      *(volatile unsigned int*)(0x40020418U) &= ~(0x01000000U);
      *(volatile unsigned int*)(0x40020418U) |= 0x00000100U;
    }

    *(volatile unsigned int*)(0x40020418U) |= 0x00000200U;

    *(volatile unsigned int*)(0x40020418U) &= ~(0x00000200U);
    *(volatile unsigned int*)(0x40020418U) |= 0x02000000U;


    num = num >> 1;
  }

  *(volatile unsigned int*)(0x40020418U) |= 0x00000080U;

  *(volatile unsigned int*)(0x40020418U) &= ~(0x00000080U);     // ST_CP PB7, ARD 1;
  *(volatile unsigned int*)(0x40020418U) |= 0x00800000U;

}

int MyDelay(unsigned char state, unsigned int n, int *reset)
{
  int before = 0;
  int time = n*180000*5;
  int next =0;
  volatile unsigned int delay;
  for(delay = 0 ; delay <= time; delay ++)
  {
    if (*(volatile unsigned int*)(0x40021810U) & 0x00000008U)
    {
      before = 1;
    }
    else if( !(*(volatile unsigned int*)(0x40020810U) & 0x00000002U) )
    {
      before = 2;
    }
    else
    {
      if(before == 1)
      {
	TurnOffinLED();
	*reset = 1;
      }
      else if(before == 2)
      {
	next =1;
	return next;
      }
      before = 0;
    }
  }
  return next;
}

int main(void)
{
  hwInit();
  MyApp();
}

void hwInit(void)
{
  SCB_EnableICache();
  SCB_EnableDCache();
  HAL_Init();
  SystemClock_Config();
}


// ARD_D2 =PG 6, ARD_D4 = PG7  ARD D1 = PC 6, ARD_D0 = PC7, ARD_D7 = PC1

void init()
{

  *(volatile unsigned int*)(0x40023830U) |= 0x00000010U;
  *(volatile unsigned int*)(0x40021800U) &= ~(0x000000C0U);
  *(volatile unsigned int*)(0x40021808U) &= ~(0x000000C0U);
  *(volatile unsigned int*)(0x4002180CU) &= ~(0x000000C0U);


  *(volatile unsigned int*)(0x40023830U) |= 0x00000040U;
  *(volatile unsigned int*)(0x40021800U) |= 0x01000000U;
  *(volatile unsigned int*)(0x40021808U) |= 0x03000000U;    // 내부 LED 1
  *(volatile unsigned int*)(0x4002180CU) |= 0x01000000U;

  *(volatile unsigned int*)(0x40021818U) &= 0xEFFFFFFFU;
  *(volatile unsigned int*)(0x40021818U) |= 0x00001000U;

  *(volatile unsigned int*)(0x40023830U) |= 0x00000010U;
  *(volatile unsigned int*)(0x40021000U) |= 0x00000400U;
  *(volatile unsigned int*)(0x40021008U) |= 0x00000C00U;    // 내부 LED 2
  *(volatile unsigned int*)(0x4002100CU) |= 0x00000400U;

  *(volatile unsigned int*)(0x40021018U) &= 0xFFDFFFFFU;
  *(volatile unsigned int*)(0x40021018U) |= 0x00000020U;

  *(volatile unsigned int*)(0x40023830U) |= 0x00000010U;
  *(volatile unsigned int*)(0x40021000U) |= 0x00000100U;
  *(volatile unsigned int*)(0x40021008U) |= 0x00000300U;
  *(volatile unsigned int*)(0x4002100CU) |= 0x00000100U;    // 내부 LED 3

  *(volatile unsigned int*)(0x40021018U) &= 0xFFEFFFFFU;
  *(volatile unsigned int*)(0x40021018U) |= 0x00000010U;

  *(volatile unsigned int*)(0x40023830U) |= 0x00000040U;
  *(volatile unsigned int*)(0x40021800U) |= 0x00100000U;
  *(volatile unsigned int*)(0x40021808U) |= 0x00300000U;
  *(volatile unsigned int*)(0x4002180CU) |= 0x00100000U;    // 내부 LED 4

  *(volatile unsigned int*)(0x40021818U) &= 0xFBFFFFFFU;
  *(volatile unsigned int*)(0x40021818U) |= 0x00000400U;

  *(volatile unsigned int*)(0x40023830U) |= 0x00000040U;
  *(volatile unsigned int*)(0x40021800U) |= 0x00004000U;
  *(volatile unsigned int*)(0x40021808U) |= 0x0000C000U;    // A ; PG 7
  *(volatile unsigned int*)(0x4002180CU) |= 0x00004000U;
  *(volatile unsigned int*)(0x40021818U) |= 0x00800000U;

  *(volatile unsigned int*)(0x40021800U) |= 0x00001000U;
  *(volatile unsigned int*)(0x40021808U) |= 0x00003000U;   // B ; PG 6
  *(volatile unsigned int*)(0x4002180CU) |= 0x00001000U;
  *(volatile unsigned int*)(0x40021818U) |= 0x00400000U;

  *(volatile unsigned int*)(0x40023830U) |= 0x00000004U;
  *(volatile unsigned int*)(0x40020800U) |= 0x00004000U;
  *(volatile unsigned int*)(0x40020808U) |= 0x0000C000U;   // C: PC7
  *(volatile unsigned int*)(0x4002080CU) |= 0x00004000U;
  *(volatile unsigned int*)(0x40020818U) |= 0x00800000U;

  *(volatile unsigned int*)(0x40020800U) |= 0x00000000U;
  *(volatile unsigned int*)(0x40020808U) |= 0x0000000CU;   // C: PC1
  *(volatile unsigned int*)(0x4002080CU) |= 0x00000004U;

  *(volatile unsigned int*)(0x40020800U) |= 0x00000010U;
  *(volatile unsigned int*)(0x40020808U) |= 0x00000030U;   // walker_green2 : PC2
  *(volatile unsigned int*)(0x4002080CU) |= 0x00000010U;
  *(volatile unsigned int*)(0x40020818U) |= 0x00040000U;

  *(volatile unsigned int*)(0x40020800U) |= 0x00001000U;
  *(volatile unsigned int*)(0x40020808U) |= 0x00003000U;   // walker_green1 : PC6
  *(volatile unsigned int*)(0x4002080CU) |= 0x00001000U;
  *(volatile unsigned int*)(0x40020818U) |= 0x00400000U;

  *(volatile unsigned int*)(0x40023830U) |= 0x00000002U;
  *(volatile unsigned int*)(0x40020400U) |= 0x00004000U;
  *(volatile unsigned int*)(0x40020408U) |= 0x0000C000U;   // ST_CP PB7, ARD 14
  *(volatile unsigned int*)(0x4002040CU) |= 0x00004000U;
  *(volatile unsigned int*)(0x40020418U) |= 0x00800000U;

  *(volatile unsigned int*)(0x40020400U) |= 0x00010000U;
  *(volatile unsigned int*)(0x40020408U) |= 0x00030000U;   // DS  PB8, ARD 15
  *(volatile unsigned int*)(0x4002040CU) |= 0x00010000U;
  *(volatile unsigned int*)(0x40020418U) |= 0x00000100U;

  *(volatile unsigned int*)(0x40020400U) |= 0x00040000U;
  *(volatile unsigned int*)(0x40020408U) |= 0x000C0000U;   // SH_CP PB9, ARD 10
  *(volatile unsigned int*)(0x4002040CU) |= 0x00040000U;
  *(volatile unsigned int*)(0x40020418U) |= 0x02000000U;
}

void led_off()
{
  *(volatile unsigned int*)(0x40021818U) &= ~(0x00000080U);
  *(volatile unsigned int*)(0x40021818U) |= 0x00800000U;

  *(volatile unsigned int*)(0x40021818U) &= ~(0x00000040U);
  *(volatile unsigned int*)(0x40021818U) |= 0x00400000U;

  *(volatile unsigned int*)(0x40020818U) &= ~(0x00000080U);
  *(volatile unsigned int*)(0x40020818U) |= 0x00800000U;

  *(volatile unsigned int*)(0x40020818U) &= ~(0x00000040U);
  *(volatile unsigned int*)(0x40020818U) |= 0x00400000U;

  *(volatile unsigned int*)(0x40020818U) &= ~(0x00000004U);
  *(volatile unsigned int*)(0x40020818U) |= 0x00040000U;

  *(volatile unsigned int*)(0x40020818U) &= ~(0x00000040U);
  *(volatile unsigned int*)(0x40020818U) |= 0x00400000U;

}

void TurnOffinLED()
{
  *(volatile unsigned int*)(0x40021818U) &= 0xEFFFFFFFU;
  *(volatile unsigned int*)(0x40021818U) |= 0x00001000U;   // 내부 LED 1

  *(volatile unsigned int*)(0x40021018U) &= 0xFFDFFFFFU;
  *(volatile unsigned int*)(0x40021018U) |= 0x00000020U;   // 내부 LED 2

  *(volatile unsigned int*)(0x40021018U) &= 0xFFEFFFFFU;
  *(volatile unsigned int*)(0x40021018U) |= 0x00000010U;   // 내부 LED 3

  *(volatile unsigned int*)(0x40021818U) &= 0xFBFFFFFFU;   // 내부 LED 4
  *(volatile unsigned int*)(0x40021818U) |= 0x00000400U;
}


void control_led(unsigned char state,int duration, int* reset)
{
  duration = duration * 10;
  unsigned int A = state & 0b00000100;
  unsigned int B = state & 0b00000010;
  unsigned int C = state & 0b00000001;

  *(volatile unsigned int*)(0x40021818U) |= A << 5; // 0x0000 0080U;  // A
  *(volatile unsigned int*)(0x40021818U) |= B << 5; // 0x0000 0040U;  // B
  *(volatile unsigned int*)(0x40020818U) |= C << 7; // 0x0000 0080U;  // C
  int next;
  if(state == 0b00000000)
  {
    *(volatile unsigned int*)(0x40020818U) |= 0x00000004U;
    for (int i=0 ; i<5 ;i++)
    {
      segment(7-i);
      next= MyDelay(state,10, reset);
      if(next == 1)
      {
	break;
      }
    }
    if(next != 1)
    {
      for(int i= 1; i<= 5; i++)
      {
	*(volatile unsigned int*)(0x40020818U) |= 0x00000004U;
	segment(3-(i+2)/3);
	next = MyDelay(state,2, reset);
	if(next == 1)
	{
	  break;
	}
	*(volatile unsigned int*)(0x40020818U) &= ~(0x00000004U);
	*(volatile unsigned int*)(0x40020818U) |= 0x00040000U;
	next = MyDelay(state,2, reset);
	if(next == 1)
	{
	  break;
	}
      }
    }
    segment(8);
  }
  else if(state == 0b00000100)
  {
    *(volatile unsigned int*)(0x40020818U) |= 0x00000040U;
    next = MyDelay(state,50, reset);
    if(next != 1)
    {
      for(int i= 0; i< 5; i++)
      {
	*(volatile unsigned int*)(0x40020818U) |= 0x00000040U;
	MyDelay(state,2, reset);
	if(next == 1)break;
	*(volatile unsigned int*)(0x40020818U) &= ~(0x00000040U);
	*(volatile unsigned int*)(0x40020818U) |= 0x00400000U;
	MyDelay(state,2, reset);
	if(next == 1)
	{
	  break;
	}
      }
    }
  }
  else
  {
    MyDelay( state,duration, reset);
  }
  led_off();
}

void control_inled(unsigned char state)
{
  unsigned int led1 = 0x00000001 & state;
  unsigned int led2 = 0x00000002 & state;
  unsigned int led3 = 0x00000004 & state;
  unsigned int led4 = 0x00000008 & state;

  *(volatile unsigned int*)(0x40021818U) |= led1 << 28;  // 1000 0000 내부 LED 1

  *(volatile unsigned int*)(0x40021018U) |= led2 << 20;  // 0200 0000 내부 LED 2

  *(volatile unsigned int*)(0x40021018U) |= led3 << 18;  // 0010 0000 내부 LED 3

  *(volatile unsigned int*)(0x40021818U) |= led4 << 23;  // 0400 0000 내부 LED 4
}

void MyApp()
{
  init();
  unsigned char count = 0b00000000;
  int reset = 0;
  while(1)
  {
    TurnOffinLED();
    control_inled(count);
    control_led(0, 7, &reset);
    control_led(1, 2, &reset);
    control_led(2, 7, &reset);
    control_led(3, 2, &reset);
    control_led(4, 7, &reset);
    control_led(5, 2, &reset);
    control_led(6, 7, &reset);
    control_led(7, 2, &reset);

    if(reset > 0)
    {
      reset = 0;
      count = 1;
    }
    else
    {
      count = (count +1) % 16;
    }
  }
}
