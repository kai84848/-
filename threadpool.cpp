//
// Created by 凯 on 2026/5/7.
//
#include"threadpool.h"
#include<functional>
#include <future>
#include <iostream>
const int TASK_MAX_THREADHOLD = 1024;

//线程池构造
ThreadPool::ThreadPool()
    :initThreadSizes_(0)
    ,taskSize_(0)
    ,TaskMaxThreadHold_(TASK_MAX_THREADHOLD)
    ,poolMode_(PoolMode::MODE_FIXED)
{}

//线程池析构
ThreadPool::~ThreadPool()
{}

void ThreadPool::setMode(PoolMode mode)//设置线程池的工作模式
{
    poolMode_ = mode;
}

//void ThreadPool::setInitThreadSize(int size)//设置初始线程数量
//{
//    initThreadSizes_ = size;
//}

void ThreadPool::setTaskQueMaxThreshHold(int threshhold)//设置任务队列上限阈值
{
    TaskMaxThreadHold_ = threshhold;
}

void ThreadPool::submitTask(std::shared_ptr<Task> sp)//给线程池提交任务 用户调用该接口传入任务对象，生产任务
{
    //获取锁
    std::unique_lock<std::mutex> lock(taskQueMtx_);

    //线程通信 等待任务队列有空余
    //用户提交任务，最长不能阻塞超过1s，否则判断提交任务失败，返回

    if (!notFull_.wait_for(lock,std::chrono::seconds(1),[&]()->bool{return taskQue_.size()<taskQueMaxThreshold_;})) {
        std::cout<<"task queue is full,submit task failed!"<<std::endl;
        return;
    }

    //如果有空余，把任务放入任务队列
    taskQue_.emplace(sp);
    taskSize_++;

    //因为新放任务，任务队列不空，在notEmpty上进行通知,并分配线程执行任务
    notEmpty_.notify_all();
}

void ThreadPool::start(int initThreadSize)//开启线程池
{
    //记录初始线程个数
    initThreadSizes_ = initThreadSize;

    //创建线程对象
    for (int i = 0; i < initThreadSize; i++)
    {
     //创建线程对象时，把线程函数给到thread线程对象,unique_ptr左值引用的拷贝构造是禁止的
     threads_.emplace_back(std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this)));
    }

    //启动所有线程
    for (int i = 0; i < initThreadSize; i++)
    {
        threads_[i]->start();
    }
}

//定义线程函数 线程池的所有线程从任务队列里消费任务
void ThreadPool::threadFunc()
{
    //std::cout<<"begin threadFunc tid:"<<std::this_thread::get_id()<<std::endl;
    //std::cout<<"end threadFunc"<<std::this_thread::get_id()<<std::endl;
    for (;;)
    {
        std::shared_ptr<Task>task;
        {
            //获取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);

            std::cout<<"tid:"<<std::this_thread::get_id()<<"尝试获取任务..."<<std::endl;

            //等待notEmpty条件
            notEmpty_.wait(lock,[&]()->bool{return taskQue_.size()>0;});

            std::cout<<"tid:"<<std::this_thread::get_id()<<"获取任务成功..."<<std::endl;

            //从任务队列中取一个任务出来
            task = taskQue_.front();
            taskQue_.pop();
            taskSize_--;

            //如果依然有剩余任务，继续通知其他线程执行任务
            if (taskSize_> 0) {
                notEmpty_.notify_all();
            }

            //取出一个任务后通知,通知可以继续提交生产任务
            notFull_.notify_all();

        }//在这里把锁释放掉,防止一直只有一个线程能拿到锁

        //当前线程负责执行这个任务
        if (task!=nullptr) {
            std::any res = task->run();
            if (res.has_value()) {
                std::cout<<"任务有返回值"<<std::endl;

            }
            else {
                std::cout<<"任务无返回值"<<std::endl;
            }


            //执行完一个任务后通知
            notFull_.notify_all();
        }
    }
}


void Thread::start() {
    //创建一个线程
    std::thread t(func_);

    //设置分离线程,c++要求创建线程后，线程必须join()或detach()
    t.detach();
}

Thread::Thread(ThreadFunc func):func_(func)//线程构造
{
}
Thread::~Thread()//线程析构
{
}

