/*!
    \file  main.c
    \brief USART echo interrupt demo
    
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

#define BUFFER_SIZE   (COUNTOF(tx_buffer))
#define COUNTOF(a)   (sizeof(a)/sizeof(*(a)))

uint8_t tx_buffer[] = {"welcome to BearPi-IoT!\r\n"};

uint8_t rx_buffer[BUFFER_SIZE];
volatile uint16_t tx_counter = 0, rx_counter = 0;
uint32_t nbr_data_to_read = BUFFER_SIZE, nbr_data_to_send = BUFFER_SIZE;
volatile ErrStatus transfer_status = ERROR; 

void led_init(void);
void led_flash(int times);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);

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
    
    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 0, 0);
    
    /* flash the LEDs for 1 time */
    led_flash(1);
    
    /* configure EVAL_COM0 */
    gd_eval_com_init(EVAL_COM0);
    
    /* enable USART0 receive interrupt */
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    
    /* enable USART0 transmit interrupt */
    usart_interrupt_enable(USART0, USART_INT_TBE);

    /* wait until USART0 send the tx_buffer */
    while(tx_counter < nbr_data_to_send){
    }
    
    /* wait until USART0 receive the rx_buffer */
    while(rx_counter < nbr_data_to_read){
    }
    
    /* check the received data with the send ones */
    transfer_status = memory_compare(tx_buffer, rx_buffer, BUFFER_SIZE);
    
    while(1){
        if(SUCCESS == transfer_status){
            /* turn on LED */
            gd_eval_led_on(LED);
            delay_1ms(200);
            /* turn off LED */
            gd_eval_led_off(LED);
            delay_1ms(200);
        }else{
            /* flash LED for status error */
            led_flash(1);
        }
    }
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
    \brief      flash the LEDs for test
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
