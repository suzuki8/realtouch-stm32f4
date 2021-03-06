/*
 * 程序清单：事件例程
 *
 * 这个程序会创建3个动态线程及初始化一个静态事件对象
 * 一个线程等待于事件对象上以接收事件；
 * 一个线程定时发送事件 (事件3)
 * 一个线程定时发送事件 (事件5)
 */

#include <stdio.h>

#include <board.h>
#include <rtthread.h>

void rt_init_thread_entry(void* parameter)
{

}

/* 事件控制块 */
static struct rt_event event;


ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;
/* 线程1入口函数 */
static void thread1_entry(void *param)
{
    rt_uint32_t e;

    /* receive first event */
    if (rt_event_recv(&event, ((1 << 3) | (1 << 5)),
        RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
        RT_WAITING_FOREVER, &e) == RT_EOK)
    {
        rt_kprintf("thread1: AND recv event 0x%x\n", e);
    }

    rt_kprintf("thread1: delay 1s to prepare second event\n");
    rt_thread_delay(RT_TICK_PER_SECOND);

    /* receive second event */
    if (rt_event_recv(&event, ((1 << 3) | (1 << 5)),
        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
        RT_WAITING_FOREVER, &e) == RT_EOK)
    {
        rt_kprintf("thread1: OR recv event 0x%x\n", e);
    }
    rt_kprintf("thread1 leave.\n");
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;
/* 线程2入口 */
static void thread2_entry(void *param)
{
    rt_kprintf("thread2: send event1\n");
    rt_event_send(&event, (1 << 3));
    rt_kprintf("thread2 leave.\n");
}

ALIGN(RT_ALIGN_SIZE)
static char thread3_stack[1024];
struct rt_thread thread3;
/* 线程3入口函数 */
static void thread3_entry(void *param)
{
    rt_kprintf("thread3: send event2\n");
    rt_event_send(&event, (1 << 5));

    rt_thread_delay(20);

    rt_kprintf("thread3: send event2\n");
    rt_event_send(&event, (1 << 5));

    rt_kprintf("thread3 leave.\n");
}

int rt_application_init()
{
    rt_thread_t init_thread;
    rt_err_t result;

    /* 初始化事件对象 */
    rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("init event failed.\n");
        return -1;
    }

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    rt_thread_init(&thread1,
                   "thread1",
                   thread1_entry,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),8,50);
    rt_thread_startup(&thread1);


    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),9,50);
    rt_thread_startup(&thread2);


    rt_thread_init(&thread3,
                   "thread3",
                   thread3_entry,
                   RT_NULL,
                   &thread3_stack[0],
                   sizeof(thread3_stack),10,50);
    rt_thread_startup(&thread3);
    return 0;
}

/*@}*/
