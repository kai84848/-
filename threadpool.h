//
// Created by 凯 on 2026/5/7.
//
#ifndef THREAD_THREADPOOL_H
#define THREAD_THREADPOOL_H
#include<vector>
#include<queue>
#include<memory>
#include<atomic>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<thread>
#include<any>

//任务抽象基类
//用户可以自定义任务类型，从Task继承，重写run方法，实现自定义任务处理
class Task {
public:
    virtual std::any run() = 0;//纯虚基类用于继承
    std::any getRet() const{return ret_;};
protected:
    std::any ret_;
};

enum class PoolMode {             //enum class枚举,防止枚举内层名称相同时，无法区分来自哪个enum
    MODE_FIXED,//固定数量线程
    MODE_CACHED,//线程池可动态增长
};

//线程类型
class Thread {
public:
    //线程函数对象类型
    using ThreadFunc = std::function<void()>;//using为类型别名类似于typedef，更灵活

    Thread(ThreadFunc func);//线程构造

    ~Thread();//线程析构

    void start();//启动线程
private:
    ThreadFunc func_;
};

//线程池类型
class ThreadPool {
public:
    ThreadPool();//线程池构造

    ~ThreadPool();//线程池析构

    void setMode(PoolMode mode);//设置线程池的工作模式

    //void setInitThreadSize(int size);//设置初始线程数量

    void setTaskQueMaxThreshHold(int threshhold);//设置任务队列上限阈值

    void submitTask(std::shared_ptr<Task> sp);//给线程池提交任务

    void start(int initThreadSize = 4);//开启线程池

    ThreadPool(const ThreadPool&) = delete;

    ThreadPool& operator=(const ThreadPool&) = delete;//禁用类的拷贝构造函数和拷贝赋值运算符

private:
    //定义线程函数
    void threadFunc();
private:
    std::vector<std::unique_ptr<Thread>> threads_;//线程列表
    size_t initThreadSizes_;//初始的线程数量，size_t无符号整数类型，恒大于等于0
    std::queue<std::shared_ptr<Task>> taskQue_;//任务队列，不直接使用裸指针*Task，内存泄漏的风险极高，裸指针本身不负责管理对象的生命周期。任务执行完后，你必须手动调用 delete task; 释放内存。一旦出现异常、线程池提前退出、任务中途抛出异常，delete 就可能执行不到，直接造成内存泄漏。
    std::atomic_int taskSize_;//任务的数量，使用atomic使所有操作为原子的，不会被打断，不需要手动加锁，防止不同多线程时出现数据错乱
    int TaskMaxThreadHold_;//任务队列上限阈值
    int taskQueMaxThreshold_ = 16;//任务队列最大任务数;
    std::mutex taskQueMtx_;//保证任务队列的线程安全，任何线程访问队列前都必须获取这个锁
    std::condition_variable notFull_;//任务队列不满，告诉生产者队列满了
    std::condition_variable notEmpty_;//任务队列不空，告诉消费者队列空了
    PoolMode poolMode_;//当前线程池的工作模式
};




#endif //THREAD_THREADPOOL_H
