/*
 * @Author: SingleBiu
 * @Date: 2023-03-04 19:04:13
 * @LastEditors: SingleBiu
 * @LastEditTime: 2023-03-05 11:30:15
 * @Description: 线程池应用
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "sys/time.h"
#include "../inc/pool.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int g_ticket = 1000000;

void *get_g_ticket(void *arg)
{
    int times = *(int *)arg;

    while (g_ticket > 0 && times > 0)
    {
        if (0 == pthread_mutex_trylock(&mutex))
        {
            printf("g_ticket1:%d\n", --g_ticket);
            pthread_mutex_unlock(&mutex);
            times--;
        }
        else
        {
            // printf("lock is in use 1\n");
        }
    }
}

void *get_g_ticket2(void *arg2)
{
    int times = *(int *)arg2;

    while (g_ticket > 0 && times > 0)
    {
        if (0 == pthread_mutex_trylock(&mutex))
        {
            printf("g_ticket2:%d\n", --g_ticket);
            pthread_mutex_unlock(&mutex);
            times--;
        }
        else
        {
            // printf("lock is in use 2\n");
        }
    }
}

int main(int argc, char const *argv[])
{
    THREAD_POOL *thread_pool = init_pool();

    int arg = 500000;
    int arg2 = 500000;

    struct timeval time_start, time_end;

    gettimeofday(&time_start, NULL);

    add_task(thread_pool, &get_g_ticket, &arg, sizeof(arg));

    add_task(thread_pool, &get_g_ticket2, &arg2, sizeof(arg2));

    sleep(1);

    destory_pool(thread_pool);

    gettimeofday(&time_end, NULL);

    int timeuse = 1000000 * (time_end.tv_sec - time_start.tv_sec) + time_end.tv_usec - time_start.tv_usec;

    printf("Used:%d ms\n", timeuse / 1000);

    return 0;
}
