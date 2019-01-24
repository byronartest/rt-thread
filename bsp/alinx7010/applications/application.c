/*
 * COPYRIGHT (C) 2013-2014, Shanghai Real-Thread Technology Co., Ltd
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <rtthread.h>

#include <cp15.h>

/* thread phase init */
#define LED_PL_DATA		0x41210000
#define LED_PL_DIR		0x41210004
#define BTN_PL_DATA		0x41220000
#define BTN_PL_DIR		0x41220004

static uint32_t Xil_In32(uint32_t Addr)
{
	return *(volatile uint32_t *) Addr;
}

static void Xil_Out32(uint32_t Addr, uint32_t Value)
{
	volatile uint32_t *LocalAddr = (volatile uint32_t *)Addr;
	*LocalAddr = Value;
}

static void rt_init_thread_entry(void *parameter)
{
    /* do component initialization */
	uint32_t i=0;
    rt_components_init();
    rt_kprintf("running on cpu %d\n",
               rt_cpu_get_smp_id() & 0x0f);
	Xil_Out32(LED_PL_DIR,0x0);
	Xil_Out32(BTN_PL_DIR,0xF);
		i = Xil_In32(BTN_PL_DATA);

    /* add your initialization here */
	for(;;){
		//rt_kprintf("init_thread count %d\n",i++);
		Xil_Out32(LED_PL_DATA,i&0xf);
		i++;
		rt_thread_delay(250);
	}
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init", rt_init_thread_entry, RT_NULL, 2048,
                            RT_THREAD_PRIORITY_MAX/3, 20);
    if (tid != RT_NULL)
	{
        rt_thread_startup(tid);
	}

    return 0;
}

