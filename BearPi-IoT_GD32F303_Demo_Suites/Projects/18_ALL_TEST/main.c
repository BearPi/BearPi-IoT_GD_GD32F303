/*!
    \file  main.c
    \brief USART printf demo
    

    \version 2019-10-30, V1.0.0, demo for BearPi-IoT_GD32F30x

*/

/*
    Copyright (c) 2019, BearPi LIMITED.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "BearPi-IoT_gd32f303.h"
#include "gd25qxx.h"
#include "gd32f30x.h"
#include "lcd.h"
#include "sdcard.h"
#include "systick.h"
#include <stdio.h>

uint8_t AT_tx_buffer[] = {"AT\r\n"};
uint8_t AT_rx_buffer[2];

// uint8_t WE_buffer[] = {"welcome to BearPi-IoT!\r\n"};

#define BUFFER_SIZE 256
#define TX_BUFFER_SIZE BUFFER_SIZE
#define RX_BUFFER_SIZE BUFFER_SIZE

volatile uint16_t tx_counter = 0, rx_counter = 0;
uint32_t nbr_data_to_read = BUFFER_SIZE, nbr_data_to_send = BUFFER_SIZE;

#define VERSION_ID "1.0.0"
#define SFLASH_ID 0xC84017
#define FLASH_WRITE_ADDRESS 0x000002
#define FLASH_READ_ADDRESS FLASH_WRITE_ADDRESS

uint32_t int_device_serial[3];
uint8_t count;
uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t rx_buffer[TX_BUFFER_SIZE];
uint32_t flash_id = 0;
uint16_t i = 0;
uint8_t is_successful = 0;

sd_card_info_struct sd_cardinfo; /* information of SD card */
uint32_t buf_write[512];         /* store the data written to the card */
uint32_t buf_read[512];          /* store the data read from the card */

ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);
void nvic_config(void);
sd_error_enum sd_io_init(void);
void card_info_get(void);
void SPI_FLASH_TEST(void);
void SD_TEST(void);
void WAN_TEST(void);
void KEF_TEST(void);
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* systick configuration*/
    systick_config();
    /* configure the NVIC, USART and LED */
    nvic_config();
    gd_eval_com_init(EVAL_COM0,115200);
    gd_eval_com_init(EVAL_COM1,9600);
    gd_eval_led_init(LED);

    //	  /* USART interrupt configuration */
    //    nvic_irq_enable(USART1_IRQn, 0, 0);
    //		/* enable USART0 receive interrupt */
    //    usart_interrupt_enable(USART1, USART_INT_RBNE);
    //
    //    /* enable USART0 transmit interrupt */
    //    usart_interrupt_enable(USART1, USART_INT_TBE);

    LCD_Init();
    LCD_Clear(BLACK); //����Ϊ��ɫ
    POINT_COLOR = GREEN;
    SPI_FLASH_TEST();
    SD_TEST();
    KEF_TEST();
    //		WAN_TEST();

    printf("\n\rss:%s\n\r", rx_buffer);
}
void WAN_TEST(void)
{
    //	usart_data_transmit(EVAL_COM1, AT_tx_buffer[4]);
    ////
    //	AT_rx_buffer[100]= (uint8_t)usart_data_receive(USART1);

    //	usart_data_transmit(EVAL_COM0, WE_buffer[0]);
    //	usart_data_transmit(EVAL_COM0, AT_rx_buffer[12]);
}
void KEF_TEST(void)
{
    /* enable the F1 key GPIO clock */
    gd_eval_key_init(F1_KEY, KEY_MODE_GPIO);

    /* enable the F2 key GPIO clock */
    gd_eval_key_init(F2_KEY, KEY_MODE_GPIO);

    LCD_ShowString(0, 100, 240, 24, 24, "Please press Button!");

    while (1)
    {
        /* check whether the button is pressed */
        if (RESET == gd_eval_key_state_get(F1_KEY))
        {
            /* delay 100ms for software removing jitter */
            delay_1ms(100);

            /* check whether the button is pressed */
            if (RESET == gd_eval_key_state_get(F1_KEY))
            {
                gd_eval_led_on(LED);
                LCD_ShowString(0, 150, 240, 24, 24, "This is F1!");
            }
        }

        if (RESET == gd_eval_key_state_get(F2_KEY))
        {
            /* delay 100ms for software removing jitter */
            delay_1ms(100);

            /* check whether the button is pressed */
            if (RESET == gd_eval_key_state_get(F2_KEY))
            {
                gd_eval_led_off(LED);
                LCD_ShowString(0, 150, 240, 24, 24, "This is F2!");
            }
        }
    }

    //	usart_data_transmit(EVAL_COM1, AT_tx_buffer[4]);
    ////
    //	AT_rx_buffer[100]= (uint8_t)usart_data_receive(USART1);

    //	usart_data_transmit(EVAL_COM0, WE_buffer[0]);
    //	usart_data_transmit(EVAL_COM0, AT_rx_buffer[12]);
}

