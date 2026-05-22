//
// Created by 凯 on 2026/5/8.
//
#include <any>
#include <iostream>
#include <chrono>
#include<thread>
#include"threadpool.h"
using namespace std;

/*example
// 创建线程任务，从Task中继承run方法
class MyTask : public Task
{
    public:
        std::any run()
        {
        // 线程代码...


        //无返回值
        ret_ =  std::any{};
        return std::any{};

        有返回值
        ret_ = returnNum;
        return returnNum;
        }
}；

// main中创建线程
ThreadPool pool;
pool.start( // 线程数 )
auto myTask = std::make_shared<MyTask>();
pool.submitTask(myTask);
std::this_thread::sleep_for(std::chrono::seconds(1));
std::any res = myTask->getRet();
int val = std::any_cast<int>(res);
std::cout<<"val:"<<val<<std::endl;
*/


class MyTask : public Task {
public:
    //java python中object类是所有其他类的父类，其他所有类型都可以继承object
    //c++17后有Any类是所有其他类的父类，其他所有类型都可以继承any类
    std::any run(){
        std::cout<<"tid:"<<std::this_thread::get_id()<<"begin!"<<std::endl;
        ret_ =  std::any{};
        int a = 10,b = 20;
        int sum = a+b;
        ret_ = sum;
        //std::cout<<"tid:"<<std::this_thread::get_id()<<"end!"<<std::endl;
        return sum;

        //std::this_thread::sleep_for(std::chrono::seconds(5));
        //std::cout<<"tid:"<<std::this_thread::get_id()<<"end!"<<std::endl;
        //ret_ =  std::any{};
        //return std::any{};//无返回值
    }
};

int main() {
    ThreadPool pool;
    pool.start(1);

    auto myTask = std::make_shared<MyTask>();
    pool.submitTask(myTask);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::any res = myTask->getRet();
    int val = std::any_cast<int>(res);
    std::cout<<"val:"<<val<<std::endl;




    getchar();

}
