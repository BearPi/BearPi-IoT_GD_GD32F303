/*!
    \file  main.c
    \brief GPIO key polling mode demo
    
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
#include <stdio.h>
#include "systick.h"

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

    /* enable the LED GPIO clock */
    gd_eval_led_init(LED);

    /* enable the F1 key GPIO clock */
    gd_eval_key_init(F1_KEY, KEY_MODE_GPIO);
	
		/* enable the F2 key GPIO clock */
		gd_eval_key_init(F2_KEY, KEY_MODE_GPIO);

    while(1){
        /* check whether the button is pressed */
        if(RESET == gd_eval_key_state_get(F1_KEY)){
            /* delay 100ms for software removing jitter */
            delay_1ms(100);

            /* check whether the button is pressed */
            if(RESET == gd_eval_key_state_get(F1_KEY)){
                gd_eval_led_on(LED);
            }
        }
				
				if(RESET == gd_eval_key_state_get(F2_KEY)){
            /* delay 100ms for software removing jitter */
            delay_1ms(100);

            /* check whether the button is pressed */
            if(RESET == gd_eval_key_state_get(F2_KEY)){
                gd_eval_led_off(LED);
            }
        }
    }
}