void SPI_FLASH_TEST(void)
{
    /* configure SPI0 GPIO and parameter */
    spi_flash_init();
    /* get flash id */
    flash_id = spi_flash_read_id();
    printf("\n\rThe Flash_ID:0x%X\n\r", flash_id);

    /* flash id is correct */
    if (SFLASH_ID == flash_id)
    {
        printf("\n\r\n\rWrite to tx_buffer:\n\r\n\r");

        /* printf tx_buffer value */
        for (i = 0; i < BUFFER_SIZE; i++)
        {
            tx_buffer[i] = i;
            printf("0x%02X ", tx_buffer[i]);

            if (15 == i % 16)
                printf("\n\r");
        }

        printf("\n\r\n\rRead from rx_buffer:\n\r\n\r");

        /* erases the specified flash sector */
        spi_flash_sector_erase(FLASH_WRITE_ADDRESS);
        /* write tx_buffer data to the flash */
        spi_flash_buffer_write(tx_buffer, FLASH_WRITE_ADDRESS, TX_BUFFER_SIZE);

        delay_1ms(10);

        /* read a block of data from the flash to rx_buffer */
        spi_flash_buffer_read(rx_buffer, FLASH_READ_ADDRESS, RX_BUFFER_SIZE);
        /* printf rx_buffer value */
        for (i = 0; i <= 255; i++)
        {
            printf("0x%02X ", rx_buffer[i]);
            if (15 == i % 16)
                printf("\n\r");
        }

        if (ERROR == memory_compare(tx_buffer, rx_buffer, 256))
        {
            printf("Err:Data Read and Write aren't Matching.\n\r");
            is_successful = 1;
        }

        /* spi flash test passed */
        if (0 == is_successful)
        {
            printf("\n\rSPI-GD25Q64 Test Passed!\n\r");
            LCD_ShowString(0, 0, 240, 24, 24, "GD25Q64 Test Passed!");
        }
    }
    else
    {
        /* spi flash read id fail */
        printf("\n\rSPI Flash: Read ID Fail!\n\r");
        LCD_ShowString(0, 0, 240, 24, 24, "GD25Q64 Test Failed!");
    }
}

void SD_TEST(void)
{
    sd_error_enum sd_error;
    uint16_t i = 5;
#ifdef DATA_PRINT
    uint8_t *pdata;
#endif /* DATA_PRINT */

    /* initialize the card */
    do
    {
        sd_error = sd_io_init();
    } while ((SD_OK != sd_error) && (--i));

    if (i)
    {
        printf("\r\n Card init success!\r\n");
        LCD_ShowString(0, 50, 240, 24, 24, "SD Card Test Passed!");
    }
    else
    {
        printf("\r\n Card init failed!\r\n");
        LCD_ShowString(0, 50, 240, 24, 24, "Don't Find SD Card!");
    }
}

