/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <board.h>
#include "drivers/pin.h"


#define CPU_USAGE_CALC_TICK    10
#define CPU_USAGE_LOOP        100
static rt_uint8_t  cpu_usage_major = 0, cpu_usage_minor= 0;
static rt_uint32_t total_count = 0;
static void cpu_usage_idle_hook(void)
{
  rt_tick_t tick;
  rt_uint32_t count;
  volatile rt_uint32_t loop;
  if (total_count == 0)
  {
/* get total count */
    rt_enter_critical();
    tick = rt_tick_get();
    while(rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
    {
      total_count ++;
      loop = 0;
      while (loop < CPU_USAGE_LOOP) loop ++;
    }
    rt_exit_critical();
  }
  count = 0;
/* get CPU usage */
  tick = rt_tick_get();
  while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
  {
    count ++;
    loop  = 0;
    while (loop < CPU_USAGE_LOOP) loop ++;
  }
  
      //rt_kprintf("CPU usage count is : %u.%u%%\n",count,total_count);
/* calculate major and minor */
  if (count < total_count)
  {
    count = total_count - count;
    cpu_usage_major = (count * 100) / total_count;
    cpu_usage_minor = ((count * 100) % total_count) * 100 / total_count;
  }
  else
  {
    total_count = count;
/* no CPU usage */
    cpu_usage_major = 0;
    cpu_usage_minor = 0;
  }
}
void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor)
{
  RT_ASSERT(major != RT_NULL);
  RT_ASSERT(minor != RT_NULL);
  *major = cpu_usage_major;
  *minor = cpu_usage_minor;
}
void cpu_usage_init(void)
{
/* set idle thread hook */
  rt_thread_idle_sethook(cpu_usage_idle_hook);
}

ALIGN(8)
static rt_uint8_t t1_stack[1024];
struct rt_thread t1_thread;
void t1_thread_entry(void* p)
{
    rt_pin_mode(21,PIN_MODE_OUTPUT);
    rt_pin_mode(22,PIN_MODE_OUTPUT);
    for(;;){
      rt_thread_delay(RT_TICK_PER_SECOND/16);
      rt_pin_write(22,1);
      rt_thread_delay(RT_TICK_PER_SECOND/16);
      rt_pin_write(22,0);
		}
}
ALIGN(8)
static rt_uint8_t t2_stack[1024];
struct rt_thread t2_thread;
void t2_thread_entry(void* p)
{
  rt_uint8_t major,minor,i;
    for(;;){
			i=100;
      rt_thread_delay(RT_TICK_PER_SECOND/1);
			while(i--){
				cpu_usage_get(&major,&minor);
			}
      //rt_kprintf("CPU usage is : %u.%u%%\n",major,minor);
		}
}
ALIGN(8)
static rt_uint8_t t3_stack[1024];
struct rt_thread t3_thread;
void t3_thread_entry(void* p)
{
    for(;;){
      rt_thread_delay(RT_TICK_PER_SECOND/RT_TICK_PER_SECOND);
		}
}
ALIGN(8)
static rt_uint8_t t4_stack[1024];
struct rt_thread t4_thread;
void t4_thread_entry(void* p)
{
    for(;;){
      rt_thread_delay(RT_TICK_PER_SECOND/RT_TICK_PER_SECOND);
		}
}
ALIGN(8)
static rt_uint8_t t5_stack[1024];
struct rt_thread t5_thread;
void t5_thread_entry(void* p)
{
    for(;;){
      rt_thread_delay(RT_TICK_PER_SECOND/RT_TICK_PER_SECOND);
		}
}
int main(void)
{
    rt_err_t result;
    rt_thread_t tid;
//  rt_device_t dev_pin = rt_device_find("pin");
  cpu_usage_init();
//  if(dev_pin != NULL){
//    struct rt_device_pin *pin = (struct rt_device_pin *)dev_pin;
//    //pin->
//    ///dev_pin->read();
//  }
	/* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
        rt_kprintf("File System initialzation failed!\n");
#endif  /* RT_USING_DFS */

    tid = &t1_thread;
    result = rt_thread_init(tid, "t1", t1_thread_entry, RT_NULL,
                            t1_stack, sizeof(t1_stack), RT_THREAD_PRIORITY_MAX / 4, 5);
    RT_ASSERT(result == RT_EOK);
    rt_thread_startup(tid);
	
    tid = &t2_thread;
    result = rt_thread_init(tid, "t2", t2_thread_entry, RT_NULL,
                            t2_stack, sizeof(t2_stack), RT_THREAD_PRIORITY_MAX / 4, 5);
    RT_ASSERT(result == RT_EOK);
    rt_thread_startup(tid);
		
    tid = &t3_thread;
    result = rt_thread_init(tid, "t3", t3_thread_entry, RT_NULL,
                            t3_stack, sizeof(t3_stack), RT_THREAD_PRIORITY_MAX / 4, 5);
    RT_ASSERT(result == RT_EOK);
    rt_thread_startup(tid);
		
    tid = &t4_thread;
    result = rt_thread_init(tid, "t4", t4_thread_entry, RT_NULL,
                            t4_stack, sizeof(t4_stack), RT_THREAD_PRIORITY_MAX / 4, 5);
    RT_ASSERT(result == RT_EOK);
    rt_thread_startup(tid);
		
    tid = &t5_thread;
    result = rt_thread_init(tid, "t5", t5_thread_entry, RT_NULL,
                            t5_stack, sizeof(t5_stack), RT_THREAD_PRIORITY_MAX / 3, 5);
    RT_ASSERT(result == RT_EOK);
    rt_thread_startup(tid);
		
		rt_thread_suspend(rt_thread_self());
    for(;;){
    }
  /* user app entry */
//  return 0;
}





