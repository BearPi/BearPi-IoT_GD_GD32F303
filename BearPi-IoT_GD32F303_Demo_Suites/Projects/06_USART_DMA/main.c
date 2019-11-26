/*!
    \file  main.c
    \brief USART DMA transmitter receiver
    
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

#include "gd32f30x.h"
#include "BearPi-IoT_gd32f303.h"
#include "systick.h"
#include <stdio.h>

uint8_t tx_buffer[] = {"welcome to BearPi-IoT!\r\n"};

#define ARRAYNUM(arr_name)     (uint32_t)(sizeof(arr_name)/sizeof(*(arr_name)))
#define USART0_DATA_ADDRESS    ((uint32_t)0x40013804)
uint8_t rx_buffer[ARRAYNUM(tx_buffer)];
volatile ErrStatus transfer_status = ERROR; 

void led_init(void);
void led_flash(int times);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);
void usart_dma_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the LED */
    led_init();
    
    /* configure systick */
    systick_config();
    
    /* flash the LEDs for 1 time */
    led_flash(1);
    
    /* configure EVAL_COM0 */
    gd_eval_com_init(EVAL_COM0);
    
    /* configure USART DMA */
    usart_dma_config();
    
    /* enable USART0 DMA channel transmission and reception */
    dma_channel_enable(DMA0, DMA_CH3);
    dma_channel_enable(DMA0, DMA_CH4);
    
    /* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    usart_dma_receive_config(USART0, USART_DENR_ENABLE);
    
    /* wait until USART0 TX DMA0 channel transfer complete */
    while(RESET == dma_flag_get(DMA0, DMA_CH3, DMA_INTF_FTFIF)){
    }

    /* wait until USART0 RX DMA0 channel receive complete */
    while(RESET == dma_flag_get(DMA0, DMA_CH4, DMA_INTF_FTFIF)){
    }
    /* check the received data with the send ones */
    transfer_status = memory_compare(tx_buffer , rx_buffer , ARRAYNUM(tx_buffer));
    
    while(1){
        if(SUCCESS == transfer_status){
            /* turn on LED */
            gd_eval_led_on(LED);
            delay_1ms(200);
            /* turn off all the LED */
            gd_eval_led_off(LED);
            delay_1ms(200);
        }else{
            /* flash LED for status error */
            led_flash(1);
        }
    }
}

/*!
    \brief      configure USART DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA0 */
    rcu_periph_clock_enable(RCU_DMA0);
    /* deinitialize DMA channel3(USART0 tx) */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)tx_buffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(tx_buffer);
    dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);
    
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)rx_buffer;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);
    dma_memory_to_memory_disable(DMA0, DMA_CH4);
}

/*!
    \brief      initialize the LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    gd_eval_led_init(LED);
}

/*!
    \brief      flash the LED for test
    \param[in]  times: times to flash the LED
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for(i=0; i<times; i++){
        /* delay 400 ms */
        delay_1ms(400);

        /* turn on LED */
        gd_eval_led_on(LED);

        /* delay 400 ms */
        delay_1ms(400);

        /* turn off LED */
        gd_eval_led_off(LED);
    }
}

/*!
    \brief      memory compare function
    \param[in]  src: source data
    \param[in]  dst: destination data
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) 
{
    while(length--){
        if(*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
