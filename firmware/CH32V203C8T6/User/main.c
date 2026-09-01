/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */

#include "debug.h"

#define LM1972_CH1_ADDRESS 0 
#define LM1972_CH2_ADDRESS 1

#define LM1972_VOL_MAX     0
#define LM1972_VOL_MUTE    128

static u_int8_t lm1972_mapping[] = {0x80,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
                                    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
                                    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
                                    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
                                    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
                                    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7A,0x59,0x50,0x48,0x43,0x41,0x3E,0x3C,0x3A,
                                    0x39,0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x30,0x2F,0x2E,0x2D,0x2C,0x2B,0x2B,0x2A,
                                    0x29,0x28,0x28,0x27,0x26,0x26,0x25,0x24,0x24,0x23,0x23,0x22,0x22,0x21,0x21,0x20,
                                    0x20,0x1F,0x1F,0x1E,0x1E,0x1D,0x1D,0x1C,0x1C,0x1B,0x1B,0x1A,0x1A,0x19,0x19,0x18,
                                    0x18,0x17,0x17,0x17,0x16,0x16,0x16,0x15,0x15,0x14,0x14,0x14,0x14,0x13,0x13,0x13,
                                    0x12,0x12,0x12,0x12,0x11,0x11,0x11,0x11,0x10,0x10,0x10,0x10,0x10,0x0F,0x0F,0x0F,
                                    0x0F,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0C,0x0C,0x0C,
                                    0x0C,0x0C,0x0C,0x0C,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0A,0x0A,
                                    0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x09,0x09,
                                    0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,
                                    0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09};

static uint16_t ch1_volume_data = 255;
static uint16_t ch1_volume_last = 0;

static uint16_t ch2_volume_data = 255;
static uint16_t ch2_volume_last = 0;

void CLK1_IRQ (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));  // Line 3
void CLK2_IRQ (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));  // Line 4

void CLK1_IRQ (void) {
    EXTI->INTFR = EXTI_Line3;
    ch1_volume_data = (uint16_t)GPIOB->INDR >> 8;
    return;
}

void CLK2_IRQ (void) {
    EXTI->INTFR = EXTI_Line4;
    ch2_volume_data = (uint16_t)GPIOB->INDR >> 8;
    return;
}


/*********************************************************************
 * @fn      SPI1_ReadWriteByte
 *
 * @brief   SPI1 read or write one byte.
 *
 * @param   TxData - write one byte data.
 *
 * @return  Read one byte data.
 */
void SPI1_WriteByte(u8 TxData)
{
    u8 i = 0;
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, TxData);
    i = 0;
}

void SPI1_WriteChannel(u8 chan, u8 val)
{
    GPIO_WriteBit (GPIOA, GPIO_Pin_4, Bit_RESET); 
    SPI1_WriteByte(chan);
    SPI1_WriteByte(lm1972_mapping[val]);
    //SPI1_WriteByte(val);
    Delay_Us (10);
    GPIO_WriteBit (GPIOA, GPIO_Pin_4, Bit_SET); 
}


/*********************************************************************
 * @fn      SPI_1Lines_HalfDuplex_Init
 *
 * @brief   Configuring the SPI for half-duplex communication.
 *
 * @return  none
 */
void SPI_1Lines_HalfDuplex_Init(void)
{
    printf("Entering SPI_1Lines_HalfDuplex_Init\r\n");
	GPIO_InitTypeDef GPIO_InitStructure={0};
	SPI_InitTypeDef SPI_InitStructure={0};
	NVIC_InitTypeDef NVIC_InitStructure={0};

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );


	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SPI1, &SPI_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	SPI_Cmd( SPI1, ENABLE );

}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init (115200);
    printf ("SystemClk:%d\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf ("This is printf example\r\n");

    SPI_1Lines_HalfDuplex_Init();


    EXTI_InitTypeDef EXTI_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);

    // Data in - technically shoudn't be required as those values should be defauls values
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

    // LED PC14

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);


    GPIO_EXTILineConfig (GPIO_PortSourceGPIOB, GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init (&EXTI_InitStructure);

    GPIO_EXTILineConfig (GPIO_PortSourceGPIOB, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init (&EXTI_InitStructure);

    NVIC_EnableIRQ (EXTI3_IRQn);
    NVIC_SetPriority (EXTI3_IRQn, 0);
    SetVTFIRQ ((u32)CLK1_IRQ, EXTI3_IRQn, 0, ENABLE);

    NVIC_EnableIRQ (EXTI4_IRQn);
    NVIC_SetPriority (EXTI4_IRQn, 0);
    SetVTFIRQ ((u32)CLK2_IRQ, EXTI4_IRQn, 1, ENABLE);


    while (1) {

        if (ch1_volume_data != ch1_volume_last)
        {
            ch1_volume_last = ch1_volume_data;
            printf("CH1: %02x = %02x\r\n", ch1_volume_last, lm1972_mapping[ch1_volume_last]);
            GPIO_WriteBit (GPIOC, GPIO_Pin_14, Bit_RESET);  // Led ON/OFF
            Delay_Ms (1);
        }

        if (ch2_volume_data != ch2_volume_last)
        {
            ch2_volume_last = ch2_volume_data;
            
            printf("CH1: %02x = %02x\r\n", ch2_volume_last, lm1972_mapping[ch2_volume_last]);
            GPIO_WriteBit (GPIOC, GPIO_Pin_14, Bit_RESET);  // Led ON/OFF
            Delay_Ms (1);
        }

        SPI1_WriteChannel(LM1972_CH1_ADDRESS, ch1_volume_last);
        SPI1_WriteChannel(LM1972_CH2_ADDRESS, ch2_volume_last);

        GPIO_WriteBit (GPIOC, GPIO_Pin_14, Bit_SET);
        Delay_Ms (1);
    }
}
