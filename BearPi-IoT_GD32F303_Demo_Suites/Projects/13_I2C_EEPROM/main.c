/*!
    \file  main.c
    \brief use the I2C bus to write and read EEPROM
    

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
#include "at24cxx.h"
#include "gd32f30x.h"
#include "i2c.h"
#include "systick.h"
#include <stdio.h>

void led_config(void);

/*!
    \brief      configure the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED);

    /* turn off LED */
    gd_eval_led_off(LED);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* configure LEDs */
    led_config();

    /* configure USART */
    gd_eval_com_init(EVAL_COM0,115200);

    printf("I2C-24C02 configured....\n\r");

    /* configure GPIO */
    gpio_config();

    /* configure I2C */
    i2c_config();

    /* initialize EEPROM  */
    i2c_eeprom_init();

    printf("\r\nThe I2C0 is hardware interface ");
    printf("\r\nThe speed is %d", I2C0_SPEED);

    if (I2C_OK == i2c_24c02_test())
    {
        while (1)
        {
            /* turn off all LED */
            gd_eval_led_off(LED);
            /* turn on a LED */
            gd_eval_led_off(LED);

            delay_1ms(500);
        }
    }
    /* turn on all LED */
    gd_eval_led_on(LED);

    while (1)
        ;
}


