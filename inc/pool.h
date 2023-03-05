/*
 * @Author: SingleBiu
 * @Date: 2023-03-04 19:21:18
 * @LastEditors: SingleBiu
 * @LastEditTime: 2023-03-04 19:52:55
 * @Description: 线程池头文件
 */
#if !defined(__POOL_H__)
#define __POOL_H__

#include<pthread.h>
#include<string.h>

//构建任务队列结点
typedef struct task_node
{
    void *(*task)(void *arg);   //函数指针
                                //一个函数就是一个任务
                                //一个函数执行完 那么任务就算完成

    void *arg;                  //指向函数的参数

    int arg_len;                //arg指向对象的长度

    struct task_node *next;     //指向下一个任务结点
} TASK;

//构建线程池的类型
typedef struct thread_pool
{
    pthread_t *p_id;            //线程id集合

    int task_len;               //任务队列长度

    int active_thread;          //活跃的线程数

    int shutdown;               //关机位
                                // 0 进程不结束
                                // 1 进程结束
                                
    TASK *head;                 //任务队列的头结点

    pthread_mutex_t mutex;      //锁 保护共享资源

    pthread_cond_t cond;       //条件变量 用于唤醒线程
} THREAD_POOL;

// 初始化一个线程池 分配一个线程池结构体
THREAD_POOL *init_pool();

// 往线程池的任务队列上添加任务
int add_task(THREAD_POOL *pool, void *(*func)(void *), void *arg, int arg_len);

// 销毁线程池
int destory_pool(THREAD_POOL *pool);

#endif // __POOL_H__
