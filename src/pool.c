/*
 * @Author: SingleBiu
 * @Date: 2023-03-04 19:21:12
 * @LastEditors: SingleBiu
 * @LastEditTime: 2023-03-05 11:15:02
 * @Description: 线程池代码
 */
#include <stdio.h>
#include <stdlib.h>
#include "../inc/pool.h"

#define MAX_THREAD 8 // 线程池最大线程数

/**
 * @description: 线程函数
 * @param {void} *arg   线程函数参数
 * @return {*}
 */
void *func(void *arg)
{
    THREAD_POOL *pool = (THREAD_POOL *)arg;

    while (1)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

        pthread_mutex_lock(&pool->mutex);

        while (pool->task_len == 0 && pool->shutdown == 0) // 无任务执行 且 未关机
        {
            pthread_cond_wait(&pool->cond, &pool->mutex); // 线程睡眠
        }

        // 要么关机 要么执行任务
        if (pool->shutdown == 1)
        {
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(0);
        }

        // 执行任务
        TASK *t = pool->head->next; // 取出开头的任务
        pool->head->next = t->next; // 头结点后移
        t->next = NULL;             // 取下任务结点
        pool->task_len--;           // 任务数量减一

        pthread_mutex_unlock(&pool->mutex); // 之后无需访问共享资源

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        // 执行任务函数
        t->task(t->arg);

        free(t->arg);
        free(t); // 释放申请的空间
    }
}

/**
 * @description: 初始化线程池
 * @return {*}
 */
THREAD_POOL *init_pool()
{
    THREAD_POOL *pool = (THREAD_POOL *)malloc(sizeof(*pool));
    if (NULL == pool)
    {
        perror("malloc error\n");
        return NULL;
    }

    pool->task_len = 0; // 任务队列清空

    pool->shutdown = 0; // 0 表示不关机

    // 创建头结点保证任务队列不为空
    pool->head = malloc(sizeof(TASK));
    if (pool->head == NULL)
    {
        free(pool);
        perror("mallo error");
        return NULL;
    }

    pool->head->next = NULL; // 指向空 表示任务队列为空

    pthread_mutex_init(&pool->mutex, NULL); // 初始化锁
    pthread_cond_init(&pool->cond, NULL);   // 初始化条件变量

    pool->p_id = malloc(sizeof(pthread_t *) * MAX_THREAD); // 线程id集合

    pool->active_thread = 0; // 已创建的线程个数
    int i;                   // 线程创建次数
    int j = 0;               // 线程成功创建次数

    for (i = 0; i < MAX_THREAD; i++)
    {
        if (pthread_create(&pool->p_id[j], NULL, func, pool) != -1)
        {
            j++;
            pool->active_thread++;
            printf("success created thread[%d] pid = %ld\n", j, pool->p_id[j]);
        }
    }

    return pool;
}

/**
 * @description: 添加任务函数
 * @param {THREAD_POOL} *pool     线程池结构体指针
 * @param {void} *(*func)(void *) 函数指针
 * @param {void} *arg             函数参数
 * @param {int} arg_len           函数参数长度
 * @return {int} 0 正常 | -1 失败
 */
int add_task(THREAD_POOL *pool, void *(*func)(void *), void *arg, int arg_len)
{
    if (NULL == pool)
    {
        return -1;
    }

    TASK *new = malloc(sizeof(TASK));

    if (NULL == new)
    {
        perror("malloc error\n");
        return -1;
    }

    new->task = func;               // 添加任务函数
    new->arg = malloc(arg_len);     // 分配参数空间
    memcpy(new->arg, arg, arg_len); // 复制参数到申请的空间
    new->next = NULL;

    pthread_mutex_lock(&pool->mutex); // 保护任务队列

    TASK *task = pool->head;

    while (NULL != task->next)
    {
        task = task->next;
    }
    task->next = new; // 添加任务到队尾

    pool->task_len++; // 任务数加一

    pthread_cond_signal(&pool->cond); // 唤醒一个线程执行任务

    pthread_mutex_unlock(&pool->mutex);

    return 0;
}

/**
 * @description: 销毁线程池
 * @param {THREAD_POOL} *pool 线程池结构体指针
 * @return {int} 0 正常 | -1 失败
 */
int destory_pool(THREAD_POOL *pool)
{
    pool->shutdown = 1;                  // 关机位置1
    pthread_cond_broadcast(&pool->cond); // 唤醒所有线程自杀

    int i;
    for (i = 0; i < pool->active_thread; i++)
    {
        pthread_join(pool->p_id[i],NULL);   //回收线程资源

        printf("joined thread[%d] pid = %ld\n",i,pool->p_id[i]);
    }

    free(pool->p_id);
    free(pool->head);
    free(pool);

    return 0;
}