/*!
    \brief      configure the NVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(SDIO_IRQn, 0, 0);
}

/*!
    \brief      initialize the card, get the card information, set the bus mode and transfer mode
    \param[in]  none
    \param[out] none
    \retval     sd_error_enum
*/
sd_error_enum sd_io_init(void)
{
    sd_error_enum status = SD_OK;
    uint32_t cardstate = 0;
    status = sd_init();
    if (SD_OK == status)
    {
        status = sd_card_information_get(&sd_cardinfo);
    }
    if (SD_OK == status)
    {
        status = sd_card_select_deselect(sd_cardinfo.card_rca);
    }
    status = sd_cardstatus_get(&cardstate);
    if (cardstate & 0x02000000)
    {
        printf("\r\n the card is locked!");
        while (1)
        {
        }
    }
    if ((SD_OK == status) && (!(cardstate & 0x02000000)))
    {
        /* set bus mode */
        // status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
        status = sd_bus_mode_config(SDIO_BUSMODE_1BIT);
    }
    if (SD_OK == status)
    {
        /* set data transfer mode */
        status = sd_transfer_mode_config(SD_DMA_MODE);
        //         status = sd_transfer_mode_config( SD_POLLING_MODE );
    }
    return status;
}

/*!
    \brief      get the card information and print it out by USRAT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void card_info_get(void)
{
    uint8_t sd_spec, sd_spec3, sd_spec4, sd_security;
    uint32_t block_count, block_size;
    uint16_t temp_ccc;
    printf("\r\n Card information:");
    sd_spec = (sd_scr[1] & 0x0F000000) >> 24;
    sd_spec3 = (sd_scr[1] & 0x00008000) >> 15;
    sd_spec4 = (sd_scr[1] & 0x00000400) >> 10;
    if (2 == sd_spec)
    {
        if (1 == sd_spec3)
        {
            if (1 == sd_spec4)
            {
                printf("\r\n## Card version 4.xx ##");
            }
            else
            {
                printf("\r\n## Card version 3.0x ##");
            }
        }
        else
        {
            printf("\r\n## Card version 2.00 ##");
        }
    }
    else if (1 == sd_spec)
    {
        printf("\r\n## Card version 1.10 ##");
    }
    else if (0 == sd_spec)
    {
        printf("\r\n## Card version 1.0x ##");
    }

    sd_security = (sd_scr[1] & 0x00700000) >> 20;
    if (2 == sd_security)
    {
        printf("\r\n## SDSC card ##");
    }
    else if (3 == sd_security)
    {
        printf("\r\n## SDHC card ##");
    }
    else if (4 == sd_security)
    {
        printf("\r\n## SDXC card ##");
    }

    block_count = (sd_cardinfo.card_csd.c_size + 1) * 1024;
    block_size = 512;
    printf("\r\n## Device size is %dKB ##", sd_card_capacity_get());
    printf("\r\n## Block size is %dB ##", block_size);
    printf("\r\n## Block count is %d ##", block_count);

    if (sd_cardinfo.card_csd.read_bl_partial)
    {
        printf("\r\n## Partial blocks for read allowed ##");
    }
    if (sd_cardinfo.card_csd.write_bl_partial)
    {
        printf("\r\n## Partial blocks for write allowed ##");
    }
    temp_ccc = sd_cardinfo.card_csd.ccc;
    printf("\r\n## CardCommandClasses is: %x ##", temp_ccc);
    if ((SD_CCC_BLOCK_READ & temp_ccc) && (SD_CCC_BLOCK_WRITE & temp_ccc))
    {
        printf("\r\n## Block operation supported ##");
    }
    if (SD_CCC_ERASE & temp_ccc)
    {
        printf("\r\n## Erase supported ##");
    }
    if (SD_CCC_WRITE_PROTECTION & temp_ccc)
    {
        printf("\r\n## Write protection supported ##");
    }
    if (SD_CCC_LOCK_CARD & temp_ccc)
    {
        printf("\r\n## Lock unlock supported ##");
    }
    if (SD_CCC_APPLICATION_SPECIFIC & temp_ccc)
    {
        printf("\r\n## Application specific supported ##");
    }
    if (SD_CCC_IO_MODE & temp_ccc)
    {
        printf("\r\n## I/O mode supported ##");
    }
    if (SD_CCC_SWITCH & temp_ccc)
    {
        printf("\r\n## Switch function supported ##");
    }
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while (length--)
    {
        if (*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}